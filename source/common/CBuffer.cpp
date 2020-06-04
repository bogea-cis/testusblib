// CBuffer.cpp: implementation of the CBuffer class.
//
//////////////////////////////////////////////////////////////////////

#include "CBuffer.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBuffer::CBuffer()
{
	resetIterator();
	m_memoryBlockLen = DEFAULT_BUFFER_MEMORY_BLOCK_SIZE;
	m_buffer = NULL;
	m_unpackedAsciiBuffer = NULL;
	m_bufferLen = 0;
	m_allocatedLen = 0;
	adjustInternalAllocation(1);
}

CBuffer::CBuffer(unsigned int memoryBlockLen)
{
	resetIterator();
	m_memoryBlockLen = memoryBlockLen;
	m_buffer = NULL;
	m_unpackedAsciiBuffer = NULL;
	m_bufferLen = 0;
	m_allocatedLen = 0;
	adjustInternalAllocation(1);
}

CBuffer::CBuffer(const CBuffer& buffer)
{
	resetIterator();
	m_memoryBlockLen = buffer.m_memoryBlockLen;
	m_buffer = NULL;
	m_unpackedAsciiBuffer = NULL;
	m_bufferLen = 0;
	m_allocatedLen = 0;
	adjustInternalAllocation(1);

	setBuffer (buffer.m_buffer, buffer.m_bufferLen);
}

CBuffer::CBuffer(const char* buffer)
{
	resetIterator();
	m_memoryBlockLen = DEFAULT_BUFFER_MEMORY_BLOCK_SIZE;
	m_buffer = NULL;
	m_unpackedAsciiBuffer = NULL;
	m_bufferLen = 0;
	m_allocatedLen = 0;
	adjustInternalAllocation(1);

	setBuffer (buffer);
}

CBuffer::CBuffer(const unsigned char* buffer, int len)
{
	resetIterator();
	m_memoryBlockLen = DEFAULT_BUFFER_MEMORY_BLOCK_SIZE;
	m_buffer = NULL;
	m_unpackedAsciiBuffer = NULL;
	m_bufferLen = 0;
	m_allocatedLen = 0;
	adjustInternalAllocation(1);

	setBuffer (buffer, len);
}

CBuffer::~CBuffer()
{
	if (m_buffer)
	{
		delete[] m_buffer;
		m_buffer = NULL;
	}

	if (m_unpackedAsciiBuffer)
	{
		delete[] m_unpackedAsciiBuffer;
		m_unpackedAsciiBuffer = NULL;
	}

}

unsigned char CBuffer::at (int index)
{
	return m_buffer[index];
}

unsigned char CBuffer::operator[] (int index)
{
	return at(index);
}

bool CBuffer::operator==(const CBuffer& otherBuffer)
{
	return compare(otherBuffer) == 0;
}

bool CBuffer::operator!=(const CBuffer& otherBuffer)
{
	return compare(otherBuffer) != 0;
}

CBuffer& CBuffer::operator=(const CBuffer& otherBuffer)
{
	if (*this != otherBuffer)
	{
		setBuffer(otherBuffer.m_buffer, otherBuffer.m_bufferLen);
	}
	return *this;
}

CBuffer& CBuffer::operator=(const char* str)
{
	setBuffer(str);
	return *this;
}



int CBuffer::indexOf (int startIndex, const unsigned char* buffer, int bufferLen)
{
	int ret = -1;

	//verifica se nao ultrapassa os limites
	if (startIndex + bufferLen <= m_bufferLen)
	{
		for (int i = startIndex; i <= m_bufferLen - bufferLen; i++)
		{
			if (memcmp(&m_buffer[i], buffer, bufferLen) == 0)
			{
				ret = i;
				break;
			}
		}
	}

	return ret;
}

int CBuffer::replace (const unsigned char* old, int len, const unsigned char* newData, int newLen)
{
	int ret = 0;
	
	if (old == NULL || 
		len <= 0)
	{
		return 0;
	}
	
	if (newLen == 0 || newData == NULL)
	{
		return this->removeFromBuffer(old, len);
	}
	
	bool exit = false;
	int idx = 0;

	while (!exit)
	{
		idx = indexOf (0, old, len);

		if (idx >= 0)
		{
			memcpy (&m_buffer[idx], &m_buffer[idx + len], m_bufferLen - (len + idx));
			m_buffer[m_bufferLen - len] = 0;
			m_bufferLen  -= len;
			
			this->insert(idx, newData, newLen);
			ret ++;
		}
		else
		{
			exit = true;
		}
	}

	return ret;	
	
}

int CBuffer::replace (const char* old, const char* newData)
{
	return this->replace((const unsigned char*) old, 
							strlen (old), 
							(const unsigned char*) newData, 
							(newData) ? strlen(newData) : 0);
}

void CBuffer::updateBuffer (int startIndex, const unsigned char* buffer, int len)
{
	if (startIndex + len > m_bufferLen)
	{
		adjustInternalAllocation((startIndex + len) - m_bufferLen);
		m_bufferLen = startIndex + len;
	}
	memcpy (&m_buffer[startIndex], buffer, len);
}

void CBuffer::setBuffer (const char* buffer)
{
	setBuffer((const unsigned char*)buffer, strlen(buffer));
}

void CBuffer::setBuffer (const unsigned char* buffer, int len)
{
	clear();

	if (len > m_bufferLen)
	{
		adjustInternalAllocation(len);
	}

	memcpy (m_buffer, buffer, len);
	m_bufferLen = len;
	m_buffer[m_bufferLen] = '\0';
}

void CBuffer::setBuffer (const unsigned char fillByte, int len)
{
	clear();
	adjustInternalAllocation(len);
	memset (m_buffer, fillByte, len);
	m_bufferLen = len;
	m_buffer[m_bufferLen] = '\0';
}

void CBuffer::setBuffer (CBuffer& buffer)
{
	setBuffer(buffer.m_buffer, buffer.m_bufferLen);
}

void CBuffer::setBufferFormatted (const char* format, ...)
{
	va_list args;
	va_start( args, format );
	
	int needed = vsnprintf(NULL, 0, format, args) + 1;
	char* buf = new char[needed + 1];
	memset (buf, 0, needed + 1);
	vsnprintf(buf, needed, format, args);
	
	setBuffer(buf);
	delete[] buf;

	va_end(args);
}

const unsigned char* CBuffer::getBuffer ()
{
	return m_buffer;
}

const char* CBuffer::getAsciiBuffer ()
{
	return (const char*) m_buffer;
}

const char* CBuffer::getAsciiUnpackedBuffer()
{
	if (m_unpackedAsciiBuffer)
	{
		delete[] m_unpackedAsciiBuffer;
		m_unpackedAsciiBuffer = NULL;
	}

	if (m_bufferLen > 0)
	{
		
		CBuffer buf;
		unpack(buf);
		
		const char* pbuf = buf.getAsciiBuffer();
		if (pbuf)
		{
			int len = strlen(pbuf);
			m_unpackedAsciiBuffer = new char[len + 1];
			
			//copia o \0 tb
			memcpy (m_unpackedAsciiBuffer, pbuf, len + 1);
		}
	}
	else
	{
		return "";
	}
	return m_unpackedAsciiBuffer;
}

void CBuffer::insert (int idx, CBuffer& buffer)
{
	insert (idx, buffer.m_buffer, buffer.m_bufferLen);
}

void CBuffer::insert (int idx, const char* buffer)
{
	insert (idx, (unsigned char*)buffer, strlen(buffer));
}

void CBuffer::insert (int idx, const unsigned char* buffer, int len)
{
	if (idx == 0)
	{
		CBuffer b;
		b.append(buffer, len);
		b.append(*this);
		this->setBuffer(b);
	}
	else if (idx > 0)
	{
		CBuffer b;

		b.append(this->m_buffer, idx);
		b.append(buffer, len);
		b.append(&this->m_buffer[idx], this->m_bufferLen - idx);
		
		this->setBuffer(b);
	}
}

void CBuffer::insertFormatted (int idx, const char* format, ...)
{
	va_list args;
	va_start( args, format );
	
	int needed = vsnprintf(NULL, 0, format, args) + 1;
	char* buf = new char[needed + 1];
	memset (buf, 0, needed + 1);
	vsnprintf(buf, needed, format, args);
	
	insert(idx, buf);
	delete[] buf;

	va_end(args);
}

int CBuffer::indexOf(CBuffer& buffer)
{
	return indexOf(0, buffer.m_buffer, buffer.m_bufferLen);
}

int CBuffer::indexOf(const char* str)
{
	return indexOf(0, (const unsigned char*) str, strlen (str));
}

int CBuffer::indexOf(char ch)
{
	char str[2] = {0};
	str[0] = ch;
	return indexOf (str);
}

int CBuffer::removeFromBuffer(CBuffer& buffer)
{
	return removeFromBuffer(buffer.m_buffer, buffer.m_bufferLen);
}

int CBuffer::removeFromBuffer(const char* buffer)
{
	return removeFromBuffer((const unsigned char*)buffer, strlen (buffer));
}

int CBuffer::removeFromBuffer(char ch)
{
	char str[2] = {0};
	str[0] = ch;
	return removeFromBuffer(str);
}


int CBuffer::removeFromBuffer(const unsigned char* buffer, int bufferLen)
{
	bool exit = false;
	int ret = 0;
	int idx = 0;

	while (!exit)
	{
		idx = indexOf (0, buffer, bufferLen);

		if (idx >= 0)
		{
			memcpy (&m_buffer[idx], &m_buffer[idx + bufferLen], m_bufferLen - (bufferLen + idx));
			m_buffer[m_bufferLen - bufferLen] = 0;
			m_bufferLen  -= bufferLen;
			ret ++;
		}
		else
		{
			exit = true;
		}
	}

	return ret;
}

bool CBuffer::removeFromBuffer(int idx, int len)
{
	bool ret = false;
	
	if (idx < 0 ||
		len <= 0 ||
		(idx + len > m_bufferLen))
	{
		return false;
	}
	else if (idx == 0 && len == m_bufferLen)
	{
		this->clear();
		return true;
	}
	
	memcpy (&m_buffer[idx], &m_buffer[idx + len], m_bufferLen - (idx + len));
	m_bufferLen = m_bufferLen - len;
	m_buffer[m_bufferLen] = 0;
	
	return ret;
}


void CBuffer::trim (const unsigned char character)
{
	this->lTrim(character);
	this->rTrim(character);
}

void CBuffer::lTrim (const unsigned char character)
{
	int i;

	if (m_buffer[0] != character)
	{
		return;
	}

	//pega inicio
	for (i = 0 ; (m_buffer[i] == character) && (i < m_bufferLen); i++)
	{}
	
	if (i == m_bufferLen)
	{
		//apenas com espaço
		this->clear ();
	}
	else if (i > 0)
	{
		CBuffer aux(&m_buffer[i], m_bufferLen - i);
		this->setBuffer(aux);
	}
}

void CBuffer::rTrim (const unsigned char character)
{
	int i;

	if (m_buffer[m_bufferLen - 1] != character)
	{
		return;
	}

	for (i = m_bufferLen - 1 ; (m_buffer[i] == character) && (i >= 0); i--)
	{}
	
	if (i < 0)
	{
		//apenas com espaço
		this->clear ();
	}
	else if (i >= 0)
	{
		m_bufferLen = i + 1;
		m_buffer[i + 1] = 0;
	}
}

int CBuffer::length ()
{
	return m_bufferLen;
}

int CBuffer::allocatedLen ()
{
	return m_allocatedLen;
}

int CBuffer::compare (const CBuffer& buffer)
{
	int max = (m_bufferLen > buffer.m_bufferLen) ? m_bufferLen : buffer.m_bufferLen;
	return memcmp(m_buffer, buffer.m_buffer, max);
}

int CBuffer::compare (const CBuffer& buffer, int len)
{
	return memcmp(m_buffer, buffer.m_buffer, len);
}

int CBuffer::compare (const unsigned char* buffer, int len)
{
	return memcmp(m_buffer, buffer, len);
}

void CBuffer::append (int minLen, bool rightAlignment, char padChar, const char* value)
{
	if (value == NULL)
	{
		value = "";
	}

	int vlen = strlen (value);
	//int size = (minLen > vlen) ? minLen : vlen;

	CBuffer b;

	if (rightAlignment)
	{
		if (minLen > vlen)
		{
			b.append(padChar, minLen - vlen);
		}
		b.append(value);
	}
	else
	{
		b.append(value);
		if (minLen > vlen)
		{
			b.append (padChar, minLen - vlen);
		}
	}
	
	this->append(b);
}

void CBuffer::append (const CBuffer& buffer)
{
	append(buffer.m_buffer, buffer.m_bufferLen);
}

void CBuffer::append (const unsigned char* buffer, int len)
{
	//verifica se precisa alocar mais memoria
	adjustInternalAllocation(len);
	
	memcpy (&m_buffer[m_bufferLen], buffer, len);
	m_bufferLen += len;
	m_buffer[m_bufferLen] = '\0';
}

void CBuffer::append (const char* buffer)
{
	append( (const unsigned char*)buffer, strlen(buffer) );
}

void CBuffer::append (unsigned long long value)
{
	append ((unsigned char*)&value, sizeof (unsigned long long));
}

void CBuffer::append (long long value)
{
	append ((unsigned char*)&value, sizeof (long long));
}

void CBuffer::append (unsigned long value)
{
	append ((unsigned char*)&value, sizeof (unsigned long));
}

void CBuffer::append (long value)
{
	append ((unsigned char*)&value, sizeof (unsigned long));
}

void CBuffer::append (unsigned int value)
{
	append ((unsigned char*)&value, sizeof (unsigned int));
}

void CBuffer::append (int value)
{
	append ((unsigned char*)&value, sizeof (int));
}

void CBuffer::append (unsigned short value)
{
	append ((unsigned char*)&value, sizeof (unsigned short));
}

void CBuffer::append (short value)
{
	append ((unsigned char*)&value, sizeof (short));
}

void CBuffer::append (unsigned char value)
{
	append ((unsigned char*)&value, sizeof (unsigned char));
}

void CBuffer::append (char value)
{
	append ((unsigned char*)&value, sizeof (char));
}

void CBuffer::append (bool value)
{
	append ((unsigned char*)&value, sizeof (bool));
}

void CBuffer::append (unsigned char fillByte, int len)
{
	if (len > 0)
	{
		unsigned char* buf = new unsigned char[len];
		memset (buf, fillByte, len);
		append(buf, len);
		delete[] buf;
	}
}

void CBuffer::appendFormatted (const char* format, ...)
{
	va_list args;
	va_start( args, format );
	
	int needed = vsnprintf(NULL, 0, format, args) + 1;
	char* buf = new char[needed + 1];
	memset (buf, 0, needed + 1);
	vsnprintf(buf, needed, format, args);
	
	append(buf);
	delete[] buf;

	va_end(args);
}

void CBuffer::clear ()
{
	m_bufferLen = 0;
	m_buffer[0] = '\0';
}

void CBuffer::setInteratorIndex (int index)
{
	if (index >=0 &&
		index < m_bufferLen)
	{
		m_iteratorIdx = index;
	}
}

int CBuffer::getIteratorIndex ()
{
	return m_iteratorIdx;
}

void CBuffer::getNext (CBuffer& output)
{
	int len = m_bufferLen - m_iteratorIdx;
	subBuffer(m_iteratorIdx, len, output);
	m_iteratorIdx += len;
}

void CBuffer::getNext (int len, CBuffer& output)
{
	subBuffer(m_iteratorIdx, len, output);
	m_iteratorIdx += len;
}

void CBuffer::getNext (int len, unsigned char* output, int allocatedLen)
{
	subBuffer (m_iteratorIdx, len, output, allocatedLen);
	m_iteratorIdx += len;
}

void CBuffer::getNext (unsigned long long& output)
{
	getNext (sizeof(unsigned long long), (unsigned char*)&output, sizeof(unsigned long long));
}

void CBuffer::getNext (long long& output)
{
	getNext (sizeof(long long), (unsigned char*)&output, sizeof(long long));
}

void CBuffer::getNext (unsigned long& output)
{
	getNext (sizeof(unsigned long), (unsigned char*)&output, sizeof(unsigned long));
}

void CBuffer::getNext (long& output)
{
	getNext (sizeof(long), (unsigned char*)&output, sizeof(long));
}

void CBuffer::getNext (unsigned int& output)
{
	getNext (sizeof(unsigned int), (unsigned char*)&output, sizeof(unsigned int));
}

void CBuffer::getNext (int& output)
{
	getNext (sizeof(int), (unsigned char*)&output, sizeof(int));
}

void CBuffer::getNext (unsigned short& output)
{
	getNext (sizeof(unsigned short), (unsigned char*)&output, sizeof(unsigned short));
}

void CBuffer::getNext (short& output)
{
	getNext (sizeof(short), (unsigned char*)&output, sizeof(short));
}

void CBuffer::getNext (unsigned char& output)
{
	getNext (sizeof(unsigned char), (unsigned char*)&output, sizeof(unsigned char));
}

void CBuffer::getNext (char& output)
{
	getNext (sizeof(char), (unsigned char*)&output, sizeof(char));
}

void CBuffer::getNext (bool& output)
{
	getNext (sizeof(bool), (unsigned char*)&output, sizeof(bool));
}

void CBuffer::adjustInternalAllocation (int aditionalSize)
{
	//verifica se o tamanho atual + o que vai ser 
	//adicionado eh maior que o alocado
	if (m_bufferLen + aditionalSize >= m_allocatedLen)
	{
		//precisa alocar mais
		
		//aloca aux
		unsigned char* aux = NULL;
		if (m_bufferLen > 0)
		{
			aux = new unsigned char[m_bufferLen];

			//copia conteudo para aux
			memcpy (aux, m_buffer, m_bufferLen);
		}

		//limpa buffer atual
		freeInternalAllocation ();

		//aloca mais um bloco de memoria
		m_allocatedLen = (m_memoryBlockLen * ((m_bufferLen + aditionalSize) / m_memoryBlockLen + 1));
		m_buffer = new unsigned char [m_allocatedLen + 1];
		memset (m_buffer, '\0', m_allocatedLen + 1);

		//copia de aux
		if (aux)
		{
			memcpy (m_buffer, aux, m_bufferLen);

			//destroi aux
			delete[] aux;
			aux = NULL;
		}
	}
}

void CBuffer::freeInternalAllocation ()
{
	if (m_buffer)
	{
		delete[] m_buffer;
		m_buffer = NULL;
		m_allocatedLen = 0;
	}
}

void CBuffer::setDefaultMemoryBlockLength (int len)
{
	m_memoryBlockLen = len;
	
	CBuffer aux(len);
	setBuffer(aux);
}

int CBuffer::getDefaultMemoryBlockLength ()
{
	return m_memoryBlockLen;
}


void CBuffer::subBuffer (int startIndex, CBuffer& output)
{
	subBuffer (startIndex, m_bufferLen - startIndex, output);
}

void CBuffer::subBuffer (int startIndex, int len, CBuffer& output)
{
	output.clear();

	if (startIndex + len <= m_bufferLen)
	{
		output.setBuffer(&m_buffer[startIndex], len);
	}
}

void CBuffer::subBuffer (int startIndex, int len, unsigned char* output, int allocatedLen)
{
	if (startIndex + len <= m_bufferLen)
	{
		if (output &&
			allocatedLen >= len)
		{
			memcpy (output, &m_buffer[startIndex], len);
		}
	}
}

void CBuffer::subBuffer (int startIndex, unsigned long& output)
{
	subBuffer (startIndex,  sizeof (unsigned long), (unsigned char*)&output, sizeof (unsigned long));
}

void CBuffer::subBuffer (int startIndex, long& output)
{
	subBuffer (startIndex,  sizeof (long), (unsigned char*)&output, sizeof (long));
}

void CBuffer::subBuffer (int startIndex, unsigned int& output)
{
	subBuffer (startIndex,  sizeof (unsigned int), (unsigned char*)&output, sizeof (unsigned int));
}

void CBuffer::subBuffer (int startIndex, int& output)
{
	subBuffer (startIndex,  sizeof (int), (unsigned char*)&output, sizeof (int));
}

void CBuffer::subBuffer (int startIndex, unsigned short& output)
{
	subBuffer (startIndex,  sizeof (unsigned short), (unsigned char*)&output, sizeof (unsigned short));
}

void CBuffer::subBuffer (int startIndex, short& output)
{
	subBuffer (startIndex,  sizeof (short), (unsigned char*)&output, sizeof (short));
}

void CBuffer::subBuffer (int startIndex, unsigned char& output)
{
	subBuffer (startIndex, sizeof (unsigned char), (unsigned char*)&output, sizeof (unsigned char));
}

void CBuffer::subBuffer (int startIndex, char& output)
{
	subBuffer (startIndex, sizeof (char), (unsigned char*)&output, sizeof (char));
}

void CBuffer::subBuffer (int startIndex, bool& output)
{
	subBuffer (startIndex, sizeof (bool), (unsigned char*)&output, sizeof (bool));
}

int CBuffer::bufferLen (const unsigned char* buffer, const unsigned char* terminator, int terminatorLen)
{
	const unsigned char* p = buffer;

	if (p == NULL)
	{
		return -1;
	}

	for (; ; p++)
	{
		if (memcmp(terminator, p, terminatorLen) == 0)
		{
			break;
		}
	}

	return p - buffer;
}

void CBuffer::bcdPack ()
{
	CBuffer packedBuffer;
	this->bcdPack(packedBuffer);
	
	this->setBuffer(packedBuffer);
}

void CBuffer::bcdPack (CBuffer& packedBuffer)
{
	unsigned char packed;
	packedBuffer.clear();

	//Percorre o vetor de dados de entrada, para comprimí-los
	for( int i = 0; i < m_bufferLen; i += 2)
	{
		m_buffer[i] = toupper(m_buffer[i]);
		m_buffer[i+1] = toupper(m_buffer[i+1]);
		
		//Ex: 0011 1010 (A)
		int byteA = m_buffer[ i ];
		//Ex: 0011 1011 (B)
		int byteB= m_buffer[ i + 1 ];

		//Ex: 0000 1010 (A)
		byteA= byteA & 0x0F;

		//Ex: 0000 1011 (B)
		byteB= byteB & 0x0F;

		//Ex: 1010 0000 (A)
		byteA= byteA << 4;

		packed = 0;
		packed |= byteA;
		packed |= byteB;

		packedBuffer.append (packed);
	}

}


void CBuffer::bcdUnpack ()
{
	CBuffer unpackedBuffer;
	this->bcdUnpack(unpackedBuffer);
	
	this->setBuffer(unpackedBuffer);
}

void CBuffer::bcdUnpack (CBuffer& unpackedBuffer)
{
    unsigned char al;
    unsigned char ba;

	unpackedBuffer.clear();
   
    for (int i = 0; i < m_bufferLen; i++ )
    { 	
        al = (m_buffer[i] & 0xf0); 
        al = (al >> 4);
        //if ( al <= 9)
        al =  al + 0x30;
        //else
        //    al = al + 0x37;	
        
        ba = (m_buffer[i] & 0x0f); 
        //if ( ba <= 9)
        ba = ba + 0x30;
        //else
        //    ba = ba + 0x37;
        
		unpackedBuffer.append((unsigned char*)&al, 1);
		unpackedBuffer.append((unsigned char*)&ba, 1);
    }
}

void CBuffer::pack (CBuffer& packedBuffer)
{
    unsigned char packed;

	packedBuffer.clear();

	//Percorre o vetor de dados de entrada, para comprimí-los
    for (int i = 0; i < m_bufferLen; i += 2)
	{
		//Ex: 0011 1010 (A)
		int byteA = toupper(m_buffer[ i ]);
		//Ex: 0011 1011 (B)
		int byteB = toupper(m_buffer[ i + 1 ]);


		if (byteA >= 0x41)
		{
			byteA = 0x0A + (byteA - 0x41);
		}
		else
		{
			//Ex: 0000 1010 (A)
			byteA= byteA & 0x0F;
		}

		if (byteB >= 0x41)
		{
			byteB = 0x0A + (byteB - 0x41);
		}
		else
		{
			//Ex: 0000 1011 (B)
			byteB= byteB & 0x0F;
		}

		//Ex: 1010 0000 (A)
		byteA= byteA << 4;

		packed = 0;
		packed |= byteA;
		packed |= byteB;

		packedBuffer.append (packed);
	}
}

void CBuffer::pack ()
{
	CBuffer packedBuffer;
	this->pack(packedBuffer);
	
	this->setBuffer(packedBuffer);
}

void CBuffer::unpack (CBuffer& unpackedBuffer)
{
    unsigned char al;
    unsigned char ba;

	unpackedBuffer.clear();
   
    for (int i = 0; i < m_bufferLen; i++ )
    { 	
        al = (m_buffer[i] & 0xf0); 
        al = (al >> 4);
        if ( al <= 9)
            al = al + 0x30;
        else
            al = al + 0x37;	
        
        ba = (m_buffer[i] & 0x0f); 
        if ( ba <= 9)
            ba = ba + 0x30;
        else
            ba = ba + 0x37;
        
		unpackedBuffer.append((unsigned char*)&al, 1);
		unpackedBuffer.append((unsigned char*)&ba, 1);
    }
}

void CBuffer::unpack()
{
	CBuffer unpackedBuffer;
	this->unpack(unpackedBuffer);
	
	this->setBuffer(unpackedBuffer);
}

void CBuffer::pack (const char* ascii, unsigned char* hex)
{
	CBuffer b(ascii);
	b.pack();
	memcpy(hex, b.getBuffer(), b.length());
}

void CBuffer::unpack (const unsigned char* hex, int len, char* ascii)
{
	CBuffer b(hex, len);
	memcpy(ascii, b.getAsciiUnpackedBuffer(), b.length() * 2);
}

void CBuffer::resetIterator ()
{
	m_iteratorIdx = 0;
}


void CBuffer::forwardIterator (int n)
{
	m_iteratorIdx += n;
}

void CBuffer::rewindIterator (int n)
{
	m_iteratorIdx -= n;
	if (m_iteratorIdx < 0)
	{
		m_iteratorIdx = 0;
	}
}

const unsigned char* CBuffer::getBufferFromIterator ()
{
	return &m_buffer[m_iteratorIdx];
}

const char* CBuffer::getAsciiBufferFromIterator ()
{
	return (const char*)getBufferFromIterator();
}

int CBuffer::getLengthFromIterator ()
{
	return m_bufferLen - m_iteratorIdx;
}
