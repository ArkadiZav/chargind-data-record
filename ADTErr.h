/**************************************************************************************************
	Creation date: 			19.8.15
	Last modified date: 	19.8.15
	
	Description: Definitions for various errors for ADTs.
**************************************************************************************************/

#ifndef __ADTERR_H__
#define __ADTERR_H__

typedef enum
{
    /* General ADT Errors */

    ERR_OK = 0,

    ERR_GENERAL,

    ERR_NOT_INITIALIZED,

    ERR_ALLOCATION_FAILED,

    ERR_REALLOCATION_FAILED,

    ERR_UNDERFLOW,

    ERR_OVERFLOW,

    ERR_WRONG_INDEX,
    
    ERR_ILLEGAL_INPUT,
    
    /* Hash errors */
    
    ERR_ALREADY_EXISTS,
    
    ERR_NOT_FOUND,
    
    /* File errors */
    
    ERR_FILE_OPEN,
    
    ERR_FILE_CLOSE,

	/* Parser module Errors */

	ERR_PARSING_FAILED,

	ERR_SENDING2Q_FAILED,

	ERR_INSERTION2CDR_FAILED,

    /* Vector Errors */

    /* IPC Project  Errors */
	ERR_INTERNAL_DB_FAIL,
	
	ERR_QUEUE_FULL,
	
	ERR_MUTEX_DESTROY_FAILED,
	
	ERR_SEM_INIT_FAILED,
	
	ERR_SEM_UP_FAILED,
	
	ERR_SEM_DOWN_FAILED,
	
	ERR_SEM_DESTROY_FAILED,

	/* Threads  Errors */


	ERR_THREAD_CANT_CREATE,

	ERR_THREAD_CANT_JOIN,

	/* Dir Errors */

	ERR_CANT_OPEN_DIR,





} ADTErr;

#endif /* __ADTERR_H__ */
