/*******************************************************************************************************
Description:		---  CDR File ---
*******************************************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ADTErr.h"
#include "cdr.h"

#define SIZE_OF_TEXT 32

struct CDR
{
	char 			m_imsi[SIZE_OF_TEXT];
	char 			m_msisdn[SIZE_OF_TEXT];
	char 			m_imei[SIZE_OF_TEXT];
	char 			m_operatorCode[SIZE_OF_TEXT];
	e_callType 		m_callType;
	unsigned int 	m_callDuration;
	double			m_downloaded;
	double 			m_uploaded;
	char			m_partyMSISDN[SIZE_OF_TEXT];
	char 			m_partyOperator[SIZE_OF_TEXT];
};

CDR* CDRCreate(ADTErr* _error)
{
	CDR* cdr = calloc(1, sizeof(CDR));
	if (NULL == cdr)
	{
		if(NULL != _error)
		{
			*_error = ERR_ALLOCATION_FAILED;
		}
		return NULL;
	}

	cdr->m_callType = LAST;	
	if (NULL != _error)
	{
		*_error = ERR_OK;
	}

	return cdr;
}

void CDRDestroy(CDR* _cdr)
{
	if (NULL != _cdr)
	{
		free(_cdr);
	}
}

ADTErr CDRInsertIMSI(CDR* _cdr, const char* _imsi)
{
	if (NULL == _cdr || NULL == _imsi)
	{
		return ERR_NOT_INITIALIZED;
	}

	strcpy(_cdr->m_imsi, _imsi); 

	return ERR_OK;
}

ADTErr CDRInsertMSISDN(CDR* _cdr, const char* _msisdn)
{
	if (NULL == _cdr || NULL == _msisdn)
	{
		return ERR_NOT_INITIALIZED;
	}

	strcpy(_cdr->m_msisdn, _msisdn); 

	return ERR_OK;
}

ADTErr CDRInsertIMEI(CDR* _cdr, const char* _imei)
{
	if (NULL == _cdr || NULL == _imei)
	{
		return ERR_NOT_INITIALIZED;
	}

	strcpy(_cdr->m_imei, _imei); 

	return ERR_OK;
}

ADTErr CDRInsertOpCode(CDR* _cdr, const char* _operatorCode)
{
	if (NULL == _cdr || NULL == _operatorCode)
	{
		return ERR_NOT_INITIALIZED;
	}

	strcpy(_cdr->m_operatorCode, _operatorCode); 

	return ERR_OK;
}

ADTErr CDRInsertCallType(CDR* _cdr, const e_callType _callType)
{
	if (NULL == _cdr || LAST == _callType)
	{
		return ERR_NOT_INITIALIZED;
	}

	_cdr->m_callType = _callType;
	
	return ERR_OK;
}

ADTErr CDRInsertCallDuration(CDR* _cdr, const unsigned int _callDuration)
{
	if (NULL == _cdr)
	{
		return ERR_NOT_INITIALIZED;
	}

	_cdr->m_callDuration = _callDuration; 
	
	return ERR_OK;
}

ADTErr CDRInsertDownloadedMB(CDR* _cdr, const double _downloaded)
{
	if (NULL == _cdr)
	{
		return ERR_NOT_INITIALIZED;
	}

	_cdr->m_downloaded = _downloaded; 
	
	return ERR_OK;
}

ADTErr CDRInsertUploadedMB(CDR* _cdr, const double _uploaded)
{
	if (NULL == _cdr)
	{
		return ERR_NOT_INITIALIZED;
	}

	_cdr->m_uploaded = _uploaded; 
	
	return ERR_OK;
}

ADTErr CDRInsertPartyMSISDN(CDR* _cdr, const char* _partyMSISDN)
{
	if (NULL == _cdr || NULL == _partyMSISDN)
	{
		return ERR_NOT_INITIALIZED;
	}

	strcpy(_cdr->m_partyMSISDN, _partyMSISDN); 

	return ERR_OK;
}

ADTErr CDRInsertPartyOperator(CDR* _cdr, const char* _partyOperator)
{
	if (NULL == _cdr || NULL == _partyOperator)
	{
		return ERR_NOT_INITIALIZED;
	}

	strcpy(_cdr->m_partyOperator, _partyOperator); 

	return ERR_OK;
}

/******** Get functions ********/
ADTErr CDRGetIMSI(const CDR* _cdr, char* _imsi)
{
	if (NULL == _cdr || NULL == _imsi)
	{
		return ERR_NOT_INITIALIZED;
	}

	strcpy(_imsi, _cdr->m_imsi); 

	return ERR_OK;
}

ADTErr CDRGetMSISDN(const CDR* _cdr, char* _msisdn)
{
	if (NULL == _cdr || NULL == _msisdn)
	{
		return ERR_NOT_INITIALIZED;
	}

	strcpy(_msisdn, _cdr->m_msisdn); 

	return ERR_OK;
}


ADTErr CDRGetIMEI(const CDR* _cdr, char* _imei)
{
	if (NULL == _cdr || NULL == _imei)
	{
		return ERR_NOT_INITIALIZED;
	}

	strcpy(_imei, _cdr->m_imei); 

	return ERR_OK;
}

ADTErr CDRGetOpCode(const CDR* _cdr, char* _operatorCode)
{
	if (NULL == _cdr || NULL == _operatorCode)
	{
		return ERR_NOT_INITIALIZED;
	}

	strcpy(_operatorCode, _cdr->m_operatorCode); 

	return ERR_OK;
}

ADTErr CDRGetCallType(const CDR* _cdr, e_callType* _callType)
{
	if (NULL == _cdr || NULL == _callType)
	{
		return ERR_NOT_INITIALIZED;
	}

	*_callType = _cdr->m_callType; 

	return ERR_OK;
}

ADTErr CDRGetCallDuration(const CDR* _cdr, unsigned int* _callDuration)
{
	if (NULL == _cdr || NULL == _callDuration)
	{
		return ERR_NOT_INITIALIZED;
	}

	*_callDuration = _cdr->m_callDuration; 	

	return ERR_OK;
}

ADTErr CDRGetDownloadedMB(const CDR* _cdr, double* _downloaded)
{
	if (NULL == _cdr || NULL == _downloaded)
	{
		return ERR_NOT_INITIALIZED;
	}

	*_downloaded = _cdr->m_downloaded; 	

	return ERR_OK;
}

ADTErr CDRGetUploadedMB(const CDR* _cdr, double* _uploaded)
{
	if (NULL == _cdr || NULL == _uploaded)
	{
		return ERR_NOT_INITIALIZED;
	}

	*_uploaded = _cdr->m_uploaded; 	

	return ERR_OK;
}

ADTErr CDRGetPartyMSISDN(const CDR* _cdr, char* _partyMSISDN)
{
	if (NULL == _cdr || NULL == _partyMSISDN)
	{
		return ERR_NOT_INITIALIZED;
	}

	strcpy(_partyMSISDN, _cdr->m_partyMSISDN); 

	return ERR_OK;	
}

ADTErr CDRGetPartyOperator(const CDR* _cdr, char* _partyOperator)
{
	if (NULL == _cdr || NULL == _partyOperator)
	{
		return ERR_NOT_INITIALIZED;
	}

	strcpy(_partyOperator, _cdr->m_partyOperator); 

	return ERR_OK;
}

#ifdef _DEBUG
void PrintCDR(CDR* _cdr)
{
	if (NULL == _cdr)
	{
		printf("Nothing to print");
		return;
	}

	printf("%s\n", _cdr->m_imsi);
	printf("%s\n", _cdr->m_msisdn);
	printf("%s\n", _cdr->m_imei);
	printf("%s\n", _cdr->m_operatorCode);
	printf("%d\n", _cdr->m_callType);
	printf("%u\n", _cdr->m_callDuration);
	printf("%g\n", _cdr->m_downloaded);
	printf("%g\n", _cdr->m_uploaded);
	printf("%s\n", _cdr->m_partyMSISDN);
	printf("%s\n\n", _cdr->m_partyOperator);
}
#endif /* _DEBUG */
