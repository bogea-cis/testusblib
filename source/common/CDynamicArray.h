///////////////////////////////////////////////////////////////////////////////
// Descricao:
// Autor: Luis Gustavo de Brito
// Data de criacao: 27/07/2010
//
// Alteracoes:
// 
///////////////////////////////////////////////////////////////////////////////

#ifndef _HSTAPI_C_DYNAMIC_ARRAY_H_
#define _HSTAPI_C_DYNAMIC_ARRAY_H_

#ifdef __cplusplus

#include <stdlib.h>
#include "CMutex.h"


const unsigned int DYNAMIC_ARRAY_DEFAULT_BLOCK_LEN = 16;

template <class T> class CDynamicArray
{
public:
	typedef void (*ON_DELETE) (T* obj);
	typedef int (*ON_COMPARE) (T lObj, T rObj);
	typedef int (*ON_COMPARE_STR) (const char* str, T obj);
	
	CDynamicArray(bool sorted = false, ON_DELETE onDelete = NULL, ON_COMPARE onCompare = NULL, int blockLen = DYNAMIC_ARRAY_DEFAULT_BLOCK_LEN)
	{
		this->m_blockLen = blockLen;
		this->m_elmts = NULL;
		this->m_count = this->m_realCount = 0;
		this->fOnDelete = onDelete;
		this->fOnCompare = onCompare;
		this->m_sorted = sorted;
		this->m_acending = true;
		
		this->allocMemory(1);
	};
	
	virtual ~CDynamicArray()
	{
		clear();

		if (m_elmts)
		{
			delete[] m_elmts;
			m_elmts = NULL;
		}
	};
	
	virtual bool lock ()
	{
		return m_mutex.request();
	}

	virtual void unlock ()
	{
		m_mutex.release();
	}
	
	virtual void add(T obj)
	{
		if (m_sorted && fOnCompare && m_count > 0)
		{
			int idx = getInsertionIndex(0, m_count, obj);
			insert ((int)idx, obj);
		}
		else
		{
			insert ((int)m_count, obj);
		}
	};
		
	virtual bool remove(int index)
	{
		bool ret = false;
		if (index < 0 || index >= m_count)
		{
			return ret;
		}

		if (fOnDelete)
		{
			fOnDelete(&m_elmts[index]);
		}

		int i;
		for (i = index; i < m_count; i++)
		{
			m_elmts[i] = m_elmts[i + 1];
		}
		m_elmts[i - 1] = NULL;

		m_count --;

		ret = true;


		return ret;
	};
	
	virtual int indexOf(T obj)
	{
		if (m_count <= 0 || 
			fOnCompare == NULL)
		{
			return -1;
		}
		
		if (m_sorted)
		{
			return search (0, m_count, obj);
		}
		else
		{
			int c = m_count;
			for (int i = 0; i < c; i++)
			{
				if (fOnCompare(obj, m_elmts[i]) == 0)
				{
					return i;
				}
			}
			return -1;
		}
	};
	
	virtual int count()
	{
		return (int)m_count;
	};
	
	virtual void clear()
	{
		//apaga todos os itens
		int c = m_count;

		for (int i = c - 1; i >=0; i--)
		{
			remove(i);
		}

		m_count = 0;
	};

	
	virtual T operator[] (unsigned int index)
	{
		return at(index);
	};
	
	
	virtual T at (int index)
	{
		if (index < 0 || index >= m_count)
		{
			return NULL;
		}

		return m_elmts[index];
	};
	
	bool sort(bool ascending = true)
	{
		bool ret = false;
		
		if (fOnCompare != NULL)
		{
			m_acending = ascending;
			quickSortMain(m_elmts, m_count);
			ret = true;
		}

		return ret;
	};

private:

	virtual void insert(int index, T obj)
	{
		if (index < 0 || index > m_count)
		{
			return;
		}

		allocMemory (1);

		if (m_count == 0)
		{
			m_elmts[0] = obj;
			m_count++;
			
		}
		else
		{
			int i;

			//desloca todos os elementos para a direita
			for (i = m_count; i > index; i--)
			{
				m_elmts[i] = m_elmts[i - 1];
			}
			
			m_elmts[i] = obj;
			m_count++;
		}

	};

	//virtual int getInsertionIndex (int startIdx, int endIdx, T obj)
	virtual int getInsertionIndex (int startIdx, int count, T obj)
	{
		int m = startIdx + (count / 2);

		if (count <= 1)
		{
			if (fOnCompare(obj, m_elmts[startIdx]) > 0)
			{
				return startIdx + 1;
			}
			else
			{
				return startIdx;
			}
		}

		int comp = fOnCompare(obj, m_elmts[m]);
		if (comp >= 0)
		{
			return getInsertionIndex (m, count - (count / 2), obj);
		}
		else
		{
			return getInsertionIndex (startIdx, count - (count / 2), obj);
		}

	}
	
	virtual int search (int startIdx, int count, T obj)
	{
		if (count <= 1)
		{
			int comp = fOnCompare(obj, m_elmts[0]);
			if (comp == 0)
			{
				return 0;
			}
			else
			{
				return -1;
			}
		}
		
		int m = startIdx + (count / 2);
		int comp = fOnCompare(obj, m_elmts[m]);
		if (comp > 0)
		{
			return search (m, count - (count / 2), obj);
		}
		else if (comp < 0)
		{
			return search (startIdx, count - (count / 2), obj);
		}
		else
		{
			return m;
		}
	}



	virtual void allocMemory(int numberOfElmtsToInsert)
	{

		//verifica se precisa alocar mais memoria
		if (m_count + numberOfElmtsToInsert > m_realCount)
		{
			int numberOfBytes = (m_count + numberOfElmtsToInsert) - m_realCount;
			int numberOfBlocks = 0;

			if (numberOfBytes % m_blockLen > 0)
			{
				//adiciona 1 para o resto
				numberOfBlocks++;
			}

			numberOfBlocks += (numberOfBytes) / m_blockLen;


			//aloca blocos de memoria
			int newRealCount = m_realCount + (numberOfBlocks * m_blockLen);
			T* newArray = new T[newRealCount];
			
			for (int i = 0; i < newRealCount; i++)
			{
				if (i < m_count)
				{
					newArray[i] = m_elmts[i];
				}
				else
				{
					//espaco novo vai com NULL
					newArray[i] = NULL;
				}
			}
			
			//deleta o array de ponteiros, pq criamos um outro
			if (m_elmts)
			{
				delete[] m_elmts;
			}
			
			m_realCount = newRealCount;
			m_elmts = newArray;	
		}
	}

	void quickSortMain(T* items, int count)
	{
		if (count > 0)
			quickSort(items, 0, count-1);
	}

	void quickSort(T* items, int left, int right)
	{
		int i, j;
		T x;
		T temp;
		
		i = left;
		j = right;
		x = items[(left+right)/2];
		
		register int factor = (m_acending) ? 1 : (-1);
		do
		{
			while( (factor * fOnCompare(items[i], x) < 0) && (i < right))
			{i++;}
			while( (factor * fOnCompare(items[j], x) > 0) && (j > left))
			{j--;}

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


private:
	T* m_elmts;
	
	int m_blockLen;
	int m_count;
	int m_realCount;
	bool m_sorted;
	bool m_acending;
	ON_DELETE fOnDelete;
	ON_COMPARE fOnCompare;
	CMutex m_mutex;
};


#endif

#endif

