///////////////////////////////////////////////////////////////////////////////
// Descricao:
// Autor: Amadeu Vilar Filho / Luis Gustavo de Brito
// Data de criacao: 11/12/2007
//
// Alteracoes:
//
///////////////////////////////////////////////////////////////////////////////

#include "CBasicStringList.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CBasicStringList::CBasicStringList()
{
	m_strArray = NULL;
	m_realLen = 0;
	m_usedLen = 0;
	m_acending = true;
	m_caseSensitive = false;
	
	//aloca quantidade inicial de memoria
	allocMemory(1);
}

CBasicStringList::~CBasicStringList()
{
	clear();
}

CBasicStringList* CBasicStringList::create(const char* strList, const char* separators)
{
	if (strList == NULL || 
			separators == NULL)
	{
		return NULL;
	}

	CBasicStringList* ret = new CBasicStringList();
	ret->deserialize(strList, separators);
	return ret;
}

CBasicStringList* CBasicStringList::create(const char* strList, 
													   const unsigned char* separator, 
													   int separatorLen, 
													   const unsigned char* terminator, 
													   int terminatorLen)
{
	if (strList == NULL || 
			separator == NULL ||
			terminator == NULL ||
			separatorLen <= 0 ||
			terminatorLen <= 0)
	{
		return NULL;
	}

	CBasicStringList* ret = new CBasicStringList();
	ret->deserializeBin(strList, 
						   separator, 
						   separatorLen, 
						   terminator, 
						   terminatorLen);
	return ret;
}

bool CBasicStringList::lock ()
{
	return m_mutex.request();
}

void CBasicStringList::unlock ()
{
	m_mutex.release();
}

void CBasicStringList::add(const char* str)
{
	if (str)
	{
		//verifica se precisa alocar mais memoria
		if (m_usedLen + 1 > m_realLen)
		{
			//aloca um bloco de memoria
			allocMemory(1);
		}
		
		int len = strlen(str);
		
		m_strArray[m_usedLen] = new char [len + 1];
		memset(m_strArray[m_usedLen], '\0', len + 1);
		memcpy(m_strArray[m_usedLen], str, len);
		//strcpy(m_strArray[m_usedLen], str);
		m_usedLen++;
		
	}
}

void CBasicStringList::add(const char* strList, const char* separator)
{
	int start = 0;

	if (separator == NULL)
	{
		this->add(strList);
		return;
	}

	if (strList == NULL)
	{
		return;
	}


	int lenList = strlen (strList);
	int lensep = strlen (separator);

	//aloca valor maximo que pode ter
	char* aux = new char[lenList + 1];
	memset (aux, 0, lenList + 1);

	int i = 0;
	int j = 0;

	for (i = 0; i < lenList; i++)
	{
		for (j = 0; j < lensep; j++)
		{
			if (strList[i] == separator[j] ||
				strList[i] == 0)
			{
				//achamos um separador
				memcpy(aux, &strList[start], i - start);
				this->add(aux);
				memset (aux, 0, lenList + 1);
				start = i + 1;
				break;
			}
		}
	}

	if (i > start)
	{
		//fim da string
		memcpy(aux, &strList[start], i - start);
		this->add(aux);
	}

	delete[] aux;


}

void CBasicStringList::add(const CBasicStringList& bsl)
{
	int memoryBlocksToAlloc = (bsl.m_usedLen / DEFAULT_BLOCK_LEN)  + 1;
	
	if (memoryBlocksToAlloc > 0)
	{
		allocMemory(memoryBlocksToAlloc);
	}
	
	
	for (unsigned int i = 0; i < bsl.m_usedLen; i++)
	{
		add(bsl.m_strArray[i]);
	}
}

void CBasicStringList::add(const char c)
{
	char newStr[2] = {0};
	newStr[0] = c;
	this->add(newStr);
}

void CBasicStringList::addFormatted (const char* format, ...)
{
	//verifica se precisa alocar mais memoria
	if (m_usedLen + 1 > m_realLen)
	{
		//aloca um bloco de memoria
		allocMemory(1);
	}

	va_list args;
	va_start( args, format );
	
	int len = vsnprintf(NULL, 0, format, args);
	m_strArray[m_usedLen] = new char [len + 1];
	memset(m_strArray[m_usedLen], '\0', len + 1);
	vsnprintf(m_strArray[m_usedLen], len, format, args);
	m_usedLen++;

	va_end(args);
}


void CBasicStringList::update(int index, const char* newValue)
{
	if (index >= 0 &&
		index < (int)m_usedLen &&
		newValue != NULL)
	{
		//botem o amnho da nova string
		int newValueLen = strlen(newValue);

		//apaga o conteudo do indice atual
		delete[] m_strArray[index];

		//aloca memoria para conter o novo valor
		m_strArray[index] = new char[newValueLen + 1];
		memset (m_strArray[index], '\0', newValueLen + 1);

		//copia o novo valor
		memcpy (m_strArray[index], newValue, newValueLen);
		//strcpy(m_strArray[index], newValue);
	}
}

void CBasicStringList::remove(int index)
{
	remove(index, 1);
}

void CBasicStringList::remove(int index, int count)
{
	for (int i = index; i < index + count; i++)
	{
		delete[] m_strArray[i];
	}
	
	m_usedLen -= count;
	
	
	for (unsigned int j = index; j < m_usedLen; j++)
	{
		m_strArray[j] = m_strArray[j + count];
	}
	
	for (unsigned int k = m_usedLen; k < m_usedLen + count; k++)
	{
		m_strArray[k] = NULL;
	}
	
	if ((m_realLen - m_usedLen >= DEFAULT_BLOCK_LEN))
	{
		freeMemory(((m_realLen - m_usedLen) / DEFAULT_BLOCK_LEN));
	}
}

int CBasicStringList::count()
{
	return m_usedLen;
}

void CBasicStringList::clear()
{
	remove(0, m_usedLen);
	
	delete[] m_strArray;
}

int CBasicStringList::indexOf(const char* str, bool caseSensitive)
{
	int ret = -1;
	
	if (caseSensitive)
	{
		for (unsigned int i = 0; i < m_usedLen; i++)
		{
			if (!strcmp(m_strArray[i], str))
			{
				ret = i;
				break;
			}
		}
	}
	else
	{
		for (unsigned int i = 0; i < m_usedLen; i++)
		{
			#ifdef _WIN32
			if (!stricmp(m_strArray[i], str))
			{
			#else
			if (!strcasecmp(m_strArray[i], str))
			{
			#endif
				ret = i;
				break;
			}
		}
	}
	
	return ret;
}

void CBasicStringList::sort(bool ascending, bool caseSensitive)
{
	m_acending = ascending;
	m_caseSensitive = caseSensitive;
	
	if (m_strArray)	quickSortMain(m_strArray, m_usedLen);
}

void CBasicStringList::deserialize(const char* strList, const char* separators)
{
	clear();
	add(strList, separators);
}

char* CBasicStringList::serialize(const char separator)
{
	//vamos calcular o tamanho total dos elementos
	int len;
	int count;
	char sSeparator[2];
	
	sSeparator[0] = separator;
	sSeparator[1] = '\0';
	
	len = 0;
	count = this->count();
	
	for (int i = 0; i < count; i++)
	{
		//+ 1 do separator
		len += strlen(m_strArray[i]) + 1;
	}
	
	//vamos alocar a string
	char* ret = new char[len + 1];
	
	//limpa
	memset(ret, '\0', len + 1);
	
	for (int j = 0; j < count; j++)
	{
		//+ 1 do separator
		strcat(ret, m_strArray[j]);
		strcat(ret, sSeparator);
	}
	
	return ret;
}

void CBasicStringList::deserializeBin(const char* strList, 
			   const unsigned char* separator, 
			   int separatorLen, 
			   const unsigned char* terminator, 
			   int terminatorLen)
{
	clear();

	const char* p = strList;
	const char* pStart = p;

	for (; ; p++)
	{
		if (memcmp(terminator, p, terminatorLen) == 0)
		{
			//terminar

			//adiciona na saida
			char* pBuffer = createBuffer (pStart, p);
			add(pBuffer);
			delete[] pBuffer;

			break;
		}
		else
		{
			if (memcmp (separator, p, separatorLen) == 0)
			{
				//chegou no separador

				//adiciona na saida
				char* pBuffer = createBuffer (pStart, p);
				add(pBuffer);
				delete[] pBuffer;

				p += separatorLen;
				pStart = p;
			}
		}
	}
}

char* CBasicStringList::serializeBin(const unsigned char* separator, 
								   int separatorLen, 
								   const unsigned char* terminator, 
								   int terminatorLen)
{
	
	char* ret = NULL;
	int elmtsCount = count();
	int bytesLen = ((elmtsCount - 1) * separatorLen) + terminatorLen;


	for (int i = 0; i < elmtsCount; i++)
	{
		bytesLen += strlen (elmtAt(i));
	}
	
	ret = new char[bytesLen + 1];
	memset (ret, '\0', bytesLen + 1);
	

	const char* pInput;
	int pInputLen;
	int idx = 0;
	for (int j = 0; j < elmtsCount; j++)
	{
		pInput = elmtAt(j);
		pInputLen = strlen (pInput);
		memcpy (&ret[idx], pInput, pInputLen);
		idx += pInputLen;

		//verifica se egh o ultimo elemento
		if (j + 1 >= elmtsCount)
		{
			//ultimo elemento, adiciona terminador
			memcpy (&ret[idx], terminator, terminatorLen);
			idx += terminatorLen;
		}
		else
		{
			//nao eh ultimo elemento, adiciona separador
			memcpy (&ret[idx], separator, separatorLen);
			idx += separatorLen;
		}
	}
	return ret;
}

const char* CBasicStringList::operator[] (int idx)
{
	return elmtAt(idx);
}

void CBasicStringList::allocMemory(int blocks)
{
	unsigned int newLen = m_realLen + (blocks * DEFAULT_BLOCK_LEN);
	char** newArray = new char*[newLen];
	
	for (unsigned int i = 0; i < newLen; i++)
	{
		if (i < m_realLen)
		{
			newArray[i] = m_strArray[i];
		}
		else
		{
			//espa�o novo vai com NULL
			newArray[i] = NULL;
		}
	}
	
	//deleta o array de ponteiros, pq criamos um outro
	if (m_strArray)
	{
		delete[] m_strArray;
	}
	
	m_realLen = newLen;
	m_strArray = newArray;
}

void CBasicStringList::freeMemory(int blocks)
{
	unsigned int newLen = m_realLen - (blocks * DEFAULT_BLOCK_LEN);
	char** newArray = NULL;
	
	if (newLen > 0)
	{
		newArray = new char*[newLen];
	}
	
	for (unsigned int i = 0; i < m_realLen; i++)
	{
		if (i < newLen)
		{
			newArray[i] = m_strArray[i];
		}
		else
		{
			if (m_strArray[i])
			{
				delete[] m_strArray[i];
				m_strArray[i] = NULL;
			}
		}
	}
	
	//deleta o array de ponteiros, pq criamos um outro
	delete[] m_strArray;
	
	m_realLen = newLen;
	m_strArray = newArray;
}

const char* CBasicStringList::elmtAt(int idx)
{
	return m_strArray[idx];
}

void CBasicStringList::quickSortMain(char** items, int count)
{
	if (count > 0)
		quickSort(items, 0, count-1);
}

void CBasicStringList::quickSort(char** items, int left, int right)
{
	int i, j;
	char *x;
	char* temp;
	
	i = left;
	j = right;
	x = items[(left+right)/2];
	
	register int factor = (m_acending) ? 1 : (-1);
	do
	{
		if (m_caseSensitive)
		{
			while( (factor * strcmp(items[i], x) < 0) && (i < right))
			{i++;}
			while( (factor * strcmp(items[j], x) > 0) && (j > left))
			{j--;}
		}
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

char* CBasicStringList::createBuffer (const char* pStart, const char* pEnd)
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

#ifdef _DEBUG

void CBasicStringList::show ()
{
	int c = count();
	for (int i = 0; i < c; i++)
	{
		printf ("[%s]\n", elmtAt(i));
	}
};
#endif

void CBasicStringList::bslFilter (CBasicStringList& input, const char* searchData, CBasicStringList& output)
{
	int count = input.count();
	
	output.clear();
	
	for (int i = 0; i < count; i++)
	{
		if (strstr(input[i], searchData))
		{
			output.add(input[i]);
		}
	}
}
