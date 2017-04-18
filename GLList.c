/**************************************************************************************************
	Creation date: 			6.10.15
	Last modified date: 	6.10.15
	
	Description: Implementation file for GENERIC LIST.
**************************************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "GData.h"
#include "GLList.h"

#define LIST_OK (void*) 0xDEADBEEF

#define IS_ILLEGAL_ITER(_iterator) 	IsIllegalIterator(_iterator)
#define IS_ILLEGAL_LIST		 		IsIllegalList(_list)
#define HEAD						(_list->m_head)
#define TAIL						(_list->m_tail)

typedef struct Node Node;

struct Node
{
    Data  m_data;
    Node* m_next;
    Node* m_prev;
};

struct List
{
    Node m_head;
    Node m_tail;
};

/* Utility function declarations */
static int 			IsIllegalIterator(ListItr _it);
static int 			IsIllegalList(List* _list);
static void 		FreeList(List* _list);
static Node* 		InsertNode(Node* _before, Data _data);
static Data 		RemoveNode(Node* _delete);
static ListItr 		DoFromTo(DoFunc _do, void* _param, Node* _from, Node* _to);
static ListItr 		CompareFromTo(DataComp _cmp, Data _data, Node* _from, Node* _to);

static int IsIllegalIterator(ListItr _it)
{
	return (NULL == _it);
}

static int IsIllegalList(List* _list)
{
	return ( (NULL == _list) || (LIST_OK != HEAD.m_data) );
}

/* Frees empty list */
static void FreeList(List* _list)
{
	HEAD.m_data = NULL;
	TAIL.m_data = NULL;
	free(_list);
}

/* Insert before "_after" node */
static Node* InsertNode(Node* _after, Data _data)
{
	Node* node;
	
	node = (Node*) malloc(sizeof(Node));
	if (NULL == node)
	{
		return NULL;
	}
	
	node->m_data = _data;
	node->m_next = _after;
	node->m_prev = _after->m_prev;
	_after->m_prev->m_next = node;
	_after->m_prev = node;
	
	return node;
}

/* Remove current node */
static Data RemoveNode(Node* _delete)
{
	Data data = NULL;
	
	data = _delete->m_data;
	_delete->m_next->m_prev = _delete->m_prev;
	_delete->m_prev->m_next = _delete->m_next;
	free(_delete);
	
	return data;
}

static ListItr DoFromTo(DoFunc _do, void* _param, Node* _from, Node* _to)
{
	Node* temp = NULL;
	
	for (temp = _from; temp != _to; temp = temp->m_next)
	{
		if (! _do(temp->m_data, _param))
		{
			break;
		}
	}
	return (ListItr) temp;
}

static ListItr CompareFromTo(DataComp _cmp, Data _data, Node* _from, Node* _to)
{
	Node* temp = NULL;
	
	for (temp = _from; temp != _to; temp = temp->m_next)
	{
		if (_cmp(temp->m_data, _data))
		{
			break;
		}
	}
	return (ListItr) temp;
}

/* *********************************************** */
/* **************** API FUNCTIONS **************** */
/* *********************************************** */

/* ************************************ */
/* ********** List Functions ********** */
/* ************************************ */

List* ListCreate(void)
{
	List* list = NULL;
	
	list = (List*) malloc(sizeof(List));
	if (NULL == list)
	{
		return NULL;
	}
	
	list->m_head.m_next = &list->m_tail;
	list->m_head.m_prev = NULL;
	list->m_head.m_data = LIST_OK;
	
	list->m_tail.m_prev = &list->m_head;
	list->m_tail.m_next = NULL;
	list->m_tail.m_data = LIST_OK;
	
	return list;
}

void ListDestroy( List* _list )
{
	Node* temp = NULL;
	
	if (IS_ILLEGAL_LIST)
	{
		return;
	}
	
	/* If empty list */
	if (HEAD.m_next == &TAIL)
	{
		FreeList(_list);
		return;
	}
	
	temp = HEAD.m_next;
	while (temp != &TAIL)
	{
		temp = temp->m_next;
		free(temp->m_prev);
	}
	FreeList(_list);
}

ListItr	ListPushHead(List* _list, Data _data)
{
	Node* node = NULL;
	
	if ( IS_ILLEGAL_LIST )
	{
		return NULL;
	}
	
	node = InsertNode(HEAD.m_next, _data);
	return (NULL == node) ? (ListItr) &TAIL : (ListItr) node;
}

ListItr	ListPushTail(List* _list, Data _data)
{
	Node* node = NULL;
	
	if ( IS_ILLEGAL_LIST )
	{
		return NULL;
	}
	
	node = InsertNode(&TAIL, _data);
	return (NULL == node) ? (ListItr) &TAIL : (ListItr) node;
}

Data ListPopHead (List* _list)
{
	if ( IS_ILLEGAL_LIST )
	{
		return NULL;
	}
	
	return RemoveNode(HEAD.m_next);
}

Data ListPopTail (List* _list)
{
	if ( IS_ILLEGAL_LIST )
	{
		return NULL;
	}
	
	return RemoveNode(TAIL.m_prev);
}

size_t ListCountItems( List* _list )
{
	Node* temp = NULL;
	size_t count = 0;
	
	if (IS_ILLEGAL_LIST)
	{
		return 0;
	}
	
	temp = HEAD.m_next;
	for (count = 0; &TAIL != temp; ++count)
	{
		temp = temp->m_next;
	}
	
	return count;
}

int	ListIsEmpty( List* _list )
{
	return (IS_ILLEGAL_LIST || HEAD.m_next == &TAIL);
}

ListItr	ListForEach	(ListItr _from, ListItr _to, DoFunc _do, void* _param)
{
	Node* temp = NULL;
	Node* from = (Node*) _from;
	Node* to = (Node*) _to;
	
	if (IS_ILLEGAL_ITER(_from) || IS_ILLEGAL_ITER(_to) || NULL == _do)
	{
		return NULL;
	}
	
	if (from == to && NULL == from->m_next)
	{
		return (ListItr) from;
	}
	
	/* If NULL is reached (Past "End"): Order is reversed! */
	for (temp = from; temp != to && temp != NULL; temp = temp->m_next);
	
	return (NULL == temp) ? DoFromTo(_do, _param, to, from) : DoFromTo(_do, _param, from, to);
}

ListItr ListFindFirst(ListItr _from, ListItr _to, DataComp _cmp, Data _data)
{
	Node* temp = NULL;
	Node* from = (Node*) _from;
	Node* to = (Node*) _to;
	
	if (IS_ILLEGAL_ITER(_from) || IS_ILLEGAL_ITER(_to) || NULL == _cmp)
	{
		return NULL;
	}
	
	if (from == to && NULL == from->m_next)
	{
		return (ListItr) from;
	}
	
	/* If NULL is reached (Past "End"): Order is reversed! */
	for (temp = from; temp != to && temp != NULL; temp = temp->m_next);
	
	return (NULL == temp) ? CompareFromTo(_cmp, _data, to, from) : CompareFromTo(_cmp, _data, from, to);
}

void ListPrint( List* _list, PrintFunc _doPrint )
{
	Node* temp = NULL;
	int i = 0;
	
	if (IS_ILLEGAL_LIST || NULL == _doPrint)
	{
		printf("Failed to print list!\n");
		return;
	}
	
	temp = HEAD.m_next;
	if (temp == &TAIL)
	{
		printf("List is empty!\n");
		return;
	}
	
	for (i = 1; temp != &TAIL; ++i)
	{
		printf("Data #%d: ", i);
		_doPrint(temp->m_data);
		temp = temp->m_next;
	}
	printf("\n");
}

/* ******************************************** */
/* ********** ListItr Functions *************** */
/* ******************************************** */

ListItr ListBegin (List* _list)
{
	return (IS_ILLEGAL_LIST) ? NULL : (ListItr) HEAD.m_next;
}

ListItr ListEnd	(List* _list)
{
	return (IS_ILLEGAL_LIST) ? NULL : (ListItr) &TAIL;
}

ListItr ListNext (ListItr _it)
{
	Node* node = (Node*) _it;
	
	return (IS_ILLEGAL_ITER(_it)) ? NULL : (ListItr) node->m_next;
}

ListItr ListPrev (ListItr _it)
{
	Node* node = (Node*) _it;
	
	return ( IS_ILLEGAL_ITER(_it) || NULL == node->m_prev->m_prev) ? NULL : (ListItr) node->m_prev;
}

Data ListGetData (ListItr _it)
{
	Node* node = (Node*) _it;
	
	return (IS_ILLEGAL_ITER(_it)) ? NULL : node->m_data;
}

int ListIsSame (ListItr _it1, ListItr _it2)
{
	return (_it1 == _it2);
}

ListItr ListInsert (ListItr _it, Data _data)
{
	Node* node = NULL;
	
	if (IS_ILLEGAL_ITER(_it))
	{
		return _it;
	}
	
	node = InsertNode((Node*) _it, _data);
	return (NULL == node) ? _it : (ListItr) node;
}

Data ListRemove (ListItr _it)
{
	Node* node = (Node*) _it;
	
	if (IS_ILLEGAL_ITER(_it))
	{
		return NULL;
	}
	
	return RemoveNode(node);
}
