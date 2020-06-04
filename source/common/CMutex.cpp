///////////////////////////////////////////////////////////////////////////////
// Descricao:
// Autor: Amadeu Vilar Filho / Luis Gustavo de Brito
// Data de criacao: 11/12/2007
//
// Alteracoes:
//
///////////////////////////////////////////////////////////////////////////////

// CMutex.cpp: implementation of the CMutex class.
//
//////////////////////////////////////////////////////////////////////

#include "CMutex.h"
#include <stdio.h>
#include <pthread.h>

//constructor
CMutex::CMutex() 
{
	pthread_mutex_t* new_mutex = new pthread_mutex_t;
   	if (pthread_mutex_init(new_mutex, NULL) != 0)
	{
        //m_mutex = NULL;
        //m_mutexattr	= NULL;
    }
    else {
    	m_mutex = (void*)new_mutex;
        pthread_mutexattr_init((pthread_mutexattr_t*)&m_mutexattr);
        pthread_mutexattr_settype((pthread_mutexattr_t*)&m_mutexattr, PTHREAD_MUTEX_ERRORCHECK);
    }
    
}


//destrutor
CMutex::~CMutex() 
{
    pthread_mutexattr_destroy((pthread_mutexattr_t*)&m_mutexattr);
    pthread_mutex_destroy((pthread_mutex_t*)m_mutex);
}

//requisita mutex
bool CMutex::request() {
    int rc;
    bool ret = false;
    
    rc = pthread_mutex_lock((pthread_mutex_t*)m_mutex);
    
    if (rc == 0) {
        //pegamos o mutex
        ret = true;
    }
    else {
        //erro
        ret = false;
    }
    
    return ret;
}

//requisita mutex
bool CMutex::tryRequest() {
    int rc;
    bool ret = false;
    
    
    rc = pthread_mutex_trylock((pthread_mutex_t*)m_mutex);
    
    if (rc == 0) {
        //pegamos o mutex
        ret = true;
    }
    else {
        //erro
        ret = false;
    }
    
    return ret;
}

//libera mutex
bool CMutex::release() {
    bool ret = false;
    
    if (pthread_mutex_unlock((pthread_mutex_t*)m_mutex) == 0) {
        ret = true;
    }
    
    return ret;
}
