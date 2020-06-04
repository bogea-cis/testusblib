///////////////////////////////////////////////////////////////////////////////
// Descricao:
// Autor: Amadeu Vilar Filho / Luis Gustavo de Brito
// Data de criacao: 11/12/2007
//
// Alteracoes:
//
///////////////////////////////////////////////////////////////////////////////

// CMutex.h: interface for the CMutex class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _XHSTAPI_CMUTEX_H_
#define _XHSTAPI_CMUTEX_H_

class CMutex 
{
    public:
        //constructor
        CMutex();
        
        //destrutor
        virtual ~CMutex();
        
        //requisita mutex
        bool request();
        
        //tenta requisitar mutex
        bool tryRequest();
        
        //libera mutex
        bool release();
        
    private:
        void* m_mutex;
        void* m_mutexattr;


};

#endif
