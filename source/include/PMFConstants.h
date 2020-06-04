#ifndef PMFCONSTANTS_H_
#define PMFCONSTANTS_H_

#define VENDORID  0x1781
#define PRODUCTID 0x0e88


/* Commands constants */
#define VALIDATION_MODE_OFF				0
#define VALIDATION_MODE_ON				1

#define DOCUMENT_MODE_OFF				0
#define DOCUMENT_MODE_ON				1

#define CHARACTER_ALIGNMENT_LEFT		0
#define CHARACTER_ALIGNMENT_CENTER		1

#define CARRIAGE_RETURN_DISABLE			0
#define CARRIAGE_RETURN_ENABLE			1

#define CHARACTER_TABLE_ABICOMP			0x31
#define CHARACTER_TABLE_CODEPAGE850		0x32
#define CHARACTER_TABLE_CODEPAGE437		0x33

#define TEXT_OPERATION_OFF				0
#define TEXT_OPERATION_ON				1

#define ACTIVATE_TEXT_SUPERSCRIPT		0
#define ACTIVATE_TEXT_SUBSCRIPT			1
#define DEACTIVATE_TEXT_SCRIPT			2

#define COLUMNS_NUMBER_DEFAULT			0
#define COLUMNS_NUMBER_48				1
#define COLUMNS_NUMBER_64				2

#define DOUBLE_WIDTH_CHARS_OFF			0
#define DOUBLE_WIDTH_CHARS_ON			1

#define PAPER_CUT_TOTAL					0
#define PAPER_CUT_PARTIAL				1

#define SO								0x0E
#define SI								0x0F
#define DC2								0x12
#define DC4								0x14
#define ESC								0x1B
#define LF								0x0A
#define CR								0x0D
#define US								0x1F
#define CIRCUMFLEX						0x5E

#define BBX_TMF_PTR_IO_ENABLEETX		1
#define BBX_TMF_PTR_IO_DISABLEETX		2


/* USB comms constants */
#define DEFAULT_TIMEOUT			 1000

#define UPC_SUCCESS				 0
#define UPC_ERROR_IO			 -1
#define UPC_ERROR_INVALID_PARAM	 -2
#define UPC_ERROR_ACCESS		 -3
#define UPC_ERROR_NO_DEVICE		 -4
#define UPC_ERROR_NOT_FOUND		 -5
#define UPC_ERROR_BUSY			 -6
#define UPC_ERROR_TIMEOUT		 -7
#define UPC_ERROR_OVERFLOW		 -8
#define UPC_ERROR_PIPE			 -9
#define UPC_ERROR_INTERRUPTED	 -10
#define UPC_ERROR_NO_MEM		 -11
#define UPC_ERROR_NOT_SUPPORTED	 -12
#define UPC_ERROR_OTHER			 -99

#define ENDPOINT_DIRECTION_MASK	 0x80
#define ENDPOINT_OUT			 0x00 // To Device
#define ENDPOINT_IN				 0x80 // To Host

#define GET_DEVICE_ID	0xA1
#define GET_PORT_STATUS	0xA1
#define SOFT_RESET		0x21

#endif /*PTRCONSTANTS_H_*/
