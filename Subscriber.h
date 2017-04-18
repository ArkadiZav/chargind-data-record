/**************************************************************************************************
	Creation date: 			9.11.15
	Last modified date: 	9.11.15
	
	Description: Header file for Subscriber functions declarations.
**************************************************************************************************/

#ifndef __SUBSCRIBER_H__
#define __SUBSCRIBER_H__

typedef struct Subscriber Subscriber;

Subscriber* SubscriberCreate(CDR* _cdr, ADTErr* _err);

void		SubscriberDestroy(Subscriber* _sub);

ADTErr		SubscriberUpdate(Subscriber* _destination, const Subscriber* _source);

int			SubscriberIsSame(const Subscriber* _sub1, const Subscriber* _sub2);

ADTErr		SubscriberGetIMSI(const Subscriber* _sub, char* _imsi);

void 		SubscriberPrintToFile(const Subscriber* _sub, const int _fileDescriptor);

#ifdef _DEBUG
void 		SubscriberPrint(const Subscriber* _sub);
#endif /* _DEBUG */

#endif /* __SUBSCRIBER_H__ */
