///////////////////////////////////////////////////////////////////////////////
// Descricao:
// Autor: Amadeu Vilar Filho / Luis Gustavo de Brito
// Data de criacao: 11/12/2007
//
// Alteracoes:
//
///////////////////////////////////////////////////////////////////////////////

#include "CConfigSerializer.h"
  

CConfigSerializer::CConfigSerializer(CConfigContainer* container, const char* fileName) {
	m_config = container;
	m_fileHandler = new CFileWriter(fileName);
}

CConfigSerializer::~CConfigSerializer() {
	delete m_fileHandler;
}

bool CConfigSerializer::serialize() {
	CSectionContainer* section = m_config->getFirstSection();
	if(section == NULL) {
		return false;
	}
	
	if(m_fileHandler->open(OM_WRITE) == false) {
		return false;
	}
	
	m_fileHandler->setLine("#-------------------------------------#");
	m_fileHandler->setLine("# File gererated by CConfigSerializer #");
	m_fileHandler->setLine("# Authors:                            #");
	m_fileHandler->setLine("#          Amadeu Vilar Filho         #");
	m_fileHandler->setLine("#          Luis Gustavo de Brito      #");
	m_fileHandler->setLine("#-------------------------------------#");
	
	do {
		char line[MAX_LINE_LENGHT] = {0};
		
		//writes the section name
		sprintf(line, "\n[%s]", section->getName());
		m_fileHandler->setLine(line);
		char key[MAX_LINE_LENGHT] = {0};
		char value[MAX_LINE_LENGHT] = {0};
		
		//writes every section's key
		if(section->getFirstKeyPair(key, value) == true) {
			do {
				sprintf(line, "%s=%s", key, value);
				m_fileHandler->setLine(line);
			}while(section->getNextKeyPair(key, value) == true);
		}
		
		section = m_config->getNextSection();
	}while(section != NULL);
	
	m_fileHandler->close();
	
	return true;
}

bool CConfigSerializer::deserialize() {
	if(m_fileHandler->open(OM_READ_ONLY) == false) {
		return false;
	}
	char* line = 0;
	while((line = m_fileHandler->getLine()) != NULL) {
		if(line[0] != 0) {
			if(isComment(line) == false) {
				if(isSection(line) == true) {
					loadSection(line);
				}
			}
		}
	}
	m_fileHandler->close();
	return true;
}

bool CConfigSerializer::isComment(char* line) {
	if((line[0] == '/')||
	(line[0] == ';')||
	(line[0] == '#')) {
		return true;
	}
	else {
		return false;
	}
}

bool CConfigSerializer::isSection(char* line) {
	return(line[0] == '[');
}

bool CConfigSerializer::loadSection(char* line) {
	//parse the section name
	line++;
	char* pLine = 0;
	for(pLine = line; *pLine != ']'; pLine++);
	*pLine = '\0';
	//crate a new logical section
	CSectionContainer* section = new CSectionContainer(line);
	
	char* newLine = 0;
	while((newLine = m_fileHandler->getLine()) != NULL) {
		if(newLine[0] != 0) {
			if(isComment(newLine) == false) {
				if(isSection(newLine) == true) {
					m_config->setSection(section);
					return this->loadSection(newLine);
				}
				else {
					int len;
					//get key name
					char* pName = 0;
					pLine = newLine;
					for(pLine = line; *pLine != '='; pLine++);
					*pLine = '\0'; //remove '='
					
					for(pName = (pLine - 1); (pName != newLine)&&((*pName == ' ')||(*pName == '\t')); pName--) {
						*pName = '\0';
					}
					len = strlen(newLine);
					pName = new char[len + 1];
					memset(pName, 0, len + 1);
					strcpy(pName, newLine);
					
					//get key value
					pLine++;
					char* pValue = 0;
					for(;((*pLine == ' ')||(*pLine == '\t'))&&(*pLine != '\0'); pLine++);

					//Amadeu - Now we remove the ' or " delimitters
					len = strlen(pLine);
					if(((pLine[0] == '\"')&&(pLine[len - 1] == '\"'))||
						((pLine[0] == '\'')&&(pLine[len - 1] == '\'')))
					{
						memcpy(pLine, &pLine[1], len - 2);
						pLine[len - 2] = 0;
					}
					//Amadeu - end
				

					len = strlen(pLine);
					
					pValue = new char[len + 1];
					memset(pValue, 0, len + 1);

					strcpy(pValue, pLine);

					//insert this pair into the container
					section->insertKey(pName, pValue);
					
					//libera memoria
					delete[] pName;
					delete[] pValue;
				}
			}
		}
	}
	m_config->setSection(section);
	return false;
}

