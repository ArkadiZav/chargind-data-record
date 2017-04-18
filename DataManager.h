/*************************************************************************
Creation Date: 3.11.15
Last edit: 3.11.15
Description: 
**************************************************************************/

#ifndef __DATAMNGR_H__
#define __DATAMNGR_H__

typedef struct DBManagerParams DBManagerParams;

DBManagerParams* InitDBManager (SafeQueue* _safeQ, ADTErr* _error);
ADTErr EndDBManager (DBManagerParams* _params);
ADTErr DestroyDBManager (DBManagerParams* _params);

ADTErr GetSubscriberDB (DBManagerParams* _params, SubscriberDB** _subDB);
ADTErr GetOperatorDB (DBManagerParams* _params, OperatorDB** _oprDB);
ADTErr GetDBMutex (DBManagerParams* _params, pthread_mutex_t* _DBMutex);

#endif /*__DATAMNGR_H__*/
