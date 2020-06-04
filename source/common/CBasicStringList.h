///////////////////////////////////////////////////////////////////////////////
// Descricao:
// Autor: Amadeu Vilar Filho / Luis Gustavo de Brito
// Data de criacao: 11/12/2007
//
// Alteracoes:
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CBASIC_STRING_LIST_H_
#define _CBASIC_STRING_LIST_H_

#include <stdarg.h>
#include "CMutex.h"

const unsigned int DEFAULT_BLOCK_LEN = 8;

class CBasicStringList
{
	public:
		//construtor da classe
		//parametros
		CBasicStringList();
		
		//destrutor da classe
		virtual ~CBasicStringList();

		//popula a lista com string separada por tokens
		//parametros
		//strList: string com separadores
		//separators: separadores da string
		static CBasicStringList* create(const char* strList, const char* separators);

		bool lock ();
		void unlock ();

		//popula a lista com string separada por tokens binarios (usado nos campos lpszextra do WOSA)
		//parametros
		//strList: string com separadores
		//separator: separador da string, serah usado para encontrar os separadores
		//separatorLen: tamanho da string de separator
		//terminator: terminador da string, serah usado para identificar o fim da string (no WOSA eh um \0\0)
		//terminatorLen: tamanho da string de terminator
		static CBasicStringList* create(const char* strList, 
					   const unsigned char* separator, 
					   int separatorLen, 
					   const unsigned char* terminator, 
					   int terminatorLen);

		//adiciona uma string na lista
		//parametros
		//str: string que sera adicionada
		void add(const char* str);
		
		//adiciona strings separadas por tokens
		//parametros
		//strList: string com separadores
		//separator: separadores da string
		void add(const char* strList, const char* separator);
		
		//adiciona todo o conteudo de uma CBasicStringList no objeto atual
		//parametros
		//bsl: lista de dados
		void add(const CBasicStringList& bsl);

		//adiciona um char ao objeto atual
		//parametros
		//c: char a ser adicionado
		void add(const char c);

		void addFormatted (const char* format, ...);


		//atualiza o valor do conteudo de um determinado index
		//parametros
		//index: indice de onde se deseja fzer a altera��o
		//newValue: novo valor que deve ser assumido
		void update(int index, const char* newValue);

		//remove um elemento da lista
		//parametros
		//index: indice do elemento que se deseja apagar
		void remove(int index);
		
		//remove um numero de elementos a partir de um determinado indice
		//parametros
		//index: indice inicial do(s) elemento(s) que se deseja(m) apagar
		//count: numero de elementos que v�o ser apagados a partir do index inicial
		void remove(int index, int count);
		
		//retorna o numero de elementos
		int count();
		
		//apaga todos os elementos da lista
		void clear();
		
		//retorna o index da primeira ocorrencia de uma determinada string (str)
		//parametros
		//str: string que se deseja procurar
		//caseSesitive: determina se a busca vai ser case sensitive ou n�o(default)
		int indexOf(const char* str, bool caseSensitive = false);
		
		//ordena a lista
		//parametros
		//ascending: determina se vai ordenar em ordem crescente(default) ou n�o
		//caseSesitive: determina se vai ser case sensitive ou n�o(default)
		void sort(bool ascending = true, bool caseSensitive = false);
		
		//popula a lista com string separada por tokens
		//parametros
		//strList: string com separadores
		//separators: separadores da string
		void deserialize(const char* strList, const char* separators);

		//cria uma string onde os elementos s�o separados por um dado separador(separator),
		//esta string deve ser desalocada ap�s o uso com usando-se delete[]
		//parametros
		//separator: separador usado para separar os elementos
		char* serialize(const char separator);
		

		//popula a lista com string separada por tokens binarios (usado nos campos lpszextra do WOSA)
		//parametros
		//strList: string com separadores
		//separator: separador da string, serah usado para encontrar os separadores
		//separatorLen: tamanho da string de separator
		//terminator: terminador da string, serah usado para identificar o fim da string (no WOSA eh um \0\0)
		//terminatorLen: tamanho da string de terminator
		void deserializeBin(const char* strList, 
					   const unsigned char* separator, 
					   int separatorLen, 
					   const unsigned char* terminator, 
					   int terminatorLen);

		//separator: separador da string, serah usado para separar as strigs
		//separatorLen: tamanho da string de separator
		//terminator: terminador da string, serah appendado no final
		//terminatorLen: tamanho da string de terminator
		//retorno: um buffer que deve ser deletado apos o uso
		char* serializeBin(const unsigned char* separator, 
										   int separatorLen, 
										   const unsigned char* terminator, 
										   int terminatorLen);


		static void bslFilter (CBasicStringList& input, const char* searchData, CBasicStringList& output);


		//obtem um elemento de um dado indice
		const char* operator[] (int idx);
		
		//obtem um elemento de um dado indice
		const char* elmtAt(int idx);
		

		#ifdef _DEBUG
		void show ();
		#endif

	private:
		
		void allocMemory(int blocks);
		void freeMemory(int blocks);
		void quickSortMain(char** items, int count);
		void quickSort(char** items, int left, int right);
		char* createBuffer (const char* pStart, const char* pEnd);
		
	private:
		
		char** m_strArray;
		unsigned int m_usedLen;
		unsigned int m_realLen;
		bool m_acending;
		bool m_caseSensitive;
		CMutex m_mutex;
};

#endif //_CBASIC_STRING_LIST_H_
