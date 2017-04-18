/**************************************************************************************************************************************
Creation Date : 9.11.2015
Last modified date 9.11.2015
Description : Test - CDR - Insight project 
**************************************************************************************************************************************/
#include <stdlib.h>

#include "ADTErr.h"
#include "safeQueue.h"
#include "cdr.h"
#include "Subscriber.h"
#include "SubscriberDB.h"
#include "Operator.h"
#include "OperatorDB.h"
#include "DataManager.h"
#include "Billing.h"
#include "parser.h"
#include "FilesReader.h"

#define Q_SIZE 10/*TODO*/


int main ()
{
	SafeQueue* safeQ;
	DBManagerParams* params;
	ADTErr err;
	/*create Q*/
	safeQ = SafeQueueInit (Q_SIZE);
	if(NULL == safeQ)
	{
		return -1;
	}
	/*call file manager thread*/
	if(ERR_OK != InitReaders (safeQ))
	{
		SafeQueueDestroy(safeQ);
		/*TODO printf err */
		return -1;
	}
	/*create data manager thread*/
	params = InitDBManager (safeQ, &err); 
	if(NULL == params)
	{
		SafeQueueDestroy(safeQ);
		/*TODO printf err */
		return -1;
	}
	/*init billing module*/
	if(ERR_OK != BillingInit (params))
	{
		SafeQueueDestroy(safeQ);
		/*TODO printf err */
		return -1;
	}
	/*join all threads*/
	if(ERR_OK != EndReaders(safeQ))
	{
		SafeQueueDestroy(safeQ);
		/*TODO printf err */
		return -1;
	}
	if(ERR_OK != EndDBManager(params))
	{
		return -1;
	}
	EndBilling();
	if(ERR_OK != EndDBManager (params))
	{
		return -1;
	}
	
	return 0;
}
