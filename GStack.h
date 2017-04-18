/**************************************************************************************************
	Creation date: 			20.8.15
	Last modified date: 	6.10.15
	
	Description: Performs ADT policy requirements:
				 1) Comprised of a sequence of elements - LIFO.
				 2) Grants access to the top of the stack.
				 3) Grows on demand.
				 
	Update(6.10): Changed to GENERIC STACK.
**************************************************************************************************/

#ifndef __GSTACK_H__
#define __GSTACK_H__

typedef struct Stack Stack;
   
Stack* 	StackCreate();
void	StackDestroy(Stack* _stack);

ADTErr 	StackPush(Stack* _stack, Item _data);
ADTErr 	StackPop(Stack* _stack, Item* _data);

ADTErr 	StackTop(Stack* _stack, Item* _data);

/* 	
 * false - Stack is empty.
 * true	 - Stack has Datas.	
 *
 * Note: An uninitialized stack is considered empty.
 */
int 	StackIsEmpty(Stack* _stack);

/*  Unit-Test functions  */
void 	StackPrint(Stack* _stack, PrintFunc _doPrint);

#endif /* __GSTACK_H__ */
