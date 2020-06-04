///////////////////////////////////////////////////////////////////////////////
// Descricao:
// Autor: Amadeu Vilar Filho / Luis Gustavo de Brito
// Data de criacao: 11/12/2007
//
// Alteracoes:
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __C_CONFIG_CONTAINER_H__
#define __C_CONFIG_CONTAINER_H__

#include "CSectionContainer.h"
#include "ISerializable.h"
#include "CList.h"
#include "CBasicStringList.h"
#include "CIndexedStringTable.h"


class CConfigContainer: public ISerializable 
{
	public:
		//static CConfigContainer* getInstance();
		CConfigContainer(const char* fileName);
		virtual ~CConfigContainer();
		
		void clearSections();
		void setSection(CSectionContainer* section);
		CSectionContainer* getSection(const char* name);
		
		int count();

		CSectionContainer* getFirstSection();
		CSectionContainer* getNextSection();
		
		//implementin ISerializable interface
		virtual bool saveToFile(const char* fileName);
		virtual bool loadFromFile(const char* fileName);

		virtual bool saveToFile();
		virtual bool loadFromFile();

		const char* getValueStr(const char* section, const char* key);

		bool getValueStr(const char* section, const char* key, char* output, int outputLen);

		bool getValueList(const char* section, const char* key, CBasicStringList* outList, const char* separator = "|");

		bool getValueList(const char* section, const char* key, CIndexedStringTable* outList, const char* separator = "|");

		bool getValueInt(const char* section, const char* key, int& out);

#ifdef WIN32
		bool getValueInt(const char* section, const char* key, __int64& out);
#else
		bool getValueInt(const char* section, const char* key, long long& out);
#endif

		bool getValueBin(const char* section, const char* key, unsigned char* output,
						 int allocdLen, int& outputLen);

		bool setValueStr(const char* section, const char* key, const char* value);

		bool setValueInt(const char* section, const char* key, int value);

#ifdef WIN32
		bool setValueInt(const char* section, const char* key, __int64 value);
#else
		bool setValueInt(const char* section, const char* key, unsigned long long value);
#endif

		bool setValueBin(const char* section, const char* key, unsigned char* input,
						 int inputLen);

	private:
			
		static void onDelete(CSectionContainer** obj); 

		CBasicStringList m_sectionNames;
		CList<CSectionContainer*>* m_section;
		char m_fileName[256];
		int m_containerIndex;
				
};
#endif //__C_CONFIG_CONTAINER_H__
