#include <pthread.h>
#include <stdlib.h> /*for malloc*/
#include <string.h> /*for strcmp*/
#include <fcntl.h> /*for open*/
#include <unistd.h> /*for close*/
#include <sys/stat.h> /*for flags*/

#include "ADTErr.h"
#include "GData.h"
#include "safeQueue.h"
#include "cdr.h"
#include "Operator.h"
#include "Subscriber.h"
#include "OperatorDB.h"
#include "SubscriberDB.h"
#include "DataManager.h"

#define Q_IS_EMPTY_KEY "End"
/*if m_magic == MAGIC then struct is valid*/
#define MAGIC (void*)0xDEADBEEF
#define INVALID_MNGR_PRMS(params) !params || MAGIC != params->m_magic
#define FAILED_DATA_LOG_NAME "FailedData.txt"

struct DBManagerParams
{
	SubscriberDB* m_subDB;
	OperatorDB* m_oprDB;
	pthread_mutex_t m_DBMutex;
	SafeQueue* m_safeQ;
	void* m_magic;
}; 

static pthread_t s_feederThread;

static ADTErr PrepData (SafeQueue* _safeQ, Subscriber** _newSubscriber, Operator** _newOperator);
static ADTErr InsertSub2DB (SubscriberDB* _subDB, Subscriber* _newSubscriber, char* _key);
static ADTErr InsertOpr2DB (OperatorDB* _oprDB, Operator* _newOperator, char* _key);
static void* DBFeeder(void* _params);
/*if only one is needed send the other with NULL*/
static void LogFailedData (Subscriber* _sub, Operator* _opr);

DBManagerParams* InitDBManager (SafeQueue* _safeQ, ADTErr* _error)
{	
	DBManagerParams* params;
	ADTErr errorCheck;
	if (!_safeQ)
	{
		if (_error)
		{
			*_error = ERR_NOT_INITIALIZED;
		}
		return NULL;
	}
	/*FREE IN DESTROY FUNC*/
	params = malloc(sizeof(DBManagerParams));
	if (!params)
	{
		if (_error)
		{
			*_error = ERR_ALLOCATION_FAILED;
		}
		return NULL;
	}
	params->m_magic = MAGIC;
	/*create SBI data base*/
	params->m_subDB = SubscriberDBCreate (&errorCheck);
	if (ERR_OK != errorCheck || !params->m_subDB)
	{
		if (_error)
		{
			*_error = ERR_INTERNAL_DB_FAIL;
		}
		return NULL;
	}
	/*create OBI data base*/
	params->m_oprDB = OperatorDBCreate (&errorCheck);
	if (ERR_OK != errorCheck || !params->m_oprDB)
	{
		SubscriberDBDestroy (params->m_subDB);
		if (_error)
		{
			*_error = ERR_INTERNAL_DB_FAIL;
		}
		return NULL;
	}
	/*create DBMutex*/
	if (0 != pthread_mutex_init(&params->m_DBMutex, NULL))
	{
		SubscriberDBDestroy (params->m_subDB);
		OperatorDBDestroy (params->m_oprDB);
		if (_error)
		{		
			*_error = ERR_INTERNAL_DB_FAIL;
		}
		return NULL;
	}
	if (0 != pthread_create(&s_feederThread, NULL, DBFeeder, params))
	{
		SubscriberDBDestroy (params->m_subDB);
		OperatorDBDestroy (params->m_oprDB);
		pthread_mutex_destroy (&params->m_DBMutex);
		if (_error)
		{		
			*_error = ERR_INTERNAL_DB_FAIL;
		}
		return NULL;
	}
	if (_error)
	{
		*_error = ERR_OK;
	}
	return params;
}	

static void* DBFeeder(void* _params)
{
	Subscriber* newSubscriber = NULL;
	Operator* newOperator = NULL;
	char imsi[30];
	char oprName[30];
	DBManagerParams* params = _params;

	while (1)
	{
		/*if data is invalid then skip to next in Q*/
		if (ERR_OK != PrepData (params->m_safeQ, &newSubscriber, &newOperator))
		{
			continue;
		}
		/*get the key for the DB*/
		if (ERR_OK != SubscriberGetIMSI (newSubscriber, imsi))
		{
			LogFailedData (newSubscriber, newOperator);
			SubscriberDestroy (newSubscriber);
			OperatorDestroy (newOperator);
			continue;
		}
		/*check if key is an ending signal*/
		if (0 == strcmp (imsi, Q_IS_EMPTY_KEY))
		{
			SubscriberDestroy (newSubscriber);
			OperatorDestroy (newOperator);
			pthread_exit (NULL);
		}
		if (ERR_OK != OperatorGetName(newOperator, oprName))
		{
			LogFailedData (newSubscriber, newOperator);
			SubscriberDestroy (newSubscriber);
			OperatorDestroy (newOperator);
			continue;
		}
		if (0 != pthread_mutex_lock(&params->m_DBMutex))
		{
			LogFailedData (newSubscriber, newOperator);
			SubscriberDestroy (newSubscriber);
			OperatorDestroy (newOperator);			
			pthread_exit (NULL);
		}
		if (ERR_OK != InsertSub2DB (params->m_subDB, newSubscriber, imsi))	
		{
			LogFailedData (newSubscriber, newOperator);
			SubscriberDestroy (newSubscriber);
			OperatorDestroy (newOperator);
			if (0 != pthread_mutex_unlock (&params->m_DBMutex))
			{
				pthread_exit (NULL);
			}		
			continue;
		}
		if (ERR_OK != InsertOpr2DB (params->m_oprDB, newOperator, imsi))
		{
		/*problem if insert of sub succeeded*/
			LogFailedData (newSubscriber, newOperator);
			SubscriberDestroy (newSubscriber);
			OperatorDestroy (newOperator);
			if (0 != pthread_mutex_unlock (&params->m_DBMutex))
			{
				pthread_exit (NULL);
			}		
			continue;
		}
		if (0 != pthread_mutex_unlock (&params->m_DBMutex))
		{
			pthread_exit (NULL);
		}
	}
	pthread_exit (NULL);
}

static ADTErr PrepData (SafeQueue* _safeQ, Subscriber** _newSubscriber, Operator** _newOperator)
{
	CDR* cdr = NULL;
	ADTErr errorCheck;
	/*get CDR*/
	if (ERR_OK != SafeQueuePop (_safeQ, (void**)&cdr))
	{
		return ERR_INTERNAL_DB_FAIL;
	}
	/*seperate CDR to subscriber and operator*/
	*_newSubscriber = SubscriberCreate (cdr, &errorCheck);
	if (ERR_OK != errorCheck || !*_newSubscriber)
	{
		return ERR_INTERNAL_DB_FAIL;
	}
	*_newOperator = OperatorCreate (cdr, &errorCheck);		
	if (ERR_OK != errorCheck || !*_newOperator)
	{
		LogFailedData (*_newSubscriber, NULL);
		SubscriberDestroy (*_newSubscriber);
		return ERR_INTERNAL_DB_FAIL;
	}
	CDRDestroy (cdr);
	return ERR_OK;
}

static ADTErr InsertSub2DB (SubscriberDB* _subDB, Subscriber* _newSubscriber, char* _key)
{
	Subscriber* existSubscriber = NULL;
	/*update if exists*/
	if (ERR_OK == SubscriberDBGet (_subDB, _key, &existSubscriber))
	{
		if (ERR_OK != SubscriberUpdate (existSubscriber, _newSubscriber))
		{
			return ERR_GENERAL;
		}
		SubscriberDestroy (_newSubscriber);
	}
	/*if doesn't exist, needs insert*/
	else if (ERR_OK != SubscriberDBInsert(_subDB, _newSubscriber))
	{
		return ERR_GENERAL;
	}
	return ERR_OK;
}

static ADTErr InsertOpr2DB (OperatorDB* _oprDB, Operator* _newOperator, char* _key)
{
	Operator* existOperator = NULL;
	if (ERR_OK == OperatorDBGet (_oprDB, _key, &existOperator))
	{
		if (ERR_OK != OperatorUpdate (existOperator, _newOperator))
		{
			return ERR_GENERAL;
		}
		OperatorDestroy (_newOperator);
	}
	/*if get didn't succeed, needs insert*/
	else if (ERR_OK != OperatorDBInsert(_oprDB, _newOperator))
	{
		return ERR_GENERAL;
	}	
	return ERR_OK;
}

/*If want to print only one- the other should be marked as NULL*/
static void LogFailedData (Subscriber* _sub, Operator* _opr)
{
	int fileDesc;
	if (!_sub && !_opr)
	{
		return;
	}
	fileDesc = open (FAILED_DATA_LOG_NAME, O_RDWR | O_APPEND | O_CREAT, S_IRUSR | S_IWUSR);
	if (fileDesc < 0)
	{
		return;
	}
	if (_sub)
	{
		SubscriberPrintToFile (_sub, fileDesc);
	}
	if (_opr)
	{
		OperatorPrintToFile (_opr, fileDesc);
	}
	if (-1 == close (fileDesc))
	{
		return;
	}
}

ADTErr EndDBManager (DBManagerParams* _params)		
{
	if (INVALID_MNGR_PRMS(_params))
	{
		return ERR_NOT_INITIALIZED;
	}
	if (0 != pthread_join(s_feederThread, NULL))
	{
		return ERR_GENERAL;
	}
	return ERR_OK;
}		
		
ADTErr DestroyDBManager (DBManagerParams* _params)
{
	if (INVALID_MNGR_PRMS(_params))
	{
		return ERR_NOT_INITIALIZED;
	}
	SubscriberDBDestroy (_params->m_subDB);
	OperatorDBDestroy (_params->m_oprDB);
	pthread_mutex_destroy (&_params->m_DBMutex);
	_params->m_magic = NULL;
	free (_params);
	return ERR_OK;
}		
		
ADTErr GetSubscriberDB (DBManagerParams* _params, SubscriberDB** _subDB)
{
	if (INVALID_MNGR_PRMS(_params) || !_subDB)
	{
		return ERR_NOT_INITIALIZED;
	}
	*_subDB = _params->m_subDB;
	return ERR_OK;
}

ADTErr GetOperatorDB (DBManagerParams* _params, OperatorDB** _oprDB)
{
	if (INVALID_MNGR_PRMS(_params) || !_oprDB)
	{
		return ERR_NOT_INITIALIZED;
	}
	*_oprDB = _params->m_oprDB;
	return ERR_OK;
}

ADTErr GetDBMutex (DBManagerParams* _params, pthread_mutex_t* _DBMutex)
{
	if (INVALID_MNGR_PRMS(_params) || !_DBMutex)
	{
		return ERR_NOT_INITIALIZED;
	}
	*_DBMutex = _params->m_DBMutex;
	return ERR_OK;
}

