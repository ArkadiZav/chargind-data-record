/**************************************************************************************************
	Creation date: 			9.11.15
	Last modified date: 	9.11.15
	
	Description: Header file for Operator functions declarations.
**************************************************************************************************/

#ifndef __OPERATOR_H__
#define __OPERATOR_H__

typedef struct Operator Operator;

Operator* 	OperatorCreate(CDR* _cdr, ADTErr* _err);

void 		OperatorDestroy(Operator* _operator);

int			OperatorIsSame(const Operator* _op1, const Operator* _op2);

ADTErr		OperatorUpdate(Operator* _destination, const Operator* _source);

ADTErr		OperatorGetName(const Operator* _operator, char* _operatorName);

void 		OperatorPrintToFile(const Operator* _operator, const int _fileDescriptor);

#ifdef _DEBUG
void 		OperatorPrint(const Operator* _operator);
#endif /* _DEBUG */

#endif /* __OPERATOR_H__ */
