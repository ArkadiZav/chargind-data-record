/***************************************************************************************
	Creation date: 30/9/2015
	Last modified date: 30/9/2015
	Description: Generic Queue- FIFO, No direct acees,
	Grows on demand beyond initial size, Cyclic: All API functions
***************************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "GData.h"
#include "ADTErr.h"
#include "queue.h"

#define QUEUE_MAGIC_NUMBER 0x6400
#define QUEUE_DESTROYED_NUMBER 0xDEAD
#define QUEUE_IS_NULL (NULL == _queue || _queue->m_magicNumber != QUEUE_MAGIC_NUMBER)
/* #define DEBUG 1 */ 

struct Queue
{
	int    m_magicNumber;
	size_t m_size;
	size_t m_head; 
	size_t m_tail; 
	size_t m_nItems;
	Item   m_vec[1];
};

Queue* QueueCreate(size_t _size)
{
	Queue* queue = NULL;

	if (0 == _size) 
	{ 
		return NULL;
	} 
	
	/* Item   m_vec[1] is already in Queue, so no need to add twice */
	queue = calloc (1, (sizeof(Queue) + (_size - 1) * sizeof(Item))); 
	 
	if (NULL == queue) 
	{
		return NULL;
	}
	
	queue->m_size	= _size;
	queue->m_head	= 0; 
	queue->m_tail	= 0;
	queue->m_nItems	= 0;
	queue->m_magicNumber = QUEUE_MAGIC_NUMBER;	

	return queue; 
}

void QueueDestroy(Queue* _queue)
{
	if (!QUEUE_IS_NULL)
	{
		_queue->m_magicNumber = QUEUE_DESTROYED_NUMBER; 
		free(_queue);
	}	
}

int QueueIsEmpty(Queue* _queue)
{
	if (QUEUE_IS_NULL || _queue->m_nItems != 0)
	{
		return false;
	}
	
	else
	{
		return true;
	} 
}

ADTErr QueueInsert(Queue* _queue, Item _item)
{
	if(QUEUE_IS_NULL)
	{
		return ERR_NOT_INITIALIZED;
	} 
	
	if(_queue->m_nItems == _queue->m_size)
	{
		return ERR_QUEUE_FULL;
	}
	
#ifdef DEBUG 
	printf("###########################\n");
	printf("tail phase 0: %zu\n", _queue->m_tail); 
#endif

	_queue->m_vec[_queue->m_tail] = _item;
	++_queue->m_tail;
	_queue->m_tail %= (_queue->m_size);
	++_queue->m_nItems;

#ifdef DEBUG 
	printf("tail: %zu\n", _queue->m_tail); 
	printf("head: %zu\n", _queue->m_head);
	printf("num items: %zu\n", _queue->m_nItems);
	printf("size: %zu\n", _queue->m_size);
	printf("###########################\n");
#endif

	return ERR_OK;
}

ADTErr QueueRemove(Queue* _queue, Item* _item)
{
	if (QUEUE_IS_NULL || NULL == _item)
	{
		return ERR_NOT_INITIALIZED;
	}
	
	if (0 == _queue->m_nItems)
	{
		return ERR_UNDERFLOW;
	}

	*_item = _queue->m_vec[_queue->m_head];	
	--_queue->m_nItems;		
	++_queue->m_head;
	 _queue->m_head %= (_queue->m_size);

	return ERR_OK;
}	

void QueuePrint(Queue* _queue, PrintFunc _doPrint)
{
	int i; 
	
	if (QUEUE_IS_NULL)
	{
		printf("The queue is not initialized\n");
		return;
	}
	
	if (_queue->m_nItems == 0) 
	{
		printf("The queue is empty\n");
		return;
	}

	if (NULL == _doPrint)
	{
		printf("No valid print function was passed\n");
		return;
	}	

	for (i = 0; i < _queue->m_nItems; i++)
	{
		printf("Item %zu: ", (_queue->m_head + i) % (_queue->m_size));
		_doPrint(_queue->m_vec[(_queue->m_head + i) % (_queue->m_size)]);
	}

	printf("\n");
}
