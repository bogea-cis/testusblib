///////////////////////////////////////////////////////////////////////////////
// Descricao:
// Autor: Luis Gustavo de Brito
// Data de criacao: 22/05/2014
//
// Alteracoes:
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CIndexedStringTable_H_
#define _CIndexedStringTable_H_

#include <stdarg.h>
#include "CMutex.h"
#include "CBuffer.h"

class CIndexedStringTable
{
	public:
		//construtor da classe
		//parametros
		CIndexedStringTable();
		
		//destrutor da classe
		virtual ~CIndexedStringTable();

		void invertKeyValue (CIndexedStringTable& output);

		//popula a lista com string separada por tokens
		//parametros
		//strList: string com separadores
		//separators: separadores da string
		//static CIndexedStringTable* create(const char* strList, const char* separators);

		void resetIterator();
		bool nextIterator ();
		bool getCurrentKey (char* key);
		bool getCurrentValue (char* value);
		bool getCurrentKeyValue (char* key, char* value);
		bool removeKey (const char* key);
		void clear ();

		int count ();
		void setValue (const char* keyAndValue);
		void setValue (const char* key, const char* value);
		const char* getValue (const char* key);

		void serialize (CBuffer& out, char separator = '\x1C');
		void deserialize (CBuffer& in, char separator = '\x1C');

		bool lock ();
		void unlock ();

		const char* operator[] (const char* key);


	private:
		
		void allocMemory(int numberOfElmtsToInsert);
		void freeMemory(int blocks);
		void quickSortMain(char** items, int count);
		void quickSort(char** items, int left, int right);
		char* createBuffer (const char* pStart, const char* pEnd);
		int indexOf(const char* str);
		int search (int startIdx, int count, const char* key);
		void insert(int index, const char* key, const char* value);
		int getInsertionIndex (int startIdx, int count, const char* key);

	private:
		
		char** m_strKey;
		char** m_strValue;
		unsigned int m_usedLen;
		unsigned int m_realLen;
		int m_index;
		CMutex m_mutex;
};

#endif //_CIndexedStringTable_H_
