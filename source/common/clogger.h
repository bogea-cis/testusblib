///////////////////////////////////////////////////////////////////////////////
// Descricao:
// Autor: Amadeu Vilar Filho / Luis Gustavo de Brito
// Data de criacao: 11/12/2007
//
// Alteracoes:
//
///////////////////////////////////////////////////////////////////////////////

// CLogger.h: interface for the CLogger class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _HSTAPI_CLOGGER_H_
#define _HSTAPI_CLOGGER_H_

#include <stdio.h>
#include <stdarg.h>
#include "CMutex.h"
#include "CBasicString.h"
#include "CTimeUtils.h"

const char CLOGGER_EXTENSION[] = "log";
const char CLOGGER_BAK_EXTENSION[] = "bak";
const int CLOGGER_FILENAME_MAX_LENGHT = 256;
const int CLOGGER_BASEPATH_MAX_LENGHT = 256;
const char HSTAPI_HIGH_LIMIT_TRACER[] = "c:\\tracerhcount";
const int TRACER_TIMEOUT_LIMIT = 30000; // 1/2 minute

const int DEFAULT_MAX_FILES = 5;

//~ a floppy with 1,44 Mb
const int DEFAULT_MAX_FILE_LEN = (1024 * 1024) + (1024 * 350);


#define LOG_HABILITA_PRINTF	"c:\\habilitaprintf" // If this file is found, then application will enable printf() logs to stdout

enum LOG_PRIORITY {
            P_TRACE = 0,
            P_LOW,
            P_MEDIUM,
            P_HIGH
        };

class CLogger 
{
	public:

		CLogger(const char* fileName, const char* basePath, unsigned int numberOfBackups = DEFAULT_MAX_FILES, unsigned int maxFileSize = DEFAULT_MAX_FILE_LEN, LOG_PRIORITY priority = P_TRACE);
		virtual ~CLogger();

		virtual void logAsciiIfChange(const char* key, const char* fileName, const char* functionName, LOG_PRIORITY priority, const char* format, ...);
		virtual void logAscii(LOG_PRIORITY priority, const char* format, ...);
		virtual void logAscii(const char* fileName, const char* functionName, LOG_PRIORITY priority, const char* format, ...);
		virtual void vLogAscii(const char* fileName, const char* functionName, LOG_PRIORITY priority, const char* format, va_list args);
		virtual void logHex(const char* fileName, const char* functionName, LOG_PRIORITY priority, const unsigned char* hexMessage, unsigned int len);
		virtual void logHex(const char* fileName, const char* functionName, LOG_PRIORITY priority, const char* asciiHeader, const unsigned char* hexMessage, unsigned int len);

		virtual LOG_PRIORITY getPriority();
		virtual void setPriority(LOG_PRIORITY priority);

		virtual void setFileName(const char* fileName);
		virtual void setBasePath(const char* basePath);
		virtual void setNumberOfBackups(unsigned int number);
		virtual void setMaxFileSize(unsigned int max);

	protected:
		virtual FILE* loadFile();
		virtual void unloadFile();
		virtual void doBackup();
		virtual unsigned int getFileLength(FILE* fp);
		virtual void initialize(const char* fileName, const char* basePath, unsigned int numberOfBackups, unsigned int maxFileSize, LOG_PRIORITY priority);
		virtual bool isDuplicate(const char* fileName, const char* functionName, 
									const char* format, va_list args);

	private:
		void setFileName(const char* fileName, bool updateFullLogName);
		void setBasePath(const char* basePath, bool updateFullLogName);
		void updateFullLogName();
		unsigned int crc32b(unsigned char *message);

	protected:
		char m_fileName[CLOGGER_FILENAME_MAX_LENGHT];
		char m_fullLogName[CLOGGER_BASEPATH_MAX_LENGHT + CLOGGER_FILENAME_MAX_LENGHT];
		char m_basePath[CLOGGER_BASEPATH_MAX_LENGHT];
		char m_isDuplicateMessage[2048]; //variavel colocada aqui por questoes de performance par anao precisar ficar alocando 2 mil bytes a cada chamada de log
		unsigned int m_numberOfBackups;
		unsigned int m_maxFileSize;
		LOG_PRIORITY m_priority;
		CMutex	m_logMutex;
		bool m_debug;
		CBasicString* m_lastFileName;
		CBasicString* m_lastFunctionName;
		CBasicString* m_lastLog;
		int m_opCount;
		FILE* m_fp;
		int m_defaultOperationCountLimit;
		CTimeUtils m_timer;
};

#endif // !defined(AFX_CLOGGER_H__49AA5F1C_1F2E_4564_9241_F8BD67F82C23__INCLUDED_)
