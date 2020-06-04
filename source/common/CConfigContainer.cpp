///////////////////////////////////////////////////////////////////////////////
// Descricao:
// Autor: Amadeu Vilar Filho / Luis Gustavo de Brito
// Data de criacao: 11/12/2007
//
// Alteracoes:
//
///////////////////////////////////////////////////////////////////////////////

#include "CConfigContainer.h"
#include "CConfigSerializer.h"
#include "CBuffer.h"

#define THIS_FILE "CConfigContainer.cpp"


CConfigContainer::CConfigContainer(const char* fileName) 
{
	strcpy (m_fileName, fileName);
	m_section = new CList<CSectionContainer*>(CConfigContainer::onDelete, NULL);
	m_sectionNames.clear();
	m_section->clear();
}

CConfigContainer::~CConfigContainer() {
	this->clearSections();

	delete m_section;
}

void CConfigContainer::clearSections() {
	m_sectionNames.clear();
	m_section->clear();
}

void CConfigContainer::setSection(CSectionContainer* section) {
	
	int idx = m_sectionNames.indexOf(section->getName());

	//so adiciona se nao existir
	if (idx < 0)
	{
		m_sectionNames.add(section->getName());
		m_section->add(section);
	}
	//m_sectionContainer[section->getName()] = section;
}

CSectionContainer* CConfigContainer::getSection(const char* name) {

	CSectionContainer* ret = NULL;
	int idx;

	idx = m_sectionNames.indexOf(name);

	if (idx >= 0)
	{
		ret = m_section->at(idx);
	}
	else
	{
		ret = new CSectionContainer(name);
		this->setSection(ret);
	}

	return ret;
}

CSectionContainer* CConfigContainer::getFirstSection() {
	m_containerIndex = 0;

	if(m_sectionNames.count() <= 0) {
		return NULL;
	}
	else {
		return m_section->at(0);
	}
}

CSectionContainer* CConfigContainer::getNextSection() {
	m_containerIndex++;
	if(m_containerIndex >= m_sectionNames.count()) {
		return NULL;
	}
	else {
		return m_section->at(m_containerIndex);
	}
}

bool CConfigContainer::saveToFile() {
	return this->saveToFile(m_fileName);
}

bool CConfigContainer::loadFromFile() {
	return this->loadFromFile(m_fileName);
}

//implementin ISerializable interface
bool CConfigContainer::saveToFile(const char* fileName) {
	CConfigSerializer serializer(this, fileName);
	return serializer.serialize();
}

bool CConfigContainer::loadFromFile(const char* fileName) {
	CConfigSerializer serializer(this, fileName);
	this->clearSections();
	return serializer.deserialize();
}


//apaga elementos de sessao
void CConfigContainer::onDelete(CSectionContainer** obj) {
	delete *obj;
}


//Retorna o valor da chave, na sessao no formato string
const char* CConfigContainer::getValueStr(const char* section, const char* key)
{
	const char* ret = NULL;

	CSectionContainer* pSection = this->getSection(section);
	if (pSection)
	{
		ret = pSection->getKeyValue(key);
	}
	return ret;
}

bool CConfigContainer::getValueStr(const char* section, const char* key, char* output, int outputLen)
{
	bool ret = false;
	CSectionContainer* pSection = this->getSection(section);
	if (pSection)
	{
		const char* pValue = pSection->getKeyValue(key);
		if (pValue)
		{
			int pValueLen = strlen (pValue);
			if (pValueLen  < outputLen)
			{
				strcpy (output, pValue);
				ret = true;
			}
		}
	}
	return ret;
}

//Retorna o valor da chave, na sessão, no formato de lista de strings.
bool CConfigContainer::getValueList(const char* section, const char* key, CBasicStringList* outList, const char* separator)
{
	bool ret = false;

	if( outList )
	{
		const char* value = getValueStr(section, key);
		if( value )
		{
			outList->add(value, separator);
			ret = true;
		}
	}

	return ret;
}

bool CConfigContainer::getValueList(const char* section, const char* key, CIndexedStringTable* outList, const char* separator)
{
	bool ret = false;

	if( outList )
	{
		const char* value = getValueStr(section, key);
		if( value )
		{
			CBasicStringList bslAux;
			bslAux.add(value, separator);
			int c = bslAux.count();

			for (int i = 0; i < c; i++)
			{
				outList->setValue(bslAux.elmtAt(i));
			}

			ret = true;
		}
	}

	return ret;
}


//Retorna o valor da chave, na sessao, no formato inteiro
bool CConfigContainer::getValueInt(const char* section, const char* key, int& out)
{
	bool ret = false;

	CSectionContainer* pSection = this->getSection(section);
	if (pSection)
	{
		const char* pValue = pSection->getKeyValue(key);
		if (pValue)
		{
			out = atoi(pValue);
			ret = true;
		}
	}	
	return ret;
}

#ifdef WIN32
	bool CConfigContainer::getValueInt(const char* section, const char* key, __int64& out)
	{
		bool ret = false;
	
		CSectionContainer* pSection = this->getSection(section);
		if (pSection)
		{
			const char* pValue = pSection->getKeyValue(key);
			if (pValue)
			{
				out = _atoi64(pValue);
				ret = true;
			}
		}	
		return ret;
	}
#else
	bool CConfigContainer::getValueInt(const char* section, const char* key, long long& out)
	{
		bool ret = false;

		CSectionContainer* pSection = this->getSection(section);
		if (pSection)
		{
			const char* pValue = pSection->getKeyValue(key);
			if (pValue)
			{
				out = atoll(pValue);
				ret = true;
			}
		}	
		return ret;
	}
#endif

//Retorna o valor da chave, na sessao, no formato binario
bool CConfigContainer::getValueBin(const char* section, const char* key, unsigned char* output,
								   int allocdLen, int& outputLen)
{
	bool ret = false;
	CSectionContainer* pSection = this->getSection(section);
	if (pSection)
	{
		const char* keyValue = pSection->getKeyValue(key);
		if (keyValue)
		{
			CBuffer b(keyValue);
			b.pack();
			memcpy (output, b.getBuffer(), b.length());
			outputLen = b.length();
			
			//this->convertAsciiToBin(keyValue, output, allocdLen, outputLen);
			ret = true;
		}
	}

	return ret;
}


//Armazena o valor da chave, na sessao, no formato string
bool CConfigContainer::setValueStr(const char* section, const char* key, const char* value)
{
	bool ret = false;
	CSectionContainer* pSection = this->getSection(section);
	if (pSection)
	{
		if (pSection->updateValue(key, value))
		{
			ret = true;
		}
	}
	return ret;
}


//Armazena o valor da chave, na sessao, no formato inteiro convertido para string
bool CConfigContainer::setValueInt(const char* section, const char* key, int value)
{
	char szValue[32] = {0};
	sprintf (szValue, "%d", value);
	return setValueStr(section, key, szValue);
}

//Armazena o valor da chave, na sessao, no formato inteiro convertido para string
#ifdef WIN32
	bool CConfigContainer::setValueInt(const char* section, const char* key, __int64 value)
	{
		char szValue[32] = {0};
		sprintf (szValue, HST_LONGLONG_FORMAT, value);
		return setValueStr(section, key, szValue);
	}
#else
	bool CConfigContainer::setValueInt(const char* section, const char* key, unsigned long long value)
	{
		char szValue[32] = {0};
		sprintf (szValue, "%lld", value);
		return setValueStr(section, key, szValue);
	}
#endif
	
//Armazena o valor da chave, na sessao, no formato binario convertido para string
bool CConfigContainer::setValueBin(const char* section, const char* key, unsigned char* input, int inputLen)
{
	CBuffer b(input, inputLen);
	b.unpack();
	return setValueStr(section, key, b.getAsciiBuffer());
}

int CConfigContainer::count()
{
	if(m_section)
	{
		return m_section->count();
	}
	else
	{
		return 0;
	}
}
