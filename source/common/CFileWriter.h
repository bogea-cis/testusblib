///////////////////////////////////////////////////////////////////////////////
// Descricao:
// Autor: Amadeu Vilar Filho / Luis Gustavo de Brito
// Data de criacao: 11/12/2007
//
// Alteracoes:
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __C_FILE_WRITER_H__
#define __C_FILE_WRITER_H__

#include "CFileReader.h"
        

class CFileWriter: public CFileReader 
{
	public:
		CFileWriter(const char* file);
		virtual ~CFileWriter();
		bool setLine(const char* lineData);
};


#endif //__C_FILE_WRITER_H__
