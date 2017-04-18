#include <pthread.h>
#include <signal.h>
#include <stdio.h> /*for perror*/

#include "ADTErr.h"
#include "GData.h"
#include "safeQueue.h"
#include "cdr.h"
#include "Operator.h"
#include "Subscriber.h"
#include "OperatorDB.h"
#include "SubscriberDB.h"
#include "DataManager.h"
#include "Billing.h"

#define SUB_DB_FILE "SubscribersInfos.txt"
#define OPR_DB_FILE "OperatorsInfos.txt"

static pthread_t s_billThread;
static pthread_cond_t s_waitForSig = PTHREAD_COND_INITIALIZER;

void handler (int sig)
{
	pthread_cond_signal(&s_waitForSig);
}

static void* ExportBills (void* _params)
{
	DBManagerParams* mngParams = (DBManagerParams*)_params;
	pthread_mutex_t DBMutex;
	SubscriberDB* subDB;
	OperatorDB* oprDB;
	if (ERR_OK != GetDBMutex (mngParams, &DBMutex))
	{
		pthread_exit (NULL);
	}
	if (ERR_OK != GetSubscriberDB(mngParams, &subDB))
	{
		pthread_exit (NULL);
	}
	if (ERR_OK != GetOperatorDB(mngParams, &oprDB))
	{
		pthread_exit (NULL);
	}	
	if (0 != pthread_mutex_lock(&DBMutex))
	{
		pthread_exit (NULL);
	}	
	/*wait for signal*/
	if (0 != pthread_cond_wait(&s_waitForSig, &DBMutex))
	{
		pthread_mutex_unlock (&DBMutex);
		pthread_exit (NULL);
	}
	/*start exporting bills*/
	SubscriberDBPrintToFile (subDB, SUB_DB_FILE);
	OperatorDBPrintToFile (oprDB, OPR_DB_FILE);
	if (0 != pthread_mutex_unlock (&DBMutex))
	{
		pthread_exit (NULL);
	}
	pthread_exit (NULL);
}

ADTErr BillingInit (DBManagerParams* _mngParams)
{
	struct sigaction sig;
	if (!_mngParams)
	{
		return ERR_NOT_INITIALIZED;
	}
	/*set default for signal SIGUSR1*/
	sig.sa_handler = &handler;
	if (-1 == sigaction(SIGUSR1, &sig, NULL))
	{
		perror ("sigaction");
		return ERR_GENERAL;
	}
	/*start billing thread*/
	if (0 != pthread_create(&s_billThread, NULL, ExportBills, _mngParams))
	{	
		return ERR_GENERAL;
	}
	return ERR_OK;
}

ADTErr EndBilling (void)		
{
	if (0 != pthread_join(s_billThread, NULL))
	{
		return ERR_GENERAL;
	}
	return ERR_OK;
}	

