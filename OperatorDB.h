/**************************************************************************************************
	Creation date: 			9.11.15
	Last modified date: 	9.11.15
	
	Description: Header file for OperatorDB functions implementations.
**************************************************************************************************/

#ifndef __OPERATORDB_H__
#define __OPERATORDB_H__

typedef struct OperatorDB OperatorDB;

OperatorDB* OperatorDBCreate(ADTErr* _err);

/* Note: frees all operators still stored */
void 		OperatorDBDestroy(OperatorDB* _odb);

ADTErr 		OperatorDBInsert(OperatorDB* _odb, const Operator* _op);

ADTErr 		OperatorDBGet(const OperatorDB* _odb, const char* _operatorName , Operator** _op);

ADTErr 		OperatorDBRemove(OperatorDB* _odb, const char* _operatorName, Operator** _op);

ADTErr 		OperatorDBPrintToFile(const OperatorDB* _odb, const char* _fileName);

#ifdef _DEBUG
void		OperatorDBPrint(const OperatorDB* _odb);
#endif /* _DEBUG */

#endif /* __OPERATORDB_H__ */
