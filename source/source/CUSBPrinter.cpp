#include "CUSBPrinter.h"
#include "CObjectContainer.h"
#include "PMFConstants.h"

#define THIS_FILE "CUSBPrinter.cpp"


CUSBPrinter::CUSBPrinter( void)
{
  m_initialized = false;

  m_devsList = NULL;
  m_dev = NULL;
  m_handle = NULL;
  m_interface = 0;
  m_epOut = 0;
  m_epIn = 0;

  m_iManufactorerIdx = 0;
  m_iProductIdx = 0;
  m_iSerialNumIdx = 0;

  m_deviceProductId = 0;
  m_deviceVendorId = 0;

  m_lastCommunicationOk = true;
}

CUSBPrinter::~CUSBPrinter( void)
{
  this->finalize();
}

int CUSBPrinter::initialize()
{
  int ret = libusb_init( NULL );
  if( ret == LIBUSB_SUCCESS )
  {
    m_initialized = true;
  }
  else
  {
    CObjectContainer::getTraceInstance()->logAscii( THIS_FILE, "initialize", P_HIGH, "libusb_init error [%d]", ret );
  }

  return ret;
}

int CUSBPrinter::finalize()
{
  this->close();

  if( m_devsList != NULL )
  {
    libusb_free_device_list( m_devsList, 1 );
    m_devsList = NULL;
  }

  if( m_initialized )
  {
    libusb_exit( NULL );
    m_initialized = false;
  }

  return UPC_SUCCESS;
}

int CUSBPrinter::open( unsigned short vendorId, unsigned short productId )
{
  m_dev = this->findDevice( vendorId, productId );
  if( m_dev == NULL )
  {
    return UPC_ERROR_NO_DEVICE;
  }

  m_deviceProductId = productId;
  m_deviceVendorId = vendorId;

  int ret = libusb_open( m_dev, &m_handle );
  if( ret == UPC_SUCCESS )
  {
    this->disableKernelDriver();

    ret = libusb_claim_interface( m_handle, 0 );
    if( ret == UPC_SUCCESS )
    {
      if( m_devsList != NULL )
      {
        libusb_free_device_list( m_devsList, 1 );
        m_devsList = NULL;
      }

      ret = this->getEndPoints( &m_epOut, &m_epIn );
      if( ret != UPC_SUCCESS )
      {
        CObjectContainer::getTraceInstance()->logAscii( THIS_FILE, "open", P_HIGH, "Libusb error [%d]", ret );
      }
    }
    else
    {
      CObjectContainer::getTraceInstance()->logAscii( THIS_FILE, "open", P_HIGH, "libusb_claim_interface error [%d] - %s", ret, libusb_error_name( ret ) );
    }
  }
  else
  {
    CObjectContainer::getTraceInstance()->logAscii( THIS_FILE, "open", P_HIGH, "libusb_open error [%d] - %s", ret, libusb_error_name( ret ) );
  }

  return ret;
}

int CUSBPrinter::close()
{
  if( m_handle != NULL )
  {
    libusb_release_interface( m_handle, 0 );

    m_epIn = 0;
    m_epOut = 0;

    libusb_close( m_handle );

    m_handle = NULL;
  }

  return UPC_SUCCESS;
}

int CUSBPrinter::send( unsigned char* data, int dataLen )
{
  this->reconnectionCheck();

  int ret = 0;
  if( m_handle != NULL && m_epOut != 0 )
  {
    int sent = 0;

    ret = libusb_bulk_transfer( m_handle, m_epOut, data, dataLen, &sent, DEFAULT_TIMEOUT );
    if( ret == UPC_SUCCESS &&
      sent == dataLen )
    {
      return UPC_SUCCESS;
    }
    else
    {
      CObjectContainer::getTraceInstance()->logAscii( THIS_FILE, "send", P_HIGH, "libusb_bulk_transfer error [%d] - %s", ret, libusb_error_name( ret ) );
    }
  }
  else
  {
    CObjectContainer::getTraceInstance()->logAscii( THIS_FILE, "send", P_HIGH, "Error accessing usb connection, is the device really plugged?" );
    ret = UPC_ERROR_ACCESS;
  }

  m_lastCommunicationOk = false;
  return ret;
}

int CUSBPrinter::receive( unsigned char* data, int dataLen, unsigned int timeout )
{
  this->reconnectionCheck();

  int ret = 0;
  if( m_handle != NULL && m_epIn != 0 )
  {
    int readed = 0;

    ret = libusb_bulk_transfer( m_handle, m_epIn, data, dataLen, &readed, timeout );
    if( ret == UPC_SUCCESS )
    {
      return ret;
    }
    else
    {
      CObjectContainer::getTraceInstance()->logAscii( THIS_FILE, "receive", P_HIGH, "libusb_bulk_transfer error [%d] - %s", ret, libusb_error_name( ret ) );
    }
  }
  else
  {
    CObjectContainer::getTraceInstance()->logAscii( THIS_FILE, "receive", P_HIGH, "Error accessing usb connection, is the device really plugged?" );
    ret = UPC_ERROR_ACCESS;
  }

  m_lastCommunicationOk = false;
  return ret;
}

unsigned char CUSBPrinter::getStatus( void)
{
  unsigned char status = 0xFF;

  if( m_handle != NULL )
  {
    int ret = libusb_control_transfer( m_handle, GET_PORT_STATUS, 1, 0, m_interface, &status, 1, DEFAULT_TIMEOUT );
    if( ret != 1 )
    {
      CObjectContainer::getTraceInstance()->logAscii( THIS_FILE, "getStatus", P_HIGH, "libusb_control_transfer error [%d] - %s", ret, libusb_error_name( ret ) );
    }
  }

  return status;
}

int CUSBPrinter::getInfo( unsigned char* info, unsigned short* infoLen )
{
  int ret = UPC_ERROR_OTHER;

  if( m_handle != NULL )
  {
    ret = libusb_control_transfer( m_handle, GET_DEVICE_ID, 0, 0, m_interface, info, *infoLen, DEFAULT_TIMEOUT );
    if( ret != *infoLen )
    {
      CObjectContainer::getTraceInstance()->logAscii( THIS_FILE, "getInfo", P_HIGH, "libusb_control_transfer error [%d] - %s", ret, libusb_error_name( ret ) );
    }

    *infoLen = ret;
  }

  return ret;
}

int CUSBPrinter::getManufactorer( unsigned char* manufactorer, unsigned short* manufactorerLen )
{
  this->reconnectionCheck();

  int ret = 0;
  if( m_handle != NULL )
  {
    ret = libusb_get_string_descriptor_ascii( m_handle, m_iManufactorerIdx, manufactorer, *manufactorerLen );
    if( ret > 0 )
    {
      *manufactorerLen = ret;
    }
    else
    {
      CObjectContainer::getTraceInstance()->logAscii( THIS_FILE, "getManufactorer", P_HIGH, "libusb_get_string_descriptor_ascii error [%d] - %s", ret, libusb_error_name( ret ) );
      m_lastCommunicationOk = false;
    }
  }
  else
  {
    CObjectContainer::getTraceInstance()->logAscii( THIS_FILE, "getManufactorer", P_HIGH, "Error accessing usb connection, is the device really plugged?" );
    ret = UPC_ERROR_ACCESS;
  }

  return ret;
}

int CUSBPrinter::getProduct( unsigned char* product, unsigned short* productLen )
{
  this->reconnectionCheck();

  int ret = 0;
  if( m_handle != NULL )
  {
    ret = libusb_get_string_descriptor_ascii( m_handle, m_iProductIdx, product, *productLen );
    if( ret > 0 )
    {
      *productLen = ret;
    }
    else
    {
      //CObjectContainer::getTraceInstance()->logAscii( THIS_FILE, "getProduct", P_HIGH, "libusb_get_string_descriptor_ascii error [%d] - %s", ret, libusb_error_name( ret ) );
      m_lastCommunicationOk = false;
    }
  }
  else
  {
    //CObjectContainer::getTraceInstance()->logAscii( THIS_FILE, "getProduct", P_HIGH, "Error accessing usb connection, is the device really plugged?" );
    ret = UPC_ERROR_ACCESS;
  }

  return ret;
}

int CUSBPrinter::getSerialNumber( unsigned char* sn, unsigned short* snLen )
{
  this->reconnectionCheck();

  int ret = 0;
  if( m_handle != NULL )
  {
    ret = libusb_get_string_descriptor_ascii( m_handle, m_iSerialNumIdx, sn, *snLen );
    if( ret > 0 )
    {
      *snLen = ret;
    }
    else
    {
      //CObjectContainer::getTraceInstance()->logAscii( THIS_FILE, "getSerialNumber", P_HIGH, "libusb_get_string_descriptor_ascii error [%d] - %s", ret, libusb_error_name( ret ) );
      m_lastCommunicationOk = false;
    }
  }
  else
  {
    //CObjectContainer::getTraceInstance()->logAscii( THIS_FILE, "getSerialNumber", P_HIGH, "Error accessing usb connection, is the device really plugged?" );
    ret = UPC_ERROR_ACCESS;
  }

  return ret;
}

int CUSBPrinter::reset()
{
  int ret = UPC_ERROR_OTHER;

  if( m_handle != NULL )
  {
    ret = libusb_control_transfer( m_handle, SOFT_RESET, 2, 0, m_interface, NULL, 0, DEFAULT_TIMEOUT );
    if( ret != 0 )
    {
      //CObjectContainer::getTraceInstance()->logAscii( THIS_FILE, "reset", P_HIGH, "libusb_control_transfer error [%d] - %s", ret, libusb_error_name( ret ) );
    }

    ret = libusb_reset_device( m_handle );
    if( ret == LIBUSB_ERROR_NOT_FOUND )
    {
      this->close();
      this->open( m_deviceVendorId, m_deviceProductId );
    }
    //CObjectContainer::getTraceInstance()->logAscii( THIS_FILE, "reset", P_HIGH, "libusb_reset_device ret [%d] - %s", ret, libusb_error_name( ret ) );

    if( m_handle != NULL &&
      m_epIn != 0 && m_epOut != 0 )
    {
      libusb_clear_halt( m_handle, m_epIn );
      libusb_clear_halt( m_handle, m_epOut );
    }
  }

  return ret;
}

bool CUSBPrinter::statusPaperPresent()
{
  unsigned char status = this->getStatus();

  if( status != 0xFF )
  {
    return ((status & 0x20) == 0x00);
  }

  return false;
}

bool CUSBPrinter::statusSelected()
{
  unsigned char status = this->getStatus();

  if( status != 0xFF )
  {
    return ((status & 0x10) == 0x10);
  }

  return false;
}

bool CUSBPrinter::statusError()
{
  unsigned char status = this->getStatus();

  if( status != 0xFF )
  {
    return ((status & 0x08) == 0x00);
  }

  return true;
}

void CUSBPrinter::disableKernelDriver()
{
  int ret = libusb_kernel_driver_active( m_handle, 0 );
  if( ret == 1 )
  {
    libusb_detach_kernel_driver( m_handle, 0 );
  }
}

libusb_device* CUSBPrinter::findDevice( unsigned short vendorId, unsigned short productId )
{
  ssize_t ret = 0;

  if( m_devsList != NULL )
  {
    libusb_free_device_list( m_devsList, 1 );
  }

  ret = libusb_get_device_list( NULL, &m_devsList );
  if( ret > 0 )
  {
    int devCount = 0;

    while( m_devsList[devCount] != NULL )
    {
      struct libusb_device_descriptor desc;

      ret = libusb_get_device_descriptor( m_devsList[devCount], &desc );
      if( ret == 0 )
      {
        if( desc.idVendor == vendorId &&
          desc.idProduct == productId )
        {
          m_iManufactorerIdx = desc.iManufacturer;
          m_iProductIdx = desc.iProduct;
          m_iSerialNumIdx = desc.iSerialNumber;

          return m_devsList[devCount];
        }
      }
      else
      {
        //CObjectContainer::getTraceInstance()->logAscii( THIS_FILE, "reset", P_HIGH, "libusb_get_device_descriptor error [%d] - %s", ret, libusb_error_name( ret ) );
      }

      devCount++;
    }
  }
  else
  {
    //CObjectContainer::getTraceInstance()->logAscii( THIS_FILE, "reset", P_HIGH, "libusb_get_device_list error [%d] - %s", ret, libusb_error_name( ret ) );
  }

  return NULL;
}

int CUSBPrinter::getEndPoints( unsigned char* epOut, unsigned char* epIn )
{
  libusb_config_descriptor* config = NULL;

  int ret = libusb_get_active_config_descriptor( m_dev, &config );
  if( ret == UPC_SUCCESS )
  {
    for( int i = 0; i < config->bNumInterfaces; i++ )
    {
      for( int j = 0; j < config->interface[i].num_altsetting; j++ )
      {
        if( config->interface[i].altsetting[j].bInterfaceClass == LIBUSB_CLASS_PRINTER )
        {
          for( int k = 0; k < config->interface[i].altsetting[j].bNumEndpoints; k++ )
          {
            unsigned char endpoint = config->interface[i].altsetting[j].endpoint[k].bEndpointAddress;

            if( (endpoint & ENDPOINT_DIRECTION_MASK) == ENDPOINT_OUT )
            {
              *epOut = endpoint;
            }
            else
            {
              *epIn = endpoint;
            }

            if( *epOut != 0 && *epIn != 0 )
            {
              m_interface = config->interface[i].altsetting[j].bInterfaceNumber;

              libusb_clear_halt( m_handle, *epIn );
              libusb_clear_halt( m_handle, *epOut );

              return UPC_SUCCESS;
            }
          }
        }
      }
    }
  }

  //CObjectContainer::getTraceInstance()->logAscii( THIS_FILE, "reset", P_HIGH, "libusb_get_active_config_descriptor error [%d] - %s", ret, libusb_error_name( ret ) );
  return ret;
}

void CUSBPrinter::reconnectionCheck()
{
  if( m_lastCommunicationOk == false )
  {
    //CObjectContainer::getTraceInstance()->logAscii( THIS_FILE, "reconnectionCheck", P_HIGH, "Connection problem, trying to reconnect" );
    this->close();
    if( this->open( m_deviceVendorId, m_deviceProductId ) == UPC_SUCCESS )
    {
      //CObjectContainer::getTraceInstance()->logAscii( THIS_FILE, "reconnectionCheck", P_TRACE, "Connection restablished" );
      m_lastCommunicationOk = true;
    }
    else
    {
      //CObjectContainer::getTraceInstance()->logAscii( THIS_FILE, "reconnectionCheck", P_HIGH, "Error reconnecting" );
    }
  }
}
