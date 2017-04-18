/***************************************************************************************
	Creation date: 30/9/2015
	Last modified date: 30/9/2015
	Description: Generic Queue- FIFO, No direct acees,
	Grows on demand beyond initial size, Cyclic: Header file
***************************************************************************************/
#ifndef __QUEUE_H__
#define __QUEUE_H__

typedef struct Queue Queue;

Queue* QueueCreate 	(size_t _size);
void   QueueDestroy (Queue* _queue);
int	   QueueIsEmpty (Queue* _queue);
ADTErr QueueInsert 	(Queue* _queue, const Item _item);
ADTErr QueueRemove 	(Queue* _queue, Item* _item);
void   QueuePrint 	(Queue* _queue, PrintFunc _doPrint);

#endif /* __QUEUE_H__ */
