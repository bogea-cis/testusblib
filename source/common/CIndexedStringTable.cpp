///////////////////////////////////////////////////////////////////////////////
// Descricao:
// Autor: Amadeu Vilar Filho / Luis Gustavo de Brito
// Data de criacao: 11/12/2007
//
// Alteracoes:
//
///////////////////////////////////////////////////////////////////////////////

#include "CIndexedStringTable.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <CBasicStringList.h>

const unsigned int STRING_TABLE_DEFAULT_BLOCK_LEN = 16;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CIndexedStringTable::CIndexedStringTable()
{
	m_strKey = m_strValue = NULL;
	m_realLen = 0;
	m_usedLen = 0;
	m_index = 0;
	
	//aloca quantidade inicial de memoria
	allocMemory(1);
}

CIndexedStringTable::~CIndexedStringTable()
{
	this->clear();
	delete[] m_strKey;
	delete[] m_strValue;
}

bool CIndexedStringTable::lock ()
{
	return m_mutex.request();
}

void CIndexedStringTable::unlock ()
{
	m_mutex.release();
}

void CIndexedStringTable::invertKeyValue (CIndexedStringTable& output)
{
	output.clear();
	for (unsigned int i = 0; i < m_usedLen; i++)
	{
		output.setValue(m_strValue[i], m_strKey[i]);
	}
}

int CIndexedStringTable::count ()
{
	return m_usedLen;
}

void CIndexedStringTable::resetIterator()
{
	m_index = 0;
}

bool CIndexedStringTable::nextIterator ()
{
	bool ret = false;
	if ((unsigned int)(m_index + 1) < m_usedLen)
	{
		m_index++;
		ret = true;
	}
	return ret;
}

bool CIndexedStringTable::getCurrentKey (char* key)
{
	bool ret = false;
	if ((unsigned int)m_index < m_usedLen)
	{
		strcpy (key, m_strKey[m_index]);
		ret = true;
	}
	return ret;
}

bool CIndexedStringTable::getCurrentValue (char* value)
{
	bool ret = false;
	if ((unsigned int)m_index < m_usedLen)
	{
		strcpy (value, m_strValue[m_index]);
		ret = true;
	}
	return ret;
}

bool CIndexedStringTable::getCurrentKeyValue (char* key, char* value)
{
	bool ret = false;
	if ((unsigned int)m_index < m_usedLen)
	{
		strcpy (key, m_strKey[m_index]);
		strcpy (value, m_strValue[m_index]);
		ret = true;
	}
	return ret;
}

void CIndexedStringTable::setValue (const char* keyAndValue)
{
	const char* p = strchr(keyAndValue, '=');

	if (p != NULL)
	{
		char* key = new char[(p - keyAndValue) + 2];
		memcpy (key, keyAndValue, p - keyAndValue);
		key[p - keyAndValue] = 0;
		
		this->setValue(key, p+1);
		delete[] key;
	}
}

void CIndexedStringTable::setValue (const char* key, const char* value)
{
	if (key == NULL ||
		value == NULL ||
		key[0] == 0)
	{
		return;
	}

	int idx = indexOf(key);
	if (idx >= 0)
	{
		int l = strlen(value);
		delete[] m_strValue[idx];
		m_strValue[idx] = new char[l + 1];
		memcpy (m_strValue[idx], value, l);
		m_strValue[idx][l] = 0;
	}
	else
	{
		//novo item
		int idx = 0;
		if (m_usedLen > 0)
		{
			idx = getInsertionIndex(0, m_usedLen, key);
		}
		this->insert(idx, key, value);
	}
}

bool CIndexedStringTable::removeKey (const char* key)
{
	bool ret = false;
	if (key)
	{
		int idx = indexOf(key);
		if (idx >=0)
		{
			//desloca todos os elementos para a esquerda
			unsigned int i;
			for (i = idx; i < m_usedLen; i++)
			{
				m_strKey[i] = m_strKey[i + 1];
				m_strValue[i] = m_strValue[i + 1];
			}
			delete[] m_strKey[i];
			delete[] m_strValue[i];
			m_usedLen--;
			ret = true;
		}
	}
	return ret;
}

void CIndexedStringTable::clear ()
{
	unsigned int i;
	for (i = 0; i < m_usedLen; i++)
	{
		delete[] m_strKey[i];
		delete[] m_strValue[i];
	}
	m_usedLen = 0;
}

void CIndexedStringTable::serialize (CBuffer& out, char separator)
{
	out.clear();

	for (unsigned int i = 0; i < m_usedLen; i++)
	{
		out.appendFormatted("%s=%s%c", m_strKey[i], m_strValue[i], separator);
	}
	if (out.length() > 0)
	{
		out.removeFromBuffer(out.length() - 1, 1);
	}
}

void CIndexedStringTable::deserialize (CBuffer& in, char separator)
{
	char aux[2] = {0};
	CBasicStringList bslItems;

	aux[0] = separator;
	bslItems.add(in.getAsciiBuffer(), aux);
	this->clear();

	int c = bslItems.count();
	for (int i = 0; i < c; i++)
	{
		this->setValue(bslItems[i]);
	}
}

void CIndexedStringTable::insert(int index, const char* key, const char* value)
{
	if (index < 0 || (unsigned int)index > m_usedLen)
	{
		return;
	}

	allocMemory (1);

	if (m_usedLen == 0)
	{
		int lenKey = strlen(key);
		int lenValue = strlen(value);
		m_strKey[0] = new char[lenKey + 1];
		m_strValue[0] = new char[lenValue + 1];
		memcpy (m_strKey[0], key, lenKey);
		memcpy (m_strValue[0], value, lenValue);
		m_strKey[0][lenKey] = 0;
		m_strValue[0][lenValue] = 0;
		m_usedLen++;
	}
	else
	{
		int i;

		//desloca todos os elementos para a direita
		for (i = m_usedLen; i > index; i--)
		{
			m_strKey[i] = m_strKey[i - 1];
			m_strValue[i] = m_strValue[i - 1];
		}

		int lenKey = strlen(key);
		int lenValue = strlen(value);
		m_strKey[i] = new char[lenKey + 1];
		m_strValue[i] = new char[lenValue + 1];
		memcpy (m_strKey[i], key, lenKey);
		memcpy (m_strValue[i], value, lenValue);
		m_strKey[i][lenKey] = 0;
		m_strValue[i][lenValue] = 0;
		m_usedLen++;
	}

};

int CIndexedStringTable::indexOf(const char* str)
{
	if (m_usedLen <= 0)
	{
		return -1;
	}
	
	return search (0, m_usedLen, str);
}

int CIndexedStringTable::getInsertionIndex (int startIdx, int count, const char* key)
{
	int m = startIdx + (count / 2);

	if (count <= 1)
	{
		if (strcmp(key, m_strKey[startIdx]) > 0)
		{
			return startIdx + 1;
		}
		else
		{
			return startIdx;
		}
	}

	int comp = strcmp(key, m_strKey[m]);
	if (comp >= 0)
	{
		return getInsertionIndex (m, count - (count / 2), key);
	}
	else
	{
		return getInsertionIndex (startIdx, count - (count / 2), key);
	}
}


int CIndexedStringTable::search (int startIdx, int count, const char* key)
{
	int m = startIdx + (count / 2);

	int comp = strcmp(key, m_strKey[m]);
	
	if (count <= 1)
	{
		if (comp == 0)
		{
			return m;
		}
		else
		{
			return -1;
		}
	}
	
	if (comp > 0)
	{
		return search (m, count - (count / 2), key);
	}
	else if (comp < 0)
	{
		return search (startIdx, count - (count / 2), key);
	}
	else
	{
		return m;
	}
}

const char* CIndexedStringTable::getValue (const char* key)
{
	int idx = indexOf(key);
	if (idx >= 0)
	{
		return m_strValue[idx];
	}
	return NULL;
}

const char* CIndexedStringTable::operator[] (const char* key)
{
	return this->getValue(key);
}

/*
void CIndexedStringTable::operator= (const CBuffer& value)
{

}

void CIndexedStringTable::operator= (const char* value)
{
}
*/

void CIndexedStringTable::allocMemory(int numberOfElmtsToInsert)
{
	//verifica se precisa alocar mais memoria
	if (m_usedLen + numberOfElmtsToInsert > m_realLen)
	{
		int numberOfPositions = (m_usedLen + numberOfElmtsToInsert) - m_realLen;
		int numberOfBlocks = 0;

		if (numberOfPositions % STRING_TABLE_DEFAULT_BLOCK_LEN > 0)
		{
			//adiciona 1 para o resto
			numberOfBlocks++;
		}

		numberOfBlocks += (numberOfPositions / STRING_TABLE_DEFAULT_BLOCK_LEN);


		//aloca blocos de memoria
		unsigned int newRealLen = m_realLen + (numberOfBlocks * STRING_TABLE_DEFAULT_BLOCK_LEN);
		char** newArrayKey = new char*[newRealLen];
		char** newArrayValue = new char*[newRealLen];
		
		for (unsigned int i = 0; i < newRealLen; i++)
		{
			if (i < m_usedLen)
			{
				newArrayKey[i] = m_strKey[i];
				newArrayValue[i] = m_strValue[i];
			}
			else
			{
				//espaco novo vai com NULL
				newArrayKey[i] = NULL;
				newArrayValue[i] = NULL;
			}
		}
		
		//deleta o array de ponteiros, pq criamos um outro
		if (m_strKey)
		{
			delete[] m_strKey;
		}
		if (m_strValue)
		{
			delete[] m_strValue;
		}
		
		m_realLen = newRealLen;
		m_strKey = newArrayKey;	
		m_strValue = newArrayValue;	
	}
}

void CIndexedStringTable::freeMemory(int blocks)
{
	unsigned int newLen = m_realLen - (blocks * STRING_TABLE_DEFAULT_BLOCK_LEN);
	char** newArray = NULL;
	
	if (newLen > 0)
	{
		newArray = new char*[newLen];
	}
	
	for (unsigned int i = 0; i < m_realLen; i++)
	{
		if (i < newLen)
		{
			newArray[i] = m_strKey[i];
		}
		else
		{
			if (m_strKey[i])
			{
				delete[] m_strKey[i];
				m_strKey[i] = NULL;
			}
		}
	}
	
	//deleta o array de ponteiros, pq criamos um outro
	delete[] m_strKey;
	
	m_realLen = newLen;
	m_strKey = newArray;
}

void CIndexedStringTable::quickSortMain(char** items, int count)
{
	if (count > 0)
		quickSort(items, 0, count-1);
}

void CIndexedStringTable::quickSort(char** items, int left, int right)
{
	int i, j;
	char *x;
	char* temp;
	
	i = left;
	j = right;
	x = items[(left+right)/2];
	
	//register int factor = (m_acending) ? 1 : (-1);
	register int factor = 1;
	do
	{
		//if (m_caseSensitive)
		{
			while( (factor * strcmp(items[i], x) < 0) && (i < right))
			{i++;}
			while( (factor * strcmp(items[j], x) > 0) && (j > left))
			{j--;}
		}
		/*
		else
		{

			#ifdef _WIN32
			while( (factor * stricmp(items[i], x) < 0) && (i < right))
			#else
			while( (factor * strcasecmp(items[i], x) < 0) && (i < right))
			#endif
			{i++;}

			#ifdef _WIN32
			while( (factor * stricmp(items[j], x) > 0) && (j > left))
			#else
			while( (factor * strcasecmp(items[j], x) > 0) && (j > left))
			#endif

			{j--;}
		}
		*/
		if(i <= j)
		{
			temp = items[i];
			items[i] = items[j];
			items[j] = temp;
			
			i++;
			j--;
		}
	} while(i <= j);
	
	if(left < j)
	{
		quickSort(items, left, j);
	}
	if(i < right)
	{
		quickSort(items, i, right);
	}
}

char* CIndexedStringTable::createBuffer (const char* pStart, const char* pEnd)
{
	char* ret = NULL;

	if (pStart && pEnd && (pEnd > pStart))
	{
		int len = pEnd - pStart;
		ret = new char[ len + 1 ];
		memset (ret, '\0', len + 1);
		memcpy (ret, pStart, len);
	}

	return ret;
}
