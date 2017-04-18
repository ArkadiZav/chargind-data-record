/**************************************************************************************************
	Creation date: 			9.11.15
	Last modified date: 	9.11.15
	
	Description: Implementation module for Operator functions.
**************************************************************************************************/

#include <stdio.h> /* printf, snprintf*/
#include <stdlib.h> /* malloc, NULL */
#include <string.h> /* strcpy */
#include <unistd.h> /* write */

#include "ADTErr.h"
#include "cdr.h"
#include "Operator.h"

#define STRING_SIZE 64

struct Operator
{
	char 			m_operatorName[STRING_SIZE];
	unsigned int 	m_incomingDuration;
	unsigned int 	m_outgoingDuration;
	unsigned int 	m_messagesReceived;
	unsigned int 	m_messagesSent;
	double			m_downloaded;
	double 			m_uploaded;
};

Operator* OperatorCreate(CDR* _cdr, ADTErr* _err)
{
	Operator* operator = NULL;
	e_callType type;
	
	if (NULL == _cdr)
	{
		if (NULL != _err)
		{
			*_err = ERR_NOT_INITIALIZED;
		}
		return NULL;
	}
	
	operator = (Operator*) calloc(1, sizeof(Operator));
	if (NULL == operator)
	{
		if (NULL != _err)
		{
			*_err = ERR_ALLOCATION_FAILED;
		}
		return NULL;
	}
	
	CDRGetOpCode(_cdr, operator->m_operatorName);
	CDRGetCallType(_cdr, &type);
	switch (type)
	{
		case MOC:
		{
			CDRGetCallDuration(_cdr, &operator->m_outgoingDuration);
			break;
		}
		case MTC:
		{
			CDRGetCallDuration(_cdr, &operator->m_incomingDuration);
			break;
		}
		case SMS_MO:
		{
			++operator->m_messagesSent;
			break;
		}
		case SMS_MT:
		{
			++operator->m_messagesReceived;
			break;
		}
		case GPRS:
		{
			CDRGetDownloadedMB(_cdr, &operator->m_downloaded);
			CDRGetUploadedMB(_cdr, &operator->m_uploaded);
			break;
		}
		default:
		{
			free(operator);
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
	return operator;
}

void OperatorDestroy(Operator* _operator)
{
	if (NULL == _operator)
	{
		return;
	}
	
	free(_operator);
}

int	OperatorIsSame(const Operator* _op1, const Operator* _op2)
{
	if (NULL == _op1 || NULL == _op2)
	{
		return (_op1 == _op2);
	}
	
	return ( ! strcmp(_op1->m_operatorName, _op2->m_operatorName) );
}

ADTErr OperatorUpdate(Operator* _destination, const Operator* _source)
{
	if (NULL == _destination || NULL == _source)
	{
		return ERR_NOT_INITIALIZED;
	}
	
	if ( strcmp(_destination->m_operatorName, _source->m_operatorName) )
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

ADTErr OperatorGetName(const Operator* _operator, char* _operatorName)
{
	if (NULL == _operator)
	{
		return ERR_NOT_INITIALIZED;
	}
	if (NULL == _operatorName)
	{
		return ERR_ILLEGAL_INPUT;
	}
	
	strcpy(_operatorName, _operator->m_operatorName);
	return ERR_OK;
}

void OperatorPrintToFile(const Operator* _operator, const int _fileDescriptor)
{
	int nBytes;
	char buf[STRING_SIZE];
	
	if (_fileDescriptor < 0)
	{
		printf("Invalid file for output!\n");
		return;
	}
	
	if (NULL == _operator)
	{
		nBytes = snprintf(buf, STRING_SIZE, "Operator data unavailable!\n\n");
		write(_fileDescriptor, buf, nBytes);
		return;
	}
	
	nBytes = snprintf(buf, STRING_SIZE, "Operator: %s\n", _operator->m_operatorName);
	if (write(_fileDescriptor, buf, nBytes) < 0)
	{
		return;
	}
	nBytes = snprintf(buf, STRING_SIZE, "----------------------\n");
	if (write(_fileDescriptor, buf, nBytes) < 0)
	{
		return;
	}
	nBytes = snprintf(buf, STRING_SIZE, "Total incoming calls duration: %u\n", _operator->m_incomingDuration);
	if (write(_fileDescriptor, buf, nBytes) < 0)
	{
		return;
	}
	nBytes = snprintf(buf, STRING_SIZE, "Total outgoing calls duration: %u\n", _operator->m_outgoingDuration);
	if (write(_fileDescriptor, buf, nBytes) < 0)
	{
		return;
	}
	nBytes = snprintf(buf, STRING_SIZE, "Total messages received: %u\n", _operator->m_messagesReceived);
	if (write(_fileDescriptor, buf, nBytes) < 0)
	{
		return;
	}
	nBytes = snprintf(buf, STRING_SIZE, "Total messages sent: %u\n", _operator->m_messagesSent);
	if (write(_fileDescriptor, buf, nBytes) < 0)
	{
		return;
	}
	nBytes = snprintf(buf, STRING_SIZE, "Total downloaded data: %g [MB]\n", _operator->m_downloaded);
	if (write(_fileDescriptor, buf, nBytes) < 0)
	{
		return;
	}
	nBytes = snprintf(buf, STRING_SIZE, "Total uploaded data: %g [MB]\n\n", _operator->m_uploaded);
	if (write(_fileDescriptor, buf, nBytes) < 0)
	{
		return;
	}
}

#ifdef _DEBUG
void OperatorPrint(const Operator* _operator)
{
	if (NULL == _operator)
	{
		printf("Operator data unavailable!\n");
		return;
	}
	
	printf("Operator: %s\n", _operator->m_operatorName);
	printf("----------------------\n");
	printf("Total incoming calls duration: %u\n", _operator->m_incomingDuration);
	printf("Total outgoing calls duration: %u\n", _operator->m_outgoingDuration);
	printf("Total messages received: %u\n", _operator->m_messagesReceived);
	printf("Total messages sent: %u\n", _operator->m_messagesSent);
	printf("Total downloaded data: %g [MB]\n", _operator->m_downloaded);
	printf("Total uploaded data: %g [MB]\n\n", _operator->m_uploaded);
}
#endif /* _DEBUG */
