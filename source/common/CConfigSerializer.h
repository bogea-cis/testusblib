///////////////////////////////////////////////////////////////////////////////
// Descricao:
// Autor: Amadeu Vilar Filho / Luis Gustavo de Brito
// Data de criacao: 11/12/2007
//
// Alteracoes:
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __C_CONFIG_SERIALIZER_H__
#define __C_CONFIG_SERIALIZER_H__

#include "CConfigContainer.h"
#include "CFileWriter.h"


class CConfigSerializer 
{
	public:
		CConfigSerializer(CConfigContainer* container, const char* fileName);
		virtual ~CConfigSerializer();
		
		bool serialize();
		bool deserialize();

	private:
		CConfigContainer* m_config;
		CFileWriter* m_fileHandler;
		
		bool isComment(char* line);
		bool isSection(char* line);
		bool loadSection(char* line);
		void convertString (const char* in, char* out);

};

#endif //__C_CONFIG_SERIALIZER_H__
