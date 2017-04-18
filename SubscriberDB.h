/**************************************************************************************************
	Creation date: 			9.11.15
	Last modified date: 	9.11.15
	
	Description: Header file for Subscriber Database functions declarations.
**************************************************************************************************/

#ifndef __SUBSCRIBERDB_H__
#define __SUBSCRIBERDB_H__

typedef struct SubscriberDB SubscriberDB;

SubscriberDB* 	SubscriberDBCreate(ADTErr* _err); 

/* Note: frees all subscribers still stored */
void 			SubscriberDBDestroy(SubscriberDB* _sdb);

ADTErr 			SubscriberDBInsert(SubscriberDB* _sdb, const Subscriber* _sub);

ADTErr 			SubscriberDBGet(const SubscriberDB* _sdb, const char* _imsi , Subscriber** _sub);

ADTErr 			SubscriberDBRemove(SubscriberDB* _sdb, const char* _imsi, Subscriber** _sub);

ADTErr 			SubscriberDBPrintToFile(const SubscriberDB* _sdb, const char* _fileName);

#ifdef _DEBUG
void			SubscriberDBPrint(const SubscriberDB* _sdb);
#endif /* _DEBUG */

#endif /* __SUBSCRIBERDB_H__ */
