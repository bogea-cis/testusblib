///////////////////////////////////////////////////////////////////////////////
// Descricao:
// Autor: Amadeu Vilar Filho / Luis Gustavo de Brito
// Data de criacao: 11/12/2007
//
// Alteracoes:
//
///////////////////////////////////////////////////////////////////////////////

#include "CFileWriter.h"
#include <string.h>


CFileWriter::CFileWriter(const char* file): CFileReader(file) {
}

CFileWriter::~CFileWriter() {
}

bool CFileWriter::setLine(const char* lineData) {
	if(m_filePointer == NULL) {
		return false;
	}
	else {
		char mask[5] = {0};
		strcpy(mask, "%s");
		char lastChar = lineData[strlen(lineData) - 1];
		if((lastChar != '\n')&&(lastChar != '\r')) {
			strcat(mask, "\n");
		}
		
		//writes it down
		return(fprintf(m_filePointer, mask, lineData) > 0);
	}
	
}
