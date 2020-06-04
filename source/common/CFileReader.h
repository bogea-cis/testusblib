///////////////////////////////////////////////////////////////////////////////
// Descricao:
// Autor: Amadeu Vilar Filho / Luis Gustavo de Brito
// Data de criacao: 11/12/2007
//
// Alteracoes:
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __C_FILE_READER_H__
#define __C_FILE_READER_H__

#include <stdio.h>
#include "CMutex.h"

const int MAX_LINE_LENGHT = 1024;
const int MAX_FILE_NAME_LENGHT = 256;

//open modes
const char OM_READ_ONLY[] = "rb";
const char OM_READ_WRITE[] = "rb+";
const char OM_WRITE[] = "wb+";

class CFileReader 
{
	public:
		CFileReader(const char* file);
		virtual ~CFileReader();
		virtual bool open(const char* openMode);
		virtual bool rewind();
		char* getLine();
		virtual bool close();
		
		protected:
			FILE* m_filePointer;
			CMutex* m_mutex;
			char m_temporaryLine[MAX_LINE_LENGHT];
			char m_fileName[MAX_FILE_NAME_LENGHT];
};
#endif //__C_FILE_READER_H__
