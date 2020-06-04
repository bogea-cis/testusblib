///////////////////////////////////////////////////////////////////////////////
// Descricao:
// Autor: Amadeu Vilar Filho / Luis Gustavo de Brito
// Data de criacao: 11/12/2007
//
// Alteracoes:
//
///////////////////////////////////////////////////////////////////////////////


#include "CFileReader.h"
#include <string.h>
#ifdef WIN32
#include <windows.h>
	#include <io.h>
#define HANDLE HWND
#endif

CFileReader::CFileReader(const char* file) 
{
	memset(m_temporaryLine, 0, MAX_LINE_LENGHT);
	memset(m_fileName, 0, MAX_FILE_NAME_LENGHT);
	
	strcpy(m_fileName, file);
	m_filePointer = NULL;

	this->m_mutex = new CMutex();
}

CFileReader::~CFileReader() {
	this->close();
	delete this->m_mutex;
}

bool CFileReader::open(const char* openMode) {
	//try to close the file
	this->close();
	
	#ifdef WIN32
	m_mutex->request();
	#else
	m_mutex->request();
	#endif
	
	//open the file
	m_filePointer = fopen(m_fileName, openMode);
	
	
	if (m_filePointer == NULL) {
		m_mutex->release();
	}
	
	//returns the operation's result
	return(m_filePointer != NULL);
}

bool CFileReader::rewind() {
	if(m_filePointer == NULL) {
		return false;
	}
	else {
		//if success, fseek returns 0
		return(fseek(m_filePointer, 0, SEEK_SET) == 0);
	}
}

char* CFileReader::getLine() {
	if(m_filePointer == NULL) {
		return NULL;
	}
	else {
		int buffIndex = 0;
		int firstUselessChar = 0;
		memset(m_temporaryLine, 0, MAX_LINE_LENGHT);
		
		int nextChar = fgetc(m_filePointer);
		while((nextChar != EOF)&&
		(nextChar != '\n')&&
		(nextChar != '\r')) 
		{
			//AMADEU - now we just remove the ' or " delimiters from the returning string
			//igneore all ' and "
			//if((nextChar != '\"')&&(nextChar != '\'')) {
				//trims all left spaces
				if((buffIndex != 0)||((nextChar != ' ')&&(nextChar != '\t')&&(nextChar != 0x00))) 
				{
					m_temporaryLine[buffIndex] = (unsigned char)nextChar;
					if((nextChar == ' ')||(nextChar == '\t')) 
					{
						if(firstUselessChar == 0) 
						{
							firstUselessChar = buffIndex;
						}
					}
					else 
					{
						firstUselessChar = 0;
					}

					buffIndex++;
				}
			//}
			//If the buffer gets full, we ignore the rest of the line
			if(buffIndex >= 1023)
			{
				break;
			}
			nextChar = fgetc(m_filePointer);
		}
		
		if((nextChar == EOF)&&(strlen(m_temporaryLine) <= 0)) {
			return NULL;
		}
		
		//trims all rigth spaces
		if(firstUselessChar != 0) {
			memset(&m_temporaryLine[firstUselessChar], 0, MAX_LINE_LENGHT - firstUselessChar);
		}
		
		//if the first char is '\0', means that it is a useless information
		return m_temporaryLine;
	}
}

bool CFileReader::close() {
	if(m_filePointer == NULL) {
		//release the mutex
		m_mutex->release();
		return true;
	}
	else {
		#ifdef WIN32
		HANDLE  hOut = (HANDLE)_get_osfhandle (_fileno (m_filePointer));
		FlushFileBuffers (hOut);
		#endif
		
		bool ret = (fclose(m_filePointer) == 0);
		m_filePointer = NULL;
		
		//release the mutex
		m_mutex->release();
		return ret;
	}
}
