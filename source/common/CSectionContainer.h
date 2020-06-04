//////////////////////////////////////////////////////////////////////////////
// Descricao:
// Autor: Amadeu Vilar Filho / Luis Gustavo de Brito
// Data de criacao: 11/12/2007
//
// Alteracoes:
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __C_SECTION_CONTAINER_H__
#define __C_SECTION_CONTAINER_H__

#include <string.h>

#ifdef _WIN32
	#include "CMutex.h"
#endif
#include "CBasicStringList.h"

        
//define some useful types
struct CKeyNameComparer 
{
	bool operator()(const char* key1, const char* key2 ) const 
	{
		return(strcmp( key1, key2 ) < 0);
	}
};

class CSectionContainer 
{
    public:
        CSectionContainer(const char* name);
        virtual ~CSectionContainer();
        
        void insertKey(const char* name, const char* value);
		bool setKeyAndValue(const char* name, const char* value);
		bool removeKey(const char* name);
        const char* getKeyValue(const char* name);		
        void clearKeys();
        bool getFirstKeyPair(char* name, char* value);
        bool getNextKeyPair(char* name, char* value);
        char* getName();

		int count();

		bool updateValue(const char* key, const char* value);
		
		void lock ();
		void unlock ();

	private:
		CBasicStringList m_key;
		CBasicStringList m_value;
		int m_containerIndex;
		CMutex* m_sem;

        char m_name[100];
};


#endif //__C_SECTION_CONTAINER_H__
