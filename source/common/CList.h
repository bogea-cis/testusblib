///////////////////////////////////////////////////////////////////////////////
// Descricao:
// Autor: Amadeu Vilar Filho / Luis Gustavo de Brito
// Data de criacao: 11/12/2007
//
// Alteracoes:
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _C_LIST_H_
#define _C_LIST_H_

#include <stdlib.h>
#include "CMutex.h"
#include <stdio.h>


template <class T> class CList
{
public:
	typedef void (*ON_DELETE) (T* obj);
	typedef int (*ON_COMPARE) (T lObj, T rObj);
	
	CList(ON_DELETE onDelete = NULL, ON_COMPARE onCompare = NULL)
	{
		this->m_start = NULL;
		this->m_end   = NULL;
		this->m_count = 0;
		this->m_current = NULL;
		this->fOnDelete = onDelete;
		this->fOnCompare = onCompare;
	};
	
	virtual ~CList()
	{
		clear();
	};
	
	virtual void begin()
	{
		m_current = m_start;
	}
	
	virtual void end()
	{
		m_current = m_end;
	}
	
	virtual void next()
	{
		if (m_current)
		{
			m_current = m_current->pNext;
		}
	}
	
	virtual void prior()
	{
		if (m_current)
		{
			m_current = m_current->pPrior;
		}
	}
	
	virtual T currentObj()
	{
		if (m_current)
		{
			return m_current->pObj;
		}
		
		return 0;
	}

	virtual bool lock ()
	{
		return m_listMutex.request();
	}

	virtual void unlock ()
	{
		m_listMutex.release();
	}
	
	virtual void add(T obj)
	{
		insert ((int)m_count, obj);
	};

	virtual void insertSorted(T obj)
	{
		if (fOnCompare == NULL)
		{
			return;
		}

		if (m_count == 0)
		{
			this->add(obj);
			return;
		}

		CElement* elmt = getInsertionIndex(m_start, 0, m_count, obj);

		if (elmt)
		{
			//cria novo elemento
			CElement* newelmt = new CElement;
			newelmt->pObj = obj;


			CElement* after = elmt->pNext;
			CElement* before = elmt->pPrior;

			if (fOnCompare(newelmt->pObj, elmt->pObj) > 0)
			{
				//insere antes
	
				if (before == NULL)
				{
					//insere no inicio
					newelmt->pNext = m_start;
					newelmt->pPrior = NULL;

					m_start->pPrior = newelmt;
					m_start = newelmt;
				}
				else
				{
					//insere no meio (antes de elmt)
					newelmt->pNext = elmt;
					newelmt->pPrior = before;
					
					before->pNext = newelmt;
					elmt->pPrior = newelmt;
				}
			}
			else
			{
				//insere depois
				if (after == NULL)
				{
					//insere no fim
					newelmt->pNext = NULL;
					newelmt->pPrior = m_end;

					m_end->pNext = newelmt;
					m_end = newelmt;
				}
				else
				{
					//insere no meio (depois de elmt)
					newelmt->pNext = after;
					newelmt->pPrior = elmt;
					
					elmt->pNext = newelmt;
					after->pPrior = newelmt;
				}
			}

			//insere no fim
			m_count ++;
		}
	}

	virtual void insert(int index, T obj)
	{
		//verifica se eh o primeiro elemento
		if (m_start == NULL)
		{
			//atribui o objeto
			m_start = new CElement;
			m_start->pNext = NULL;
			m_start->pPrior = NULL;
			m_start->pObj = obj;
			
			//iguala o fim ao inicio
			m_end = m_start;
			
			m_count ++;
		}
		else
		{
			//cria novo elemento
			CElement* elmt = new CElement;
			elmt->pObj = obj;

			if (index == 0)
			{
				//insere no inicio
				elmt->pNext = m_start;
				elmt->pPrior = NULL;

				m_start->pPrior = elmt;
				m_start = elmt;
			}
			else if (index < (int)m_count)
			{
				CElement* after = this->elmtAt(index);
				CElement* before = after->pPrior;
				
				//insere no meio
				elmt->pNext = after;
				elmt->pPrior = before;
				
				before->pNext = elmt;
				after->pPrior = elmt;
			}
			else
			{
				//insere no fim
				elmt->pNext = NULL;
				elmt->pPrior = m_end;

				m_end->pNext = elmt;
				m_end = elmt;
			}			

			//insere no fim
			m_count ++;
		}
	};	
	
	virtual bool remove(unsigned int index)
	{
		//remove apenas 1 objeto
		return remove(index, 1);
	};
	
	
	virtual bool remove(unsigned int index, unsigned int count)
	{
		bool ret = false;
		CElement* elmt = elmtAt(index);
		
		if (elmt)
		{
			CElement* prior;
			CElement* next;
			
			prior = elmt->pPrior;
			next = elmtAt(index + count);
			
			//todo o bloco da lista
			if (!prior && !next)
			{
				m_start = NULL;
				m_end = NULL;
			}
			
			//bloco a esquerda da lista
			else if (!prior && next)
			{
				next->pPrior = NULL;
				m_start = next;
			}
			
			//bloco do meio da lista
			else if (prior && next)
			{
				prior->pNext = next;
				next->pPrior = prior;
			}
			
			//bloco a direita da lista
			else if (prior && !next)
			{
				prior->pNext = NULL;
				m_end = prior;
			}
			
			
			CElement* pNext;
			CElement* pFirstToBeDeleted;
			
			pFirstToBeDeleted = elmt;
			if (fOnDelete)
			{
				for (unsigned int i = index; i < index + count; i++)
				{
					pNext = elmt->pNext;
					
					fOnDelete(&elmt->pObj);
					
					elmt = pNext;
				}
			}
			
			elmt = pFirstToBeDeleted;
			for (unsigned int i = index; i < index + count; i++)
			{
				pNext = elmt->pNext;
				
				delete elmt;
				
				elmt = pNext;
				
				m_count --;
			}
			
			
			ret = true;
		}
		return ret;
	};
	
	virtual bool contains(T obj)
	{
		bool ret = false;
		
		if (fOnCompare)
		{
			for (unsigned int i = 0; i < m_count; i++)
			{
				if (fOnCompare(elmtAt(i)->pObj, obj) == 0)
				{
					ret = true;
					break;
				}
			}
		}
		
		return ret;
	};
	
	virtual int count()
	{
		return (int)m_count;
	};
	
	virtual void clear()
	{
		//apaga todos os itens
		remove(0, m_count);
	};
	
	virtual T operator[] (unsigned int index)
	{
		if (index >= 0 && index < m_count )
		{
			return elmtAt(index)->pObj;
		}
		return 0;
	};
	
	virtual T at (unsigned int index)
	{
		if (index >= 0 && index < m_count )
		{
			return elmtAt(index)->pObj;
		}
		return 0;
	};
	
private:
	
	struct CElement
	{
		CElement* pNext;
		CElement* pPrior;
		T pObj;
	};


	virtual CElement* getInsertionIndex (CElement* start, int startIdx, int endIdx, T obj)
	{
		int count = (endIdx - startIdx) / 2;

		if (count <= 0)
		{
			return start;
		}
		else
		{
			CElement* pelmt = start;
			for (int i = 0; i < count; i++)
			{
				pelmt = pelmt->pNext;
			}

			
			if (fOnCompare(obj, pelmt->pObj) > 0)
			{
				return getInsertionIndex (start, startIdx, count, obj);
			}
			else
			{
				return getInsertionIndex (pelmt, startIdx + count, endIdx, obj);
			}
		}
	}

	virtual CElement* elmtAt(int index) 
	{
		CElement* ret = NULL;
		
		if (index >= 0 && index < count() )
		{
			int i = 0;
			for (CElement* p = m_start; p != NULL ; p = p->pNext, i++)
			{
				if (i == index)
				{
					ret = p;
					break;
				}
			}
		}
		
		return ret;
	};
	
	
	private:
		
		unsigned long m_count;
		CElement* m_start;
		CElement* m_current;
		CElement* m_end;
		ON_DELETE fOnDelete;
		CMutex m_listMutex;
		ON_COMPARE fOnCompare;
		
};

#endif

