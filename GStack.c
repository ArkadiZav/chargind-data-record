/**************************************************************************************************
	Creation date: 			20.8.15
	Last modified date: 	16.9.15
	
	Description: Performs ADT policy requirements:
				 1) Comprised of a sequence of elements - LIFO.
				 2) Grants access to the top of the stack.
				 3) Grows on demand.
**************************************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>

#include "ADTErr.h"
#include "GData.h"
#include "GLList.h"
#include "GStack.h"

#define MAGIC (void*) 0xDeadBeef

struct Stack
{
	List* 	m_list; /* Pointer to list */
	void* 	m_magic;
	pthread_mutex_t m_mutex;
};

Stack* StackCreate()
{
	Stack* stack = NULL;
	
	stack = (Stack*) malloc(sizeof(Stack));
	if (NULL == stack)
	{
		return NULL;
	}
	
	stack->m_list = ListCreate();
	if (NULL == stack->m_list)
	{
		free(stack);
		return NULL;
	}
	if(-1 == pthread_mutex_init(&(stack->m_mutex),NULL))
	{
		ListDestroy(stack->m_list);
		free(stack);
		return NULL;
	}
	stack->m_magic = MAGIC;
	return stack; 
}

void StackDestroy(Stack* _stack)
{
	if (NULL == _stack || MAGIC != _stack->m_magic)
	{
		return;
	}
	pthread_mutex_destroy(&(_stack->m_mutex));
	ListDestroy(_stack->m_list);
	_stack->m_magic = NULL;
	free(_stack);
}

ADTErr StackPush(Stack* _stack, Item _data)
{
	ListItr iter;
	
	if (NULL == _stack || MAGIC != _stack->m_magic)
	{
		return ERR_NOT_INITIALIZED;
	}
	
	iter = ListPushTail(_stack->m_list, _data);
	if (ListIsSame(iter, ListEnd(_stack->m_list)))
	{
		return ERR_ALLOCATION_FAILED;
	}
	
	return ERR_OK;
}

ADTErr StackPop(Stack* _stack, Item* _data)
{
	
	if (NULL == _stack || MAGIC != _stack->m_magic)
	{
		return ERR_NOT_INITIALIZED;
	}
	
	if (NULL == _data)
	{
		return ERR_GENERAL;
	}
	pthread_mutex_lock(&(_stack->m_mutex)); /* LOCK */
	if (ListIsEmpty(_stack->m_list))
	{
		pthread_mutex_unlock(&(_stack->m_mutex));
		return ERR_UNDERFLOW;
	}
	
	*_data = ListPopTail(_stack->m_list);
	pthread_mutex_unlock(&(_stack->m_mutex)); /* UNLOCK */
	return ERR_OK;
}

ADTErr StackTop(Stack* _stack, Item* _data)
{
	ListItr iter;
		
	if (NULL == _stack || MAGIC != _stack->m_magic)
	{
		return ERR_NOT_INITIALIZED;
	}
	
	if (NULL == _data)
	{
		return ERR_GENERAL;
	}
	
	if (ListIsEmpty(_stack->m_list))
	{
		return ERR_UNDERFLOW;
	}
	
	iter = ListEnd(_stack->m_list);
	iter = ListPrev(iter);
	*_data = ListGetData(iter);
	
	return ERR_OK;
}

int StackIsEmpty(Stack* _stack)
{
	if (NULL == _stack || MAGIC != _stack->m_magic)
	{
		return true;
	}
	
	return ListIsEmpty(_stack->m_list);
}

void StackPrint(Stack* _stack, PrintFunc _doPrint)
{
	if (NULL == _stack || MAGIC != _stack->m_magic)
	{
		return;
	}
	
	ListPrint(_stack->m_list, _doPrint);
}
