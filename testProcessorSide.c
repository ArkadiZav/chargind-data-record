/**************************************************************************************************
	Creation date: 			10.11.15
	Last modified date: 	10.11.15
	
	Description: Unit test module for subscriber database.
**************************************************************************************************/

#include <stdio.h>
#include <stddef.h>
#include <stdbool.h>
#include <pthread.h>

#include "ADTErr.h"
#include "GData.h"
#include "safeQueue.h"
#include "cdr.h"
#include "Subscriber.h"
#include "SubscriberDB.h"
#include "Operator.h"
#include "OperatorDB.h"
#include "DataManager.h"
#include "Billing.h"

#define PRINT_STATEMENT(_statement) PrintStatement(_statement, __FUNCTION__)

static void PrintStatement(int _statement, const char* _funcName)
{
	printf("%s ", _funcName);
	printf(_statement ? "PASS!\n" : "FAIL!\n");
}

static CDR* CDR1Init(void)
{
	CDR* cdr1 = NULL;
	
	cdr1 = CDRCreate(NULL);
	CDRInsertIMSI(cdr1, "111111111");
	CDRInsertOpCode(cdr1, "Cellcom");
	CDRInsertCallType(cdr1, MOC);
	CDRInsertCallDuration(cdr1, 300);
	
	return cdr1;
}

static CDR* CDR2Init(void)
{
	CDR* cdr2 = NULL;
	
	cdr2 = CDRCreate(NULL);
	CDRInsertIMSI(cdr2, "111111111");
	CDRInsertOpCode(cdr2, "Cellcom");
	CDRInsertCallType(cdr2, SMS_MO);
	
	return cdr2;
}

static CDR* CDR3Init(void)
{
	CDR* cdr3 = NULL;
	
	cdr3 = CDRCreate(NULL);
	CDRInsertIMSI(cdr3, "111111112");
	CDRInsertOpCode(cdr3, "Orange");
	CDRInsertCallType(cdr3, GPRS);
	CDRInsertDownloadedMB(cdr3, 1.3);
	CDRInsertUploadedMB(cdr3, 0.42);
	
	return cdr3;
}

static CDR* CDREndInit(void)
{
	CDR* cdrEnd = NULL;
	
	cdrEnd = CDRCreate(NULL);
	CDRInsertIMSI(cdrEnd, "End");
	
	return cdrEnd;
}

int main()
{
	ADTErr err;
	SafeQueue* safeQ = NULL;
	DBManagerParams* manager = NULL;
	CDR* cdr1 = CDR1Init();
	CDR* cdr2 = CDR2Init();
	CDR* cdr3 = CDR3Init();
	CDR* cdrEnd = CDREndInit();
	
	safeQ = SafeQueueInit(10);
	SafeQueuePush(safeQ, cdr1);
	SafeQueuePush(safeQ, cdr2);
	SafeQueuePush(safeQ, cdr3);
	
	manager = InitDBManager(safeQ, &err);
	err = BillingInit(manager);
	EndDBManager(manager);
	EndBilling();
	DestroyDBManager(manager);
	SafeQueueDestroy(safeQ);
}
