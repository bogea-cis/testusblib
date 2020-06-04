// TestUSBLib.cpp : Defines the entry point for the console application.
//

#include <iostream>

#include <string.h>

#include "CObjectContainer.h"
#include "PMFConstants.h"


int main(int argc, char *argv[])
{
	std::cout << "---inicio---" << std::endl;
	CObjectContainer::initialize();
	CUSBPrinter* m_comm = CObjectContainer::getPrinterInstance();

	int iRet = m_comm->initialize();
	printf( "m_comm->initialize = %d\n", iRet);
	if( iRet == UPC_SUCCESS )
	{
		iRet = m_comm->open( VENDORID, PRODUCTID );
		printf( "m_comm->open = %d\n", iRet);
		if( iRet == UPC_SUCCESS )
		{
			std::cout << "abertura ok!" << std::endl;
		}
	}


	return 0;
}

