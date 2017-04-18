/**************************************************************************************************
	Creation date: 			11.10.15
	Last modified date: 	11.10.15
	
	Description: Implementation of functions for GENERIC HASH MAP.
**************************************************************************************************/

#include <stdio.h> /* HashPrint */
#include <stdlib.h> /* malloc/NULL */
#include <stdbool.h> /* true/false */

#include "ADTErr.h"
#include "GData.h"
#include "GLList.h"
#include "GHashMap.h"

#define MAGIC (void*) 0xDeadFFFF
#define IS_ILLEGAL_HASH ( (NULL == _map) || (MAGIC != _map->m_magic) )

typedef List* Bucket;
typedef struct Pair Pair;
typedef struct KeyCmpr KeyCmpr;
typedef struct PairPrint PairPrint;
typedef struct ParamAndFunc ParamAndFunc;

struct HashMap
{
	Bucket* 	m_buckets;
	size_t 		m_hashSize;
	size_t		m_noItems;
	size_t		m_noBuckets;
	HashFunc 	m_hashFunc;
	IsEqualFunc m_isEqualFunc;
	void* 		m_magic;
};

struct Pair
{
	HashKey m_key;
	Data	m_value;
};

struct KeyCmpr
{
	IsEqualFunc m_userCmpFunc;
	HashKey		m_key;
};

struct PairPrint
{
	HashPrintFunc m_doPrint;
};

struct ParamAndFunc
{
	void* 		m_param;
	HashDoFunc 	m_func;
};

static unsigned int HashIndex(HashMap* _map, const HashKey _key)
{
	unsigned int index = _map->m_hashFunc(_key, _map->m_hashSize);
	index = (index < 0) ? -1 * index : index;
	return index % _map->m_hashSize;
}

/* Allocate pair data */
static Pair* CreatePair(const HashKey _key, const Data _value)
{
	Pair* pair = (Pair*) malloc(sizeof(Pair));
	if (NULL == pair)
	{
		return NULL;
	}
	pair->m_key = _key;
	pair->m_value = _value;
	return pair;
}

/* Free pair data (Utility for "ListForEach") */
static int FreePair(Data _pair, void* _dummy)
{
	if (NULL == _pair)
	{
		return false;
	}
	
	free(((Pair*)_pair)->m_key);
	free(_pair);
	return true;
}

/* Send Hash comparator function to list */
static int KeyCompare(Data _listData, Data _toFind)
{
	KeyCmpr* keyAndFunc = (KeyCmpr*) _toFind;
	
	if (NULL == _listData)
	{
		return false;
	}
	
	/*return keyAndFunc->m_userCmpFunc( (HashKey) &((Pair*)_listData)->m_key, (HashKey) &keyAndFunc->m_key);*/
	return keyAndFunc->m_userCmpFunc( (HashKey) ((Pair*)_listData)->m_key, keyAndFunc->m_key);
}

/* Send Hash do function to list */
static int HashToList(Data _listData, void* _paramAndFunc)
{
	ParamAndFunc* input = (ParamAndFunc*) _paramAndFunc;
	return input->m_func(((Pair*)_listData)->m_key, ((Pair*)_listData)->m_value, input->m_param);
}

/***********************************/
/********** API Functions **********/
/***********************************/

HashMap* HashCreate(const size_t _size, const HashFunc _hashFunc, const IsEqualFunc _isEqualFunc)
{
	HashMap* hash = NULL;
	int i, j;
	
	if ( (! _size) || (NULL == _hashFunc) || (NULL == _isEqualFunc) )
	{
		return NULL;
	}
	
	hash = (HashMap*) malloc(sizeof(HashMap));
	if (NULL == hash)
	{
		return NULL;
	}
	
	hash->m_buckets = (Bucket*) calloc(_size, sizeof(Bucket));
	if (NULL == hash->m_buckets)
	{
		free(hash);
		return NULL;
	}
	
	for (i = 0; i < _size; ++i)
	{
		hash->m_buckets[i] = ListCreate();
		if (NULL == hash->m_buckets[i])
		{
			for (j = i - 1; j >= 0; --j)
			{
				ListDestroy(hash->m_buckets[j]);
			}
			free(hash->m_buckets);
			free(hash);
			return NULL;
		}
	}
	
	hash->m_hashSize = _size;
	hash->m_noBuckets = 0;
	hash->m_noItems = 0;
	hash->m_hashFunc = _hashFunc;
	hash->m_isEqualFunc = _isEqualFunc;
	hash->m_magic = MAGIC;
	return hash;
}

void HashDestroy(HashMap* _map)
{
	int i = 0;
	
	if (IS_ILLEGAL_HASH)
	{
		return;
	}
	
	for (i = 0; i < _map->m_hashSize; ++i)
	{
		ListForEach(ListBegin(_map->m_buckets[i]), ListEnd(_map->m_buckets[i]), FreePair, NULL);
		ListDestroy(_map->m_buckets[i]);
	}
	_map->m_magic = NULL;
	free(_map->m_buckets);
	free(_map);
}

ADTErr HashInsert(HashMap* _map, const HashKey _key, const Data _data)
{
	ListItr iter;
	ListItr end;
	unsigned int index = 0;
	KeyCmpr toSend;
	Pair* pair = NULL;
	
	if (IS_ILLEGAL_HASH)
	{
		return ERR_NOT_INITIALIZED;
	}
	if (NULL == _key)
	{
		return ERR_ILLEGAL_INPUT;
	}
	
	index = HashIndex(_map, _key);	
	end = ListEnd(_map->m_buckets[index]);
	
	/* Search if key already exists */
	toSend.m_userCmpFunc = _map->m_isEqualFunc;
	toSend.m_key = _key;
	iter = ListFindFirst( ListBegin(_map->m_buckets[index]), end, KeyCompare, &toSend );
	if (! ListIsSame(iter, end))
	{
		return ERR_ALREADY_EXISTS;
	}
	
	pair = CreatePair(_key, _data);
	if (NULL == pair)
	{
		return ERR_ALLOCATION_FAILED;
	}
	
	iter = ListPushTail(_map->m_buckets[index], pair);
	if (ListIsSame(iter, end))
	{
		return ERR_ALLOCATION_FAILED;
	}
	
	if ( 1 == ListCountItems(_map->m_buckets[index]) )
	{
		++_map->m_noBuckets;
	}
	
	++_map->m_noItems;
	return ERR_OK;
}

ADTErr HashRemove(HashMap* _map, const HashKey _key, Data* _data)
{
	ListItr iter;
	ListItr end;
	unsigned int index = 0;
	KeyCmpr toSend;
	Pair* output = NULL;
	
	if (IS_ILLEGAL_HASH)
	{
		return ERR_NOT_INITIALIZED;
	}
	if ( (NULL == _key) || (NULL == _data) )
	{
		return ERR_ILLEGAL_INPUT;
	}
	
	index = HashIndex(_map, _key);
	end = ListEnd(_map->m_buckets[index]);
	toSend.m_userCmpFunc = _map->m_isEqualFunc;
	toSend.m_key = _key;
	iter = ListFindFirst( ListBegin(_map->m_buckets[index]), end, KeyCompare, &toSend );
	
	if (ListIsSame(iter, end))
	{
		*_data = NULL;
		return ERR_OK;
	}
	
	output = (Pair*) ListRemove(iter);
	*_data = output->m_value;
	free(output->m_key);
	free(output);
	if (ListIsEmpty(_map->m_buckets[index]))
	{
		--_map->m_noBuckets;
	}
	--_map->m_noItems;
	return ERR_OK;
}

size_t HashCountItems(const HashMap* _map)
{
	if (IS_ILLEGAL_HASH)
	{
		return 0;
	}
	
	return _map->m_noItems;
}

size_t HashCountOccupiedBuckets(const HashMap* _map)
{
	if (IS_ILLEGAL_HASH)
	{
		return 0;
	}
	
	return _map->m_noBuckets;
}

int HashForEach(HashMap* _map, const HashDoFunc _doFunc, void* _params)
{
	ListItr end;
	ListItr stop;
	int i = 0;
	ParamAndFunc toSend;
	
	if (IS_ILLEGAL_HASH || NULL == _doFunc)
	{
		return false;
	}
	
	toSend.m_param = _params;
	toSend.m_func = _doFunc;
	for (i = 0; i < _map->m_hashSize; ++i)
	{
		end = ListEnd(_map->m_buckets[i]);
		stop = ListForEach(ListBegin(_map->m_buckets[i]), end, HashToList, (void*) &toSend);
		if (! ListIsSame(stop, end))
		{
			return false;
		}
	}
	return true;
}

Data HashFind(const HashMap* _map, const HashKey _key)
{
	ListItr iter;
	ListItr end;
	unsigned int index = 0;
	KeyCmpr toSend;
	
	if (IS_ILLEGAL_HASH || (NULL == _key) )
	{
		return NULL;
	}
	
	index = HashIndex((HashMap*)_map, _key);
	end = ListEnd(_map->m_buckets[index]);
	toSend.m_userCmpFunc = _map->m_isEqualFunc;
	toSend.m_key = _key;
	iter = ListFindFirst( ListBegin(_map->m_buckets[index]), end, KeyCompare, &toSend );
	
	return (ListIsSame(iter, end)) ? NULL : ((Pair*)ListGetData(iter))->m_value;
}

#ifdef _DEBUG

/* Send Hash print function to list */
static int PrintPair(Data _listData, void* _param)
{
	Pair* pair = (Pair*)_listData;
	return ((PairPrint*)_param)->m_doPrint(pair->m_key, pair->m_value);
}

int HashPrint(const HashMap* _map, const HashPrintFunc _printFunc)
{
	int i = 0;
	int count = 0;
	PairPrint doPrint;
	
	if (IS_ILLEGAL_HASH || NULL == _printFunc)
	{
		return 0;
	}
	
	doPrint.m_doPrint = _printFunc;
	for ( i = 0; i < _map->m_hashSize; ++i)
	{
		if ( ! ListIsEmpty(_map->m_buckets[i]) )
		{
			printf("Data in bucket #%d:\n", i);
			ListForEach(ListBegin(_map->m_buckets[i]), ListEnd(_map->m_buckets[i]), PrintPair, (void*) &doPrint);
			count += ListCountItems(_map->m_buckets[i]);
			/*printf("\n");*/
		}
	}
	return count;
}
#endif /*_DEBUG*/
