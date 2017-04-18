/**************************************************************************************************
	Creation date: 			9.11.15
	Last modified date: 	9.11.15
	
	Description: Implementation module for OperatorDB functions.
**************************************************************************************************/

#include <stdio.h> /* printf, snprintf */
#include <stdlib.h> /* malloc */
#include <stdbool.h> /* true/false */
#include <unistd.h> /* close */
#include <fcntl.h> /* open */
#include <string.h> /* strcmp */

#include "ADTErr.h"
#include "GData.h"
#include "GHashMap.h"
#include "cdr.h"
#include "Operator.h"
#include "OperatorDB.h"

#define NUM_OF_BUCKETS 1000
#define OPERATOR_NAME_SIZE 32

struct OperatorDB
{
	HashMap* m_map;
};

/* Hash function */
/*static unsigned int qhashmurmur3_32(HashKey _data, size_t _nbytes)
{
    const unsigned int c1 = 0xcc9e2d51;
    const unsigned int c2 = 0x1b873593;

    int nblocks;
    const unsigned int* blocks;
    const unsigned int* tail;

    unsigned int h = 0;

    int i;
    unsigned int k;
    
    if (_data == NULL || _nbytes == 0)
    {
        return 0;
	}
	
	nblocks = _nbytes / 4;
	blocks = (const unsigned int *) (_data);
	tail = ((const unsigned int *)_data + (nblocks * 4));
    
    for (i = 0; i < nblocks; i++)
    {
        k = blocks[i];

        k *= c1;
        k = (k << 15) | (k >> (32 - 15));
        k *= c2;

        h ^= k;
        h = (h << 13) | (h >> (32 - 13));
        h = (h * 5) + 0xe6546b64;
    }

    k = 0;
    switch (_nbytes & 3)
    {
        case 3:
        {
            k ^= tail[2] << 16;
            break;
        }
        case 2:
       	{
            k ^= tail[1] << 8;
            break;
        }
        case 1:
        {
            k ^= tail[0];
            k *= c1;
            k = (k << 15) | (k >> (32 - 15));
            k *= c2;
            h ^= k;
            break;
        }
    }

    h ^= _nbytes;

    h ^= h >> 16;
    h *= 0x85ebca6b;
    h ^= h >> 13;
    h *= 0xc2b2ae35;
    h ^= h >> 16;

    return h;
}*/

static unsigned int DJB2(HashKey _imsi, size_t _ignore)
{
	unsigned int hash = 0;
	int c;
	char* str = (char*)_imsi;
	
	while ((c = *str++))
	{
		hash = ((hash << 5) + hash) + c;
	}
	
	return hash;
}

static int FreeOperators(HashKey _opName, Data _operator, void* _ignore)
{
	if (NULL == _operator)
	{
		return false;
	}
	
	OperatorDestroy((Operator*)_operator);
	return true;
}

#ifdef _DEBUG
static int PrintOperators(HashKey _ignore, Data _data)
{
	OperatorPrint((Operator*)_data);
	return true;
}
#endif /* _DEBUG */

static int CompareOperators(HashKey _opName1, HashKey _opName2)
{
	return ( ! strcmp((char*)_opName1, (char*)_opName2) );
}

OperatorDB* OperatorDBCreate(ADTErr* _err)
{
	OperatorDB* odb = malloc(sizeof(OperatorDB));
	if (NULL == odb)
	{
		if (NULL != _err)
		{
			*_err = ERR_ALLOCATION_FAILED;
		}
		return NULL;
	}
	
	odb->m_map = HashCreate(NUM_OF_BUCKETS, DJB2, CompareOperators);
	if (NULL == odb->m_map)
	{
		if (NULL != _err)
		{
			*_err = ERR_ALLOCATION_FAILED;
		}
		free(odb);
		return NULL;
	}
	
	if (NULL != _err)
	{
		*_err = ERR_OK;
	}
	return odb;
}

void OperatorDBDestroy(OperatorDB* _odb)
{
	if (NULL == _odb)
	{
		return;
	}
	
	HashForEach(_odb->m_map, FreeOperators, NULL);
	HashDestroy(_odb->m_map);
	free(_odb);
}

ADTErr OperatorDBInsert(OperatorDB* _odb, const Operator* _op)
{
	ADTErr err;
	char* operatorName = NULL;
	
	if (NULL == _odb)
	{
		return ERR_NOT_INITIALIZED;
	}
	if (NULL == _op)
	{
		return ERR_ILLEGAL_INPUT;
	}
	
	operatorName = (char*) malloc(OPERATOR_NAME_SIZE * sizeof(char));
	if (NULL == operatorName)
	{
		return ERR_ALLOCATION_FAILED;
	}
	
	OperatorGetName(_op, operatorName);
	err = HashInsert(_odb->m_map, (const HashKey)operatorName, (Data)_op);
	return err;
}

ADTErr OperatorDBGet(const OperatorDB* _odb, const char* _operatorName , Operator** _op)
{
	if (NULL == _odb)
	{
		return ERR_NOT_INITIALIZED;
	}
	if (NULL == _operatorName || NULL == _op)
	{
		return ERR_ILLEGAL_INPUT;
	}
	
	*_op = HashFind(_odb->m_map, (const HashKey)_operatorName);
	return (NULL == *_op) ? ERR_NOT_FOUND : ERR_OK;
}

ADTErr OperatorDBRemove(OperatorDB* _odb, const char* _operatorName, Operator** _op)
{
	if (NULL == _odb)
	{
		return ERR_NOT_INITIALIZED;
	}
	if (NULL == _operatorName || NULL == _op)
	{
		return ERR_ILLEGAL_INPUT;
	}
	
	HashRemove(_odb->m_map, (const HashKey)_operatorName, (Data*)_op);
	return (NULL == *_op) ? ERR_NOT_FOUND : ERR_OK;
}

static int PrintAllToFile(HashKey _key, Data _operator, void* _fileDesc)
{
	OperatorPrintToFile((Operator*)_operator, *(int*)_fileDesc);
	return true;
}

ADTErr OperatorDBPrintToFile(const OperatorDB* _odb, const char* _fileName)
{
	int fileDesc;
	
	if (NULL == _odb)
	{
		return ERR_NOT_INITIALIZED;
	}
	if (NULL == _fileName)
	{
		return ERR_ILLEGAL_INPUT;
	}
	
	fileDesc = open(_fileName, O_RDWR | O_APPEND | O_CREAT, S_IRUSR | S_IWUSR);
	if (fileDesc < 0)
	{
		return ERR_FILE_OPEN;
	}
	
	if ( ! HashForEach(_odb->m_map, PrintAllToFile, (void*)&fileDesc) )
	{
		return ERR_GENERAL;
	}
	
	if (-1 == close(fileDesc))
	{
		return ERR_FILE_CLOSE;
	}
	
	return ERR_OK;
}

#ifdef _DEBUG
void OperatorDBPrint(const OperatorDB* _odb)
{
	if (NULL == _odb)
	{
		printf("Operator DB is not initialized!\n");
		return;
	}
	
	HashPrint(_odb->m_map, PrintOperators);
}
#endif /* _DEBUG */
