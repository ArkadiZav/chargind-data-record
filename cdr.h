/*******************************************************************************************************
Description:		CDR module
					---  CDR Header File ---
*******************************************************************************************************/
#ifndef __CDR_H__
#define __CDR_H__

typedef enum
{
	MOC,
	MTC,
	SMS_MO,
	SMS_MT,
	GPRS,
	LAST 	/* invalid option = initialized this way */
} e_callType;

typedef struct CDR CDR;

/* Create CDR && Destroy CDR */
CDR* CDRCreate(ADTErr* _error);
void CDRDestroy(CDR* _cdr);

/* Insert Functions */
ADTErr CDRInsertIMSI(CDR* _cdr, const char* _imsi);
ADTErr CDRInsertMSISDN(CDR* _cdr, const char* _msisdn);
ADTErr CDRInsertIMEI(CDR* _cdr, const char* _imei);
ADTErr CDRInsertOpCode(CDR* _cdr, const char* _operatorCode);
ADTErr CDRInsertCallType(CDR* _cdr, const e_callType _callType);
ADTErr CDRInsertCallDuration(CDR* _cdr, const unsigned int _callDuration);
ADTErr CDRInsertDownloadedMB(CDR* _cdr, const double _downloaded);
ADTErr CDRInsertUploadedMB(CDR* _cdr, const double _uploaded);
ADTErr CDRInsertPartyMSISDN(CDR* _cdr, const char* _partyMSISDN);
ADTErr CDRInsertPartyOperator(CDR* _cdr, const char* _partyOperator);

/* GET functions */
ADTErr CDRGetIMSI(const CDR* _cdr, char* _imsi);
ADTErr CDRGetMSISDN(const CDR* _cdr, char* _msisdn);
ADTErr CDRGetIMEI(const CDR* _cdr, char* _imei);
ADTErr CDRGetOpCode(const CDR* _cdr, char* _operatorCode);
ADTErr CDRGetCallType(const CDR* _cdr, e_callType* _callType);
ADTErr CDRGetCallDuration(const CDR* _cdr, unsigned int* _callDuration);
ADTErr CDRGetDownloadedMB(const CDR* _cdr, double* _downloaded);
ADTErr CDRGetUploadedMB(const CDR* _cdr, double* _uploaded);
ADTErr CDRGetPartyMSISDN(const CDR* _cdr, char* _partyMSISDN);
ADTErr CDRGetPartyOperator(const CDR* _cdr, char* _partyOperator);

#ifdef _DEBUG

/* Print CDR */
void PrintCDR(CDR* _cdr);

#endif /* _DEBUG */

#endif /* __CDR_H__ */
