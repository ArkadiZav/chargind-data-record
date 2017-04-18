/**************************************************************************************************
	Creation date: 			6.10.15
	Last modified date: 	6.10.15
	
	Description: Header file for GENERIC LINKED LIST. The ADT policy:
				 1) Container.
				 2) Insert/Remove by iterator.
				 3) Grows on demand.
**************************************************************************************************/

#ifndef __GLLIST_H__
#define __GLLIST_H__

typedef void* ListItr;
typedef struct List List;

/* ************************************ */
/* ********** List Functions ********** */
/* ************************************ */

List* 	ListCreate	();
void	ListDestroy	( List* _list );

/*
 * Erroneous list returns NULL iterator.
 * If Insertion fails (memory allocation error), returns "End" iterator.
 */
ListItr	ListPushHead( List* _list, Data _data );
ListItr	ListPushTail( List* _list, Data _data );

/*
 * Erroneous list returns NULL Data.
 */
Data	ListPopHead	( List* _list );
Data	ListPopTail	( List* _list );

/*
 * Erroneous list is defined empty and with 0 items.
 */
size_t	ListCountItems( List* _list );
int 	ListIsEmpty	  ( List* _list );

/* ******************************************** */
/* ********** ListIterator Functions ********** */
/* ******************************************** */

/*
 * Erroneous list returns NULL Data.
 */
ListItr ListBegin	( List* _list );
ListItr ListEnd		( List* _list );

/*
 * Erroneous iterator returns NULL iterator.
 * Next: Returns NULL if receives "End" iterator.
 * Prev: Returns NULL if receives "Begin" iterator.
 */
ListItr ListNext	( ListItr _it );
ListItr ListPrev	( ListItr _it );

/*
 * Erroneous list returns NULL data.
 */
Data	ListGetData	( ListItr _it );
/* 
 * Compare iterators
 */
int 	ListIsSame  ( ListItr _it1, ListItr _it2 ); 

/*
 * ListInsert [before] and return ListItr to new.
 * If insertion fails, returns input iterator.
 * If remove fails, returns NULL data.
 */
ListItr ListInsert	( ListItr _it, Data _data );
Data	ListRemove	( ListItr _it );

/* 
 * Return ListItr to the first Data in list
 * for which _cmp(list_data, argument_data) returns "true".
 * 
 * If _from is located after _to, the function reverses order.
 * If data is not found or input argument is invalid, returns NULL iterator.
 */
ListItr ListFindFirst( ListItr _from, ListItr _to, DataComp _cmp, Data _data ); 

/*
 * Perform specified function _do(Data, Param) for each Data in the
 * collection (Not including _to) until error is returned from the _do function.
 * returns iterator to last visited node.
 *
 * If _from is located after _to, the function reverses order.
 * If input argument is invalid, returns NULL iterator.
 */
ListItr	ListForEach	 ( ListItr _from, ListItr _to, DoFunc _do, void* _param );

/*
 * Perform specified function _doPrint(Data) for each Data in the list.
 */
void ListPrint		 ( List* _list, PrintFunc _doPrint );

#endif /* __GLLIST_H__ */
