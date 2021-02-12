#include "CObjectContainer.h"
#include "CFileUtils.h"

CLogger* CObjectContainer::m_trace = NULL;
CUSBPrinter* CObjectContainer::m_comm = NULL;

void CObjectContainer::initialize()
{
  CObjectContainer::getTraceInstance();
  CObjectContainer::getPrinterInstance();
}


CLogger* CObjectContainer::getTraceInstance()
{
  if( CObjectContainer::m_trace == NULL )
  {
    if( !CFileUtils::isDirectory( "./logs" ) )
    {
      CFileUtils::createDirectoryRecursive( "./logs" );
    }
    CObjectContainer::m_trace = new CLogger( "logPMF", "./logs", 5, 1024 * 1024 * 10, P_TRACE );
  }
  return CObjectContainer::m_trace;
}

CUSBPrinter* CObjectContainer::getPrinterInstance()
{
  if( CObjectContainer::m_comm == NULL )
  {
    CObjectContainer::m_comm = new CUSBPrinter();
  }
  return CObjectContainer::m_comm;
}