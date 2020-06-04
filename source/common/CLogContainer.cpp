/**
* \file CLogContainer.cpp
 * \brief Application's Log container.
 * \author avilar
 * \date 03/05/2010
 * \version 1.0.0
 * \details Implements all defined in CLogContainer.h.
 */
 

#include "CLogContainer.h"
#include "CMutex.h"
#include "CConfigContainer.h"


CLogContainer* CLogContainer::m_instance = NULL;

/**
 * \brief Class destructor
 */
CLogContainer::~CLogContainer()
{
	finalize();
}

/**
 * \brief Returns the only instanced object of this type
 * \details If the object is not created, call the constructor first
 * \param none
 * \return An unique instance of this class
 */
CLogContainer* CLogContainer::getInstance()
{
	if(CLogContainer::m_instance == NULL)
	{
		CLogContainer::m_instance = new CLogContainer();
	}
	return CLogContainer::m_instance;
}

/**
 * \brief Initializes the container of CLogger
 * \details 
 * \param[in] configIniFile The configuration ini files that contains the list of CLoggers to create
 * \return The CLogger object
 */
bool CLogContainer::initialize(const char* configIniFile)
{
	m_mutex.request();

	CSectionContainer* section;
	
	if (!m_initialized)
	{
		m_loggerObjects.clear();
		CConfigContainer* cfgContainer = new CConfigContainer(configIniFile);
	
		
		if (cfgContainer->loadFromFile())
		{
			CLogContainer::CLogElement* logElmt = NULL;
			
			section = cfgContainer->getFirstSection();

			while (section != NULL)
			{			
				char component[64] = {0};
				char config[256] = {0};
				if(section->getFirstKeyPair(component, config))
				{				
					do{
						logElmt = new CLogContainer::CLogElement;
						strcpy(logElmt->application, section->getName());
						strcpy(logElmt->component, component);
						strcpy(logElmt->config, config);						
						logElmt->logger = NULL;//createLogger(config);
						m_loggerObjects.add(logElmt);
						logElmt = NULL;
					} while(section->getNextKeyPair(component, config));
				}
				section = cfgContainer->getNextSection();
			}
			m_initialized = true;
		}
		delete cfgContainer;
	}
	m_mutex.release();
	return m_initialized;
}

void CLogContainer::finalize()
{
	m_mutex.request();
	if (m_initialized)
	{
		int size = m_loggerObjects.count();
		for(int i = 0; i < size; i++)
		{
			if(m_loggerObjects[i]->logger != NULL)
			{
				delete m_loggerObjects[i]->logger;
			}
			delete m_loggerObjects[i];		
		}
		m_loggerObjects.clear();
		m_initialized = false;
	}
	m_mutex.release();
}


void CLogContainer::setLogPriority(LOG_PRIORITY priority)
{
	m_mutex.request();
	if (m_initialized)
	{
		m_logPriority = priority;
		int size = m_loggerObjects.count();
		for(int i = 0; i < size; i++)
		{
			if (m_loggerObjects[i]->logger)
			{
				m_loggerObjects[i]->logger->setPriority(priority);
			}
		}
	}
	m_mutex.release();
}
/**
 * \brief Gets CLogger object of the certain name
 * \details 
 * \param[in] applicationID The application [section] configured in configuration file
 * \param[in] componentName The name [item] configured in configuration file
 * \return The CLogger object
 */
CLogger* CLogContainer::getLogger(const char* applicationID, const char* componentName)
{
	if (m_initialized)
	{
		int size = this->m_loggerObjects.count();
		
		for(int i = 0; i < size; i++)
		{		
			if ((strcmp(applicationID, m_loggerObjects[i]->application) == 0) &&
				(strcmp(componentName, m_loggerObjects[i]->component) == 0))
			{
				if(m_loggerObjects[i]->logger == NULL)
				{
					m_loggerObjects[i]->logger = createLogger(m_loggerObjects[i]->config);
				}
				return m_loggerObjects[i]->logger;
			}
		}
	}
	return NULL;
}

/**
 * \brief Class constructor
 */
CLogContainer::CLogContainer()
{
	m_loggerObjects.clear();
	m_initialized = false;
	m_logPriority = P_TRACE;
}

/**
 * \brief Creates the logger according to your settings
 * \details 
 * \param[in] config The configurations
 * \return The CLogger object
 */
CLogger* CLogContainer::createLogger(const char* config)
{
	CBasicStringList bslConfig;
	bslConfig.clear();
	bslConfig.deserialize(config, "|");
	
	if(bslConfig.count() == 5)
	{
		printf("Creating logger. Params[%s, %s, %d, %d, %d]\n", bslConfig[1], //Log name
																 bslConfig[0], //Log path
																 atoi(bslConfig[2]), //Max Backups
																 atoi(bslConfig[3]), //Max file size
																 atoi(bslConfig[4]));//Trace Level
							 
		return new CLogger(bslConfig[1], //Log name
							 bslConfig[0], //Log path
							 atoi(bslConfig[2]), //Max Backups
							 atoi(bslConfig[3]), //Max file size
							 static_cast<LOG_PRIORITY>(atoi(bslConfig[4])));//Trace Level
	}
	else
	{
		return NULL;
	}
}
