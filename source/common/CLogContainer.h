/**
 * \file CLogContainer.h
 * \brief The CLogContainer class definition.
 * \details <BR>(c) COPYRIGHT 2013 HST EQUIPAMENTOS<BR>
 * ELETRONICOS Ltda, Campinas (SP), Brasil<BR>
 * ALL RIGHTS RESERVED - TODOS OS DIREITOS RESERVADOS<BR>
 * CONFIDENTIAL, UNPUBLISHED PROPERTY OF HST E. E. Ltda<BR>
 * PROPRIEDADE CONFIDENCIAL NAO PUBLICADA DA HST Ltda.<BR><BR> 
 * A HST nao se responsabiliza pelo uso indevido de seu codigo.<BR>
 */
 
#ifndef _HSTAPI_CLOGCONTAINER_H_
#define _HSTAPI_CLOGCONTAINER_H_

#include "clogger.h"
#include "CDynamicArray.h"


/**
* \class CLogContainer
 * \brief CLogContainer factory
 * \author carlos_franciscatto
 * \date 10/12/2013
 * \version 1.0.0
 * \details Factory for the CLogContainer objects (based on version from avilarCLogContainer from IsoApplication-HST Framework).
 */
class CLogContainer
{
	public:
		virtual ~CLogContainer();
		static CLogContainer* getInstance();
		bool initialize(const char* configIniFile);
		void finalize();
		void setLogPriority(LOG_PRIORITY priority);
		CLogger* getLogger(const char* applicationID, const char* componentName);	
	protected:
	private:
		struct CLogElement
		{
			char application[64];
			char component[64];
			char config[256];
			CLogger* logger;
		};

		/**
		 * Unique instance of this object.
		 */
		static CLogContainer* m_instance;
		/**
		 * Determines if the instance was initialized
		 */
		bool m_initialized;
		/**
		 * Mutex
		 */
		CMutex m_mutex;
		/**
		 * List of created CLogger objects.
		 */		
		CDynamicArray<CLogElement*> m_loggerObjects;
		LOG_PRIORITY m_logPriority;
	private:
		CLogContainer();
		CLogger* createLogger(const char* config);
};

#endif /*_HSTAPI_CLOGCONTAINER_H_*/
