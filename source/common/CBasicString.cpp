// CBasicString.cpp: implementation of the CBasicString class.
// Luis Gustavo de Brito
//////////////////////////////////////////////////////////////////////

#include "CBasicString.h"
#include <string.h>
#include <stdio.h>
#include <ctype.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CBasicString::CBasicString ()
{
	m_length = 0;
	m_allocatedLen = 0;
	m_memoryBlockLen = DEFAULT_MEMORY_BLOCK_SIZE;
	m_str = NULL;
	adjustInternalAllocation(DEFAULT_STRING_LEN);
	clear();
}

CBasicString::CBasicString (const char* str)
{
	m_length = 0;
	m_allocatedLen = 0;
	m_memoryBlockLen = DEFAULT_MEMORY_BLOCK_SIZE;
	m_str = NULL;

	adjustInternalAllocation(DEFAULT_STRING_LEN);
	if (str)
	{
		setValue(str);
	}
	else
	{
		clear ();
	}
}

CBasicString::CBasicString (const CBasicString& otherString)
{
	m_str = NULL;
	m_length = 0;
	m_allocatedLen = 0;
	m_memoryBlockLen = otherString.m_memoryBlockLen;

	adjustInternalAllocation (otherString.m_length);
	m_length = otherString.m_length;
	memcpy (m_str, otherString.m_str, m_length);
}

CBasicString::~CBasicString()
{
	if (m_str)
	{
		delete[] m_str;
		m_str = NULL;
	}
}

void CBasicString::setDefaultMemoryBlockLength (int len)
{
	m_memoryBlockLen = len;
}

int CBasicString::getDefaultMemoryBlockLength ()
{
	return m_memoryBlockLen;
}


void CBasicString::clearAndFreeMemory ()
{
	clear ();
	freeInternalAllocation();
	adjustInternalAllocation(DEFAULT_STRING_LEN);
}

bool CBasicString::equals (const CBasicString& otherString, bool caseSensitive)
{
	return equals(otherString.m_str, caseSensitive);
}

bool CBasicString::equals (const char* otherString, bool caseSensitive)
{
	if (caseSensitive)
	{
		return !strcmp(m_str, otherString);
	}
	else
	{
#ifdef WINDOWS
		return !stricmp(m_str, otherString);
#else
		return !strcasecmp(m_str, otherString);
#endif
	}
}

void CBasicString::append (const CBasicString& otherString)
{
	append (otherString.m_str, otherString.m_length);
}

void CBasicString::append (const char* otherString)
{
	append (otherString, strlen(otherString));
}

void CBasicString::append (const char* otherString, unsigned int len)
{
	//verifica se precisa alocar mais memoria
	adjustInternalAllocation(len);


	memcpy (&m_str[m_length], otherString, len);
	m_length += len;
	m_str[m_length] = '\0';
}

void CBasicString::append (int count, const char character)
{
	char* str = new char[count + 1];
	memset (str, character, count);
	str[count] = '\0';
	append (str);
	delete[] str;
}

void CBasicString::append (const char character)
{
	char aux[2] = {0};
	aux[0] = character;
	append (aux, 1);
}

int CBasicString::compare (const CBasicString& otherString, bool caseSensitive)
{
	return compare (otherString.m_str, caseSensitive);
}

int CBasicString::compare (const char* otherString, bool caseSensitive)
{
	if (caseSensitive)
		return strcmp (m_str, otherString);
	else
#ifdef WINDOWS
		return !stricmp(m_str, otherString);
#else
		return !strcasecmp(m_str, otherString);
#endif
}

int CBasicString::nCompare (const CBasicString& otherString, int len, bool caseSensitive)
{
	return nCompare (otherString.m_str, 0, len, caseSensitive);
}

int CBasicString::nCompare (const char* otherString, int len, bool caseSensitive)
{
	return nCompare (otherString, 0, len, caseSensitive);
}

int CBasicString::nCompare (const char* otherString, int startIndex, int len, bool caseSensitive)
{
	if (caseSensitive)
		return strncmp(&m_str[startIndex], otherString, len);
	else
#ifdef WINDOWS
		return strnicmp(&m_str[startIndex], otherString, len);
#else
		return strncasecmp(&m_str[startIndex], otherString, len);
#endif
}

bool CBasicString::startWith (const CBasicString& otherString, bool caseSensitive)
{
	return startWith(otherString.m_str, caseSensitive);
}

bool CBasicString::startWith (const char* otherString, bool caseSensitive)
{
	return !nCompare (otherString, 0, strlen (otherString), caseSensitive);
}

bool CBasicString::endWith (const CBasicString& otherString, bool caseSensitive)
{
	return endWith (otherString.m_str, caseSensitive);
}

bool CBasicString::endWith (const char* otherString, bool caseSensitive)
{
	bool ret = false;
	int otherLen = strlen (otherString);

	if (otherLen <= m_length)
	{
		ret = !nCompare (otherString, m_length - otherLen, otherLen, caseSensitive);
	}

	return ret;
}

void CBasicString::trim (const char character)
{
	lTrim(character);
	rTrim(character);
}

void CBasicString::lTrim (const char character)
{
	char* input = new char[m_length + 1];
	memcpy (input, m_str, m_length);
	input[m_length] = '\0';
	int i = 0;

	//pega inicio
	for (i = 0 ; (input[i] == character) && (i < m_length); i++)
	{}
	
	if (i == m_length)
	{
		//string apenas com espa�o
		clear ();
	}
	else if (i > 0)
	{
		CBasicString str;
		subString (i, m_length - i, str);
		setValue(str);
	}
	delete[] input;
}

void CBasicString::rTrim (const char character)
{
	char* input = new char[m_length + 1];
	memcpy (input, m_str, m_length);
	input[m_length] = '\0';
	int i = 0;

	for (i = m_length - 1 ; (input[i] == character) && (i >= 0); i--)
	{}
	
	i++;

	//frase apenas com espa�o
	if (i == 0)
	{
		clear();
	}
	else if (i < m_length)
	{
		CBasicString str;
		int len = m_length - (m_length - i);
		subString (0, len, str);
		setValue(str);
	}
	delete[] input;
}

int CBasicString::firstIndexOf (const char character)
{
	char str[2] = {0};
	str[0] = character;
	return firstIndexOfAny(str);
}

int CBasicString::firstIndexOfAny (const CBasicString& characteres)
{
	return firstIndexOfAny(characteres.m_str);
}

int CBasicString::firstIndexOfAny (const char* characteres)
{
	int ret = -1;
	int characteresLen = strlen (characteres);

	for (int i = 0; i < m_length; i++)
	{
		for (int j = 0; j < characteresLen; j++)
		{
			if (m_str[i] == characteres[j])
			{
				ret = i;
				return ret;
			}
		}
	}

	return ret;
}

int CBasicString::lastIndexOf (const char character)
{
	char str[2] = {0};
	str[0] = character;
	return lastIndexOfAny(str);
}

int CBasicString::lastIndexOfAny (const CBasicString& characteres)
{
	return lastIndexOfAny (characteres.m_str);
}

int CBasicString::lastIndexOfAny (const char* characteres)
{
	int ret = -1;
	int characteresLen = strlen (characteres);

	for (int i = m_length - 1; i >= 0; i--)
	{
		for (int j = 0; j < characteresLen; j++)
		{
			if (m_str[i] == characteres[j])
			{
				ret = i;
				return ret;
			}
		}
	}

	return ret;
}

void CBasicString::subString (int startIndex, CBasicString& outStr)
{
	subString(startIndex, m_length - startIndex, outStr);
}

void CBasicString::subString (int startIndex, int len, CBasicString& outStr)
{
	if (startIndex + len <= m_length)
	{
		outStr.setValue(&m_str[startIndex], len);
	}
}

void CBasicString::padLeft (int totalLen, const char character)
{
	if (totalLen > m_length)
	{
		int numberOfCharacteres = totalLen - m_length;
		char* aux = new char [totalLen + 1];
		memset (aux, '\0', totalLen + 1);
		memset (aux, character, numberOfCharacteres);
		memcpy(&aux[numberOfCharacteres], m_str, m_length);
		setValue(aux);
		delete[] aux;
	}
}

void CBasicString::padRight (int totalLen, const char character)
{
	if (totalLen > m_length)
	{
		int numberOfCharacteres = totalLen - m_length;
		char* aux = new char [totalLen + 1];
		memset (aux, '\0', totalLen + 1);
		memcpy(aux, m_str, m_length);
		memset (&aux[m_length], character, numberOfCharacteres);
		setValue(aux);
		delete[] aux;
	}
}

void CBasicString::toUpper (char* str)
{
	char* pStr = str;
	
	while (*pStr)
	{
		(*pStr) = toupper(*pStr);
		pStr++;
	}
}

void CBasicString::toLower (char* str)
{
	char* pStr = str;
	
	while (*pStr)
	{
		(*pStr) = tolower(*pStr);
		pStr++;
	}
}

void CBasicString::toUpper ()
{
	CBasicString::toUpper(m_str);
}

void CBasicString::toLower ()
{
	CBasicString::toLower(m_str);
}

int CBasicString::countChar (const char character, bool caseSensitive)
{
	char str[2] = {0};
	str[0] = character;

	return countStr(str, caseSensitive);
}

int CBasicString::countStr(const CBasicString& str, bool caseSensitive)
{
	return countStr(str.m_str, caseSensitive);
}


int CBasicString::countStr(const char* str, bool caseSensitive)
{
    char* p = NULL;
    int counter = 0;

    if (str != NULL && 
		strlen (str) > 0 &&
		m_str != NULL)
    {
		char* tempStr = NULL;
		char* wantedStr = NULL;
		
		if (caseSensitive)
		{
			tempStr = m_str;
			wantedStr = (char*)str;
		}
		else
		{
			tempStr = new char [this->length() + 1];
			strcpy (tempStr, m_str);
			
			CBasicString::toLower(tempStr);

			wantedStr = new char [strlen (str) + 1];
			strcpy (wantedStr, str);
			CBasicString::toLower(wantedStr);
		}

        const int strLen = strlen(wantedStr);
        if (strLen > 0)
        {
			
			p = strstr(tempStr, wantedStr);
			while (p != NULL)
			{
				counter++;
				p += strLen;
				if (p != NULL)
				{
					p = strstr(p, wantedStr);
				}
			}
		}
		if (!caseSensitive)
		{
			delete[] tempStr;
			delete[] wantedStr;
		}
    }

    return counter;
}

void CBasicString::split (const char character, CBasicStringList& outList)
{
	char str[2] = {0};
	str[0] = character;
	split (str, outList);
}

void CBasicString::split (const char* characteres, CBasicStringList& outList)
{
	int characteresLen = strlen (characteres);
	CBasicString sub;
	int start = 0;
	int i = 0;

	outList.clear();

	for (i = 0; i < m_length; i++)
	{
		for (int j = 0; j < characteresLen; j++)
		{
			if (m_str[i] == characteres[j])
			{
				this->subString(start, i - start, sub);
				outList.add(sub.m_str);
				start = i + 1;
			}
		}
	}

	this->subString(start, i - start, sub);
	outList.add(sub.m_str);
}


void CBasicString::replace (const char oldCharacter, const char newCharacter)
{
	char oldChar[2] = {0};
	char newChar[2] = {0};
	oldChar[0] = oldCharacter;
	newChar[0] = newCharacter;
	replace (oldChar, newChar);	
}

void CBasicString::replace (const CBasicString& oldString, const CBasicString& newString)
{
	replace (oldString.m_str, newString.m_str);
}


//@author - Alexandre Bolzon
void CBasicString::replace(const char* oldString, const char* newString)
{
    // conta quantas strings velhas tem no buffer
    int counter = this->countStr(oldString);

    // nao tem nenhuma string velha
    if (counter == 0)
    {
        return; // pula fora
    }

	//se o ponteiro for o mesmo invalida
	if (oldString == m_str ||
		newString == m_str)
	{
        return; // pula fora
	}

    // tamanho das strings velha/nova
    const int oldStringLen = strlen(oldString);
    const int newStringLen = strlen(newString);

    // verifica tamanho da string velha
    if (oldStringLen == 0)
    {
        return; // pula fora
    }

    // obtem o tamanho do buffer velho
    int bufferLen = this->length();

    // enderecos das partes da string
    char** addrs = new char*[counter + 1];
    memset(addrs, 0, counter + 1);

    // obtem referencias dos enderecos
    int i = 0;
    char* p = strstr(m_str, oldString);

    addrs[i++] = m_str;
    while (p != NULL)
    {
        // limita string
        *p = 0;

        // incrementa ponteiro e
        // guarda nova referencia
        p += oldStringLen;
        addrs[i++] = p;

        // encontra proxima string
        if (p != NULL)
        {
            p = strstr(p, oldString);
        }
    }

    // calcula diferenca entre buffers novo/velho
    int diff = (newStringLen - oldStringLen) * counter;

    // calcula tamanho do novo buffer
    int size = (bufferLen + diff) + 1; // buffer + '\0'

    // Cria novo buffer
    char* newBuffer = new char[size];
    if (newBuffer == NULL)
    {
        return; // pula fora
    }

    // Limpa novo buffer
    memset(newBuffer, 0, size);

    int pos = 0;
    int length = 0;
    for (i=0; i<counter+1; i++)
    {
        length = strlen(addrs[i]);

        if (length > 0)
        {
            // copia parte da string antiga
            memcpy(&newBuffer[pos], addrs[i], length);
            pos += length;
        }

        if (i < counter)
        {
            // copia nova string
            memcpy(&newBuffer[pos], newString, newStringLen);
            pos += newStringLen;
        }
    }

    // seta novo buffer
    this->setValue(newBuffer, size - 1);

    // apaga novo buffer
    delete[] newBuffer;
	newBuffer = NULL;

    // apaga vetor de enderecos
    delete[] addrs;
    addrs = NULL;
}

void CBasicString::centralize (int len, char padCharacter)
{
	if (len > m_length && 
		len > 0)
	{		
		char* aux = new char[len + 1];
		memset (aux, padCharacter, len);
		aux[len] = '\0';

		memcpy (&aux[(len - m_length) / 2], m_str, m_length);

		setValue(aux);
		delete[] aux;
	}
}

void CBasicString::setValue (const CBasicString& otherString)
{
	setValue(otherString.m_str, otherString.m_length);
}

void CBasicString::setValue (const char* otherString)
{
	setValue(otherString, strlen (otherString));
}

void CBasicString::setValue (int count, const char character)
{
	char* str = new char[count + 1];
	memset (str, character, count);
	str[count] = '\0';
	setValue(str);
	delete[] str;
}

void CBasicString::setValue (const char* otherString, int len)
{
	//clear();
	m_length = 0;

	adjustInternalAllocation(len);
	memcpy (m_str, otherString, len);
	m_str[len] = '\0';
	m_length = len;
}

int CBasicString::length()
{
   return m_length;
}

char CBasicString::at (int index)
{
	if (index >= 0 && index < m_length)
	{
		return m_str[index];
	}
	else
	{
		return -1;
	}
}

void CBasicString::show()
{
	printf ("[%s, %d, %d]\n", m_str, m_length, m_allocatedLen);
}

void CBasicString::clear ()
{
	m_str[0] = '\0';
	m_length = 0;
}

const char* CBasicString::cStr ()
{
	return m_str;
}


CBasicString& CBasicString::operator=(const CBasicString& otherString)
{
	if (*this != otherString)
	{
		setValue(otherString.m_str, otherString.m_length);
	}
	return *this;
}

CBasicString& CBasicString::operator=(const char* str)
{
	setValue(str);
	return *this;
}

bool CBasicString::operator==(const CBasicString& otherString)
{
	return equals (otherString.m_str);
}

bool CBasicString::operator==(const char* otherString)
{
	return equals (otherString);
}

bool CBasicString::operator!=(const CBasicString& otherString)
{
	return !equals(otherString.m_str);
}

bool CBasicString::operator!=(const char* otherString)
{
	return !equals(otherString);
}

CBasicString& CBasicString::operator+(const CBasicString& otherString)
{
	append (otherString);
	return *this;
}

CBasicString& CBasicString::operator+(const char* str)
{
	append (str);
	return *this;
}

CBasicString& CBasicString::operator+=(const CBasicString& otherString)
{
	append (otherString);
	return *this;
}

CBasicString& CBasicString::operator+=(const char* str)
{
	append (str);
	return *this;
}

char CBasicString::operator[] (int index)
{
	//nao chama o metodo at para ser mais rapido
	return m_str[index];
}

void CBasicString::adjustInternalAllocation (int aditionalSize)
{
	//verifica se o tamanho atual da string + o que vai ser 
	//adicionado eh maior que o alocado
	if (m_length + aditionalSize >= m_allocatedLen)
	{
		//precisa alocar mais
		
		//aloca aux
		char* aux = NULL;
		if (m_length > 0)
		{
			aux = new char[m_length + 1];

			//copia conteudo para aux
			memcpy (aux, m_str, m_length);
			aux[m_length] = '\0';
		}

		//limpa buffer atual
		freeInternalAllocation ();

		//aloca mais um bloco de memoria
		m_allocatedLen = (m_memoryBlockLen * ((m_length + aditionalSize) / m_memoryBlockLen + 1));
		m_str = new char [m_allocatedLen + 1];
		memset (m_str, '\0', m_allocatedLen + 1);

		//copia de aux
		if (aux)
		{
			memcpy (m_str, aux, m_length);

			//destroi aux
			delete[] aux;
			aux = NULL;
		}
	}
}

void CBasicString::freeInternalAllocation ()
{
	if (m_str)
	{
		delete[] m_str;
		m_str = NULL;
		m_allocatedLen = 0;
	}

}

