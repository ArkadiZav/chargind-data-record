/***************************************************************************************
	Description: Producer<->Consumer Project with Our own made queue 
				       	   -- Header of Safe Queue file -- 
***************************************************************************************/
#ifndef __SAFE_QUEUE_H__
#define __SAFE_QUEUE_H__

typedef struct SafeQueue SafeQueue;

SafeQueue* SafeQueueInit (size_t _size);
ADTErr SafeQueuePush	 (SafeQueue* _queue, void* _data);
ADTErr SafeQueuePop		 (SafeQueue* _queue, void** _dataPtr);
int SafeQueueIsEmpty	 (SafeQueue* _queue);
ADTErr SafeQueueDestroy  (SafeQueue* _queue);

#endif /* __SAFE_QUEUE_H__ */
