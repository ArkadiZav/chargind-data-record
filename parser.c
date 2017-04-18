/*******************************************************************************************************
Description:		Gets A CDR string and converts it to a struct that will be inserted 
					to a queue.
					---  Parser Implementation File ---
*******************************************************************************************************/
#include <stdio.h>
#include <string.h>

#include "ADTErr.h"
#include "GData.h"   
#include "safeQueue.h" 
#include "cdr.h"
#include "parser.h"

#define RETURN_PARSING_ERROR_AND_DESTROY_CDR_IF_FAILED if (errorStatus != ERR_OK)      \
													   {						       \
															CDRDestroy(*_cdr);       	   \
															return ERR_PARSING_FAILED; \
													   }
													   
static int Convert2ChosenCallType(const char* _callTypeStr);
													   
static int Convert2ChosenCallType(const char* _callTypeStr)
{
	int callType; 

	if (strcmp(_callTypeStr, "MOC") == 0)
	{
		callType = MOC;
	}
	else if (strcmp(_callTypeStr, "MTC") == 0)
	{
		callType = MTC;
	}
	else if (strcmp(_callTypeStr, "SMS_MO") == 0)
	{
		callType = SMS_MO;
	}
	else if (strcmp(_callTypeStr, "SMS_MT") == 0)
	{
		callType = SMS_MT;
	}
	else if (strcmp(_callTypeStr, "GPRS") == 0)
	{
		callType = GPRS;
	}
	else
	{
		callType = LAST; /* invalid option */
	}
	
		return callType;	
}

/* Get CDR string, convert to CDR struct */
ADTErr Parse(char* _cdrString, CDR** _cdr)
{
	char* savePtr = NULL; /* for strtok_r function, which is thread safe */
	ADTErr errorStatus;
	char* cdrDataStr = NULL;
	int callType;
	unsigned int callDuration;
	double downloaded;
	double uploaded;

	if (NULL == _cdrString || NULL == _cdr)
	{	
		return ERR_NOT_INITIALIZED;
	}

	*_cdr = CDRCreate(&errorStatus);
	if (errorStatus != ERR_OK)
	{
		return ERR_ALLOCATION_FAILED;
	}

	/* No NULL checks for strtoks because of assumption that files are all OK */
	/* Field: IMSI */ 
	cdrDataStr = strtok_r(_cdrString, "|", &savePtr);
	errorStatus = CDRInsertIMSI(*_cdr, cdrDataStr);
	RETURN_PARSING_ERROR_AND_DESTROY_CDR_IF_FAILED;
	/* Field: MSISDN */
	cdrDataStr = strtok_r(NULL, "|", &savePtr);
	errorStatus = CDRInsertMSISDN(*_cdr, cdrDataStr);
	RETURN_PARSING_ERROR_AND_DESTROY_CDR_IF_FAILED;
	/* Field: IMEI */
	cdrDataStr = strtok_r(NULL, "|", &savePtr);
	errorStatus = CDRInsertIMEI(*_cdr, cdrDataStr);
	RETURN_PARSING_ERROR_AND_DESTROY_CDR_IF_FAILED;
	/* Field: operator code */
	cdrDataStr = strtok_r(NULL, "|", &savePtr);
	errorStatus = CDRInsertOpCode(*_cdr, cdrDataStr);
	RETURN_PARSING_ERROR_AND_DESTROY_CDR_IF_FAILED;
	/* Field: call type */
	cdrDataStr = strtok_r(NULL, "|", &savePtr);
	callType = Convert2ChosenCallType(cdrDataStr);
	errorStatus = CDRInsertCallType(*_cdr, callType);
	RETURN_PARSING_ERROR_AND_DESTROY_CDR_IF_FAILED;

	/* Field: call date: do nothing */
	cdrDataStr = strtok_r(NULL, "|", &savePtr);
	/* Field: call time: do nothing */
	cdrDataStr = strtok_r(NULL, "|", &savePtr);
	
	/* Field: call duration */
	cdrDataStr = strtok_r(NULL, "|", &savePtr);
	if(sscanf(cdrDataStr, "%u", &callDuration) != 1) 
	{ 
		CDRDestroy(*_cdr);
		return ERR_PARSING_FAILED;
    }
	errorStatus = CDRInsertCallDuration(*_cdr, callDuration);
	RETURN_PARSING_ERROR_AND_DESTROY_CDR_IF_FAILED;
	/* Field: downloaded MB */
	cdrDataStr = strtok_r(NULL, "|", &savePtr);
	if(sscanf(cdrDataStr, "%lf", &downloaded) != 1) 
	{ 
		CDRDestroy(*_cdr);
		return ERR_PARSING_FAILED;
    }
	errorStatus = CDRInsertDownloadedMB(*_cdr, downloaded);
	RETURN_PARSING_ERROR_AND_DESTROY_CDR_IF_FAILED;
	/* Field: uploaded MB */
	cdrDataStr = strtok_r(NULL, "|", &savePtr);
	if(sscanf(cdrDataStr, "%lf", &uploaded) != 1) 
	{ 
		CDRDestroy(*_cdr);
		return ERR_PARSING_FAILED;
    }
	errorStatus = CDRInsertUploadedMB(*_cdr, uploaded);
	RETURN_PARSING_ERROR_AND_DESTROY_CDR_IF_FAILED;
	/* Field: party MSISDN */
	cdrDataStr = strtok_r(NULL, "|", &savePtr);
	errorStatus = CDRInsertPartyMSISDN(*_cdr, cdrDataStr);
	RETURN_PARSING_ERROR_AND_DESTROY_CDR_IF_FAILED;
	/* Field: party operator */
	cdrDataStr = strtok_r(NULL, "|", &savePtr);
	errorStatus = CDRInsertPartyOperator(*_cdr, cdrDataStr);
	RETURN_PARSING_ERROR_AND_DESTROY_CDR_IF_FAILED;	

	return ERR_OK;
}

/* insert CDR to queue */
ADTErr SendCDR2Queue(CDR* _cdr, SafeQueue* _queue)
{
	ADTErr errorStatus;

	if (NULL == _cdr || NULL == _queue)
	{
		return ERR_NOT_INITIALIZED;
	}

	errorStatus = SafeQueuePush(_queue, _cdr);
	if (errorStatus != ERR_OK)
	{
		CDRDestroy(_cdr);
		return ERR_SENDING2Q_FAILED;
	}

	return ERR_OK;
}

/* send end message to queue */
ADTErr SendEndMsg2Queue(SafeQueue* _queue)
{
	CDR* cdr = NULL; 
	ADTErr errorStatus;

	if (NULL == _queue)
	{
		return ERR_NOT_INITIALIZED;
	}

	cdr = CDRCreate(&errorStatus);
	if (errorStatus != ERR_OK)
	{
		return ERR_ALLOCATION_FAILED;
	}

	errorStatus = CDRInsertIMSI(cdr, "END");
	if (errorStatus != ERR_OK)
	{
		CDRDestroy(cdr);
		return ERR_INSERTION2CDR_FAILED;
	}
		
	errorStatus = SafeQueuePush(_queue, cdr);
	if (errorStatus != ERR_OK)
	{
		CDRDestroy(cdr);
		return ERR_SENDING2Q_FAILED;
	}

	return ERR_OK;		
}

