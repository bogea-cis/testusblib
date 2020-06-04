///////////////////////////////////////////////////////////////////////////////
// Descricao:
// Autor: Amadeu Vilar Filho / Luis Gustavo de Brito
// Data de criacao: 11/12/2007
//
// Alteracoes:
//
///////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include "CSectionContainer.h"

CSectionContainer::CSectionContainer(const char* name) 
{
	m_sem = new CMutex();
	m_containerIndex = 0;
	
	strcpy(m_name, name);

	m_key.clear();
	m_value.clear();
}

CSectionContainer::~CSectionContainer() 
{
	this->clearKeys();
	delete m_sem;
}
bool CSectionContainer::setKeyAndValue(const char* name, const char* value)
{
	lock();
	bool ret = false;

	//verifica se já não existe
	if (m_key.indexOf(name) == -1)
	{
		m_key.add(name);
		m_value.add(value);
		ret = true;
	}
	else
	{
	
		int iIndex = this->m_key.indexOf(name);

		if(iIndex >= 0)
		{
			this->m_key.update(iIndex, name);
			this->m_value.update(iIndex, value);
			ret = true;
		}
	}
	unlock();
	return ret;
}

void CSectionContainer::insertKey(const char* name, const char* value) 
{
	lock();

	//verifica se já não existe
	if (m_key.indexOf(name) == -1)
	{
		m_key.add(name);
		m_value.add(value);
	}

	unlock();
}

bool CSectionContainer::removeKey(const char* name)
{
	bool ret = false;

	lock();

	//verifica se existe
	int index = m_key.indexOf(name);
	if (index >= 0)
	{
		m_key.remove( index );
		m_value.remove( index );
		ret = true;
	}

	unlock();

	return ret;
}

const char* CSectionContainer::getKeyValue(const char* name) 
{
	const char* ret = NULL;

	lock();
	int idx = m_key.indexOf(name);

	if (idx >= 0)
	{
		ret = m_value[idx];
	}

	unlock();
	return ret;
}

void CSectionContainer::clearKeys() 
{
	lock();

	m_key.clear();
	m_value.clear();

	unlock();

}

bool CSectionContainer::getFirstKeyPair(char* name, char* value) 
{
	bool ret = false;
	m_containerIndex = 0;

	lock();

	if (m_key.count() > 0){
		strcpy(name, m_key[m_containerIndex]);
		strcpy(value, m_value[m_containerIndex]);
		ret =  true;
	}

	unlock();

	return ret;
}

bool CSectionContainer::getNextKeyPair(char* name, char* value) 
{
	bool ret = false;

	lock();

	m_containerIndex++;
	if(m_containerIndex < m_key.count()) {
		strcpy(name, m_key[m_containerIndex]);
		strcpy(value, m_value[m_containerIndex]);
		ret =  true;
	}

	unlock();

	return ret;
}

char* CSectionContainer::getName() {
	return m_name;
}

bool CSectionContainer::updateValue(const char* key, const char* value)
{
	bool ret = false;

	lock();
	
	int iIndex = this->m_key.indexOf(key);

	if(iIndex >= 0)
	{
		this->m_key.update(iIndex, key);
		this->m_value.update(iIndex, value);
		ret = true;
	}
	else
	{
		this->m_key.add(key);
		this->m_value.add(value);
		ret = true;
	}

	unlock();

	return ret;
}

void CSectionContainer::lock ()
{
#ifdef WIN32
	m_sem->request();
#else
	m_sem->request();
#endif
}

void CSectionContainer::unlock ()
{
	m_sem->release();
}

int CSectionContainer::count()
{
	return m_key.count();
}

