/***************************************************************************************
	Description: Producer<->Consumer Project with Our own made queue 
				       	   -- Safe Queue file -- 
***************************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/shm.h>

#include "ADTErr.h"
#include "GData.h"
#include "queue.h"
#include "semaphore.h"
#include "safeQueue.h"

#define SUCCESS 0

struct SafeQueue
{
	pthread_mutex_t m_mutex;
	sem_t m_prodSem;
	sem_t m_consSem;
	Queue* m_queue;
};

SafeQueue* SafeQueueInit(size_t _size)
{
	SafeQueue* safeQ = NULL;

	/* creating safe queue */
	safeQ = malloc (sizeof(SafeQueue));
	if (NULL == safeQ)
	{
		return NULL;
	}
	safeQ->m_queue = QueueCreate(_size);
	if (NULL == safeQ->m_queue)
	{
		return NULL;
	}
	
	/* initializing its semahores and mutex */
	pthread_mutex_init(&safeQ->m_mutex, NULL);
	/* initially an empty queue, so full ability exists to insert all messages up to size */
	if (ERR_OK != SemCreate(&safeQ->m_prodSem, _size))
	{
		return NULL;
	}
	/* initially an empty queue, so consumer can't yet consume */
	if (ERR_OK != SemCreate(&safeQ->m_consSem, 0))
	{
		return NULL;
	}
	
	return safeQ;	
}

/* after pushing, producer can produce -1 items, and consumer can consume +1 items
in comparison to before */ 
ADTErr SafeQueuePush(SafeQueue* _queue, void* _data)
{
	ADTErr error;
	
	if (NULL == _queue)
	{
		return  ERR_NOT_INITIALIZED;
	}	
	
	error = SemDown(&_queue->m_prodSem);
	if (error != ERR_OK)
	{
		return error;
	}

	pthread_mutex_lock(&_queue->m_mutex); 
	error = QueueInsert(_queue->m_queue, _data);
	if (error != ERR_OK)
	{
		return error;
	}
	pthread_mutex_unlock(&_queue->m_mutex);

	error = SemUp(&_queue->m_consSem);
	if (error != ERR_OK)
	{
		return error;
	}

	return ERR_OK;
}

/* after popping, producer can produce +1 items, and consumer can consume -1 items
in comparison to before */ 
ADTErr SafeQueuePop(SafeQueue* _queue, void** _dataPtr)
{
	ADTErr error;
		
	if (NULL == _queue)
	{
		return  ERR_NOT_INITIALIZED;
	}	
	
	error = SemDown(&_queue->m_consSem);
	if (error != ERR_OK)
	{
		return error;
	}

	pthread_mutex_lock(&_queue->m_mutex); 
	error = QueueRemove(_queue->m_queue, _dataPtr);
	if (error != ERR_OK)
	{
		return error;
	}
	pthread_mutex_unlock(&_queue->m_mutex);

	error = SemUp(&_queue->m_prodSem);
	if (error != ERR_OK)
	{
		return error;
	}

	return ERR_OK;
}

int SafeQueueIsEmpty(SafeQueue* _queue)
{
	if(NULL == _queue)
	{
		return 0;
	}	
	
	pthread_mutex_lock(&_queue->m_mutex); 
	return QueueIsEmpty(_queue->m_queue);
	pthread_mutex_unlock(&_queue->m_mutex);
}

ADTErr SafeQueueDestroy(SafeQueue* _queue)
{
	ADTErr error;
	if(NULL == _queue)
	{
		return ERR_NOT_INITIALIZED;
	}	
	
	/* destroying semaphores */
	error = SemDestroy(&_queue->m_prodSem);
	if (error != ERR_OK)
	{
		return error;
	}
	error = SemDestroy(&_queue->m_consSem);
	if (error != ERR_OK)
	{
		return error;
	}
	if (SUCCESS != pthread_mutex_destroy(&_queue->m_mutex))
	{
		return ERR_MUTEX_DESTROY_FAILED; 
	}

	/* destroying queue */
	QueueDestroy(_queue->m_queue);
	free(_queue);
	
	return ERR_OK;
}
