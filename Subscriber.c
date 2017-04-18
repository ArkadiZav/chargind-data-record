/**************************************************************************************************
	Creation date: 			9.11.15
	Last modified date: 	9.11.15
	
	Description: Implementation module for subscriber functions.
**************************************************************************************************/

#include <stdio.h> /* printf, snprintf*/
#include <stdlib.h> /* malloc, NULL */
#include <string.h> /* strcpy */
#include <unistd.h> /* write */

#include "ADTErr.h"
#include "cdr.h"
#include "Subscriber.h"

#define STRING_SIZE 64

struct Subscriber
{
	char 			m_imsi[STRING_SIZE];
	unsigned int 	m_incomingDuration;
	unsigned int 	m_outgoingDuration;
	unsigned int 	m_messagesReceived;
	unsigned int 	m_messagesSent;
	double			m_downloaded;
	double 			m_uploaded;
};

Subscriber* SubscriberCreate(CDR* _cdr, ADTErr* _err)
{
	Subscriber* sub = NULL;
	e_callType type;
	
	if (NULL == _cdr)
	{
		if (NULL != _err)
		{
			*_err = ERR_NOT_INITIALIZED;
		}
		return NULL;
	}
	
	sub = (Subscriber*) calloc(1, sizeof(Subscriber));
	if (NULL == sub)
	{
		if (NULL != _err)
		{
			*_err = ERR_ALLOCATION_FAILED;
		}
		return NULL;
	}
	
	CDRGetIMSI(_cdr, sub->m_imsi);
	CDRGetCallType(_cdr, &type);
	switch (type)
	{
		case MOC:
		{
			CDRGetCallDuration(_cdr, &sub->m_outgoingDuration);
			break;
		}
		case MTC:
		{
			CDRGetCallDuration(_cdr, &sub->m_incomingDuration);
			break;
		}
		case SMS_MO:
		{
			++sub->m_messagesSent;
			break;
		}
		case SMS_MT:
		{
			++sub->m_messagesReceived;
			break;
		}
		case GPRS:
		{
			CDRGetDownloadedMB(_cdr, &sub->m_downloaded);
			CDRGetUploadedMB(_cdr, &sub->m_uploaded);
			break;
		}
		default:
		{
			free(sub);
			if (NULL != _err)
			{
				*_err = ERR_ILLEGAL_INPUT;
			}
			return NULL;
		}
	}
	
	if (NULL != _err)
	{
		*_err = ERR_OK;
	}
	return sub;
}

void SubscriberDestroy(Subscriber* _sub)
{
	if (NULL == _sub)
	{
		return;
	}
	
	free(_sub);
}

ADTErr SubscriberUpdate(Subscriber* _destination, const Subscriber* _source)
{
	if (NULL == _destination || NULL == _source)
	{
		return ERR_NOT_INITIALIZED;
	}
	
	if (strcmp(_destination->m_imsi, _source->m_imsi))
	{
		return ERR_ILLEGAL_INPUT;
	}
	
	_destination->m_incomingDuration += _source->m_incomingDuration;
	_destination->m_outgoingDuration += _source->m_outgoingDuration;
	_destination->m_messagesReceived += _source->m_messagesReceived;
	_destination->m_messagesSent += _source->m_messagesSent;
	_destination->m_downloaded += _source->m_downloaded;
	_destination->m_uploaded += _source->m_uploaded;
	
	return ERR_OK;
}

int	SubscriberIsSame(const Subscriber* _sub1, const Subscriber* _sub2)
{
	if (NULL == _sub1 || NULL == _sub2)
	{
		return (_sub1 == _sub2);
	}
	
	return ( ! strcmp(_sub1->m_imsi, _sub2->m_imsi) );
}

ADTErr SubscriberGetIMSI(const Subscriber* _sub, char* _imsi)
{
	if (NULL == _sub)
	{
		return ERR_NOT_INITIALIZED;
	}
	if (NULL == _imsi)
	{
		return ERR_ILLEGAL_INPUT;
	}
	
	strcpy(_imsi, _sub->m_imsi);
	return ERR_OK;
}

void SubscriberPrintToFile(const Subscriber* _sub, const int _fileDescriptor)
{
	int nBytes;
	char buf[STRING_SIZE];
	
	if (_fileDescriptor < 0)
	{
		printf("Invalid file for output!\n");
		return;
	}
	
	if (NULL == _sub)
	{
		nBytes = snprintf(buf, STRING_SIZE, "Subscriber data unavailable!\n\n");
		write(_fileDescriptor, buf, nBytes);
		return;
	}
	
	nBytes = snprintf(buf, STRING_SIZE, "IMSI: %s\n", _sub->m_imsi);
	if (write(_fileDescriptor, buf, nBytes) < 0)
	{
		return;
	}
	nBytes = snprintf(buf, STRING_SIZE, "----------------------\n");
	if (write(_fileDescriptor, buf, nBytes) < 0)
	{
		return;
	}
	nBytes = snprintf(buf, STRING_SIZE, "Total incoming calls duration: %u\n", _sub->m_incomingDuration);
	if (write(_fileDescriptor, buf, nBytes) < 0)
	{
		return;
	}
	nBytes = snprintf(buf, STRING_SIZE, "Total outgoing calls duration: %u\n", _sub->m_outgoingDuration);
	if (write(_fileDescriptor, buf, nBytes) < 0)
	{
		return;
	}
	nBytes = snprintf(buf, STRING_SIZE, "Total messages received: %u\n", _sub->m_messagesReceived);
	if (write(_fileDescriptor, buf, nBytes) < 0)
	{
		return;
	}
	nBytes = snprintf(buf, STRING_SIZE, "Total messages sent: %u\n", _sub->m_messagesSent);
	if (write(_fileDescriptor, buf, nBytes) < 0)
	{
		return;
	}
	nBytes = snprintf(buf, STRING_SIZE, "Total downloaded data: %g [MB]\n", _sub->m_downloaded);
	if (write(_fileDescriptor, buf, nBytes) < 0)
	{
		return;
	}
	nBytes = snprintf(buf, STRING_SIZE, "Total uploaded data: %g [MB]\n\n", _sub->m_uploaded);
	if (write(_fileDescriptor, buf, nBytes) < 0)
	{
		return;
	}
}

#ifdef _DEBUG
void SubscriberPrint(const Subscriber* _sub)
{
	if (NULL == _sub)
	{
		printf("Subscriber data unavailable!\n\n");
		return;
	}
	
	printf("IMSI: %s\n", _sub->m_imsi);
	printf("----------------------\n");
	printf("Total incoming calls duration: %u\n", _sub->m_incomingDuration);
	printf("Total outgoing calls duration: %u\n", _sub->m_outgoingDuration);
	printf("Total messages received: %u\n", _sub->m_messagesReceived);
	printf("Total messages sent: %u\n", _sub->m_messagesSent);
	printf("Total downloaded data: %g [MB]\n", _sub->m_downloaded);
	printf("Total uploaded data: %g [MB]\n\n", _sub->m_uploaded);
}
#endif /* _DEBUG */
