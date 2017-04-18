/**************************************************************************************************
	Creation date: 			11.10.15
	Last modified date: 	11.10.15
	
	Description: Header file for GENERIC HASH MAP. The ADT policy:
				 1) Container.
				 2) Insert/Remove by key.
				 3) Data saved by applying operation on values.
**************************************************************************************************/

#ifndef __GHASHMAP_H__
#define __GHASHMAP_H__

typedef struct HashMap HashMap;
typedef void*  HashKey;

typedef unsigned int (*HashFunc)(HashKey _key, size_t _size);
typedef int (*IsEqualFunc)(HashKey _key1, HashKey _key2);
typedef int (*HashDoFunc)(HashKey _key, Data _data, void* _params);
typedef int (*HashPrintFunc)(HashKey _key, Data _data);

HashMap* HashCreate(const size_t _size, const HashFunc _hashFunc, const IsEqualFunc _isEqualFunc);
void     HashDestroy(HashMap* _map);

ADTErr   HashInsert(HashMap* _map, const HashKey _key, const Data _data);
ADTErr   HashRemove(HashMap* _map, const HashKey _key, Data* _data);

size_t   HashCountItems(const HashMap* _map);
size_t   HashCountOccupiedBuckets(const HashMap* _map);

int      HashForEach(HashMap* _map, const HashDoFunc _doFunc, void* _params);

Data     HashFind(const HashMap* _map, const HashKey _key);

#ifdef _DEBUG
int      HashPrint(const HashMap* _map, const HashPrintFunc _printFunc);
#endif /*_DEBUG*/

#endif /* __GHASHMAP_H__ */
