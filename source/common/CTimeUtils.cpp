// CTimeUtils.cpp: implementation of the CTimeUtils class.
//
//////////////////////////////////////////////////////////////////////

#include "CTimeUtils.h"

#ifdef WINDOWS
#include <windows.h>
#else
#include <sys/time.h>
#endif

#include <time.h>

CTimeUtils::CTimeUtils ()
{
	m_timeout = 0;
	m_start = 0;
	this->start(0);
}

CTimeUtils::~CTimeUtils()
{

}

void CTimeUtils::msleep(unsigned long milisec)  
{  
#ifdef WINDOWS
	Sleep(milisec);
#else
    struct timespec req={0};  
    time_t sec=(int)(milisec/1000);  
    milisec=milisec-(sec*1000);  
    req.tv_sec=sec;  
    req.tv_nsec=milisec*1000000L;  
    
    while(nanosleep(&req,&req)==-1)  
         continue;  
#endif
} 

void CTimeUtils::start (int timeout)
{
#ifdef WINDOWS
	m_start = GetTickCount();
#else
	struct timeval current;

	gettimeofday (&current, NULL);
	m_start = current.tv_sec * 1000 + current.tv_usec / 1000;
#endif

	m_timeout = timeout;
}

bool CTimeUtils::checkTimeout ()
{
	int now;

#ifdef WINDOWS
	now = GetTickCount();
#else
	struct timeval current;
	gettimeofday (&current, NULL);
	now = current.tv_sec * 1000 + current.tv_usec / 1000;
#endif
		
	if ((now - m_start) > m_timeout)
	{
		return true;
	}
	else
	{
		return false;
	}
}

int CTimeUtils::getElapsedTime ()
{
	int now;

#ifdef WINDOWS
	now = GetTickCount();
#else
	struct timeval current;

	gettimeofday (&current, NULL);
	now = current.tv_sec * 1000 + current.tv_usec / 1000;
#endif
	
	return now - m_start;
}
