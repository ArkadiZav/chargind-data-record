/***************************************************************************************
	Description:  -- Semaphore file -- 
***************************************************************************************/
#include <unistd.h>
#include <stdio.h>
#include <pthread.h>  
#include <semaphore.h> /* posix semaphores */
#include "ADTErr.h"
#include "semaphore.h" /* my own header of semaphores */

#define SHRD_BTWN_THRDS 0
#define SHRD_BTWN_PRCSS 1

/* TODO: maybe separate SemCreate to 2 functions: 1 for processes, 1 for threads */
ADTErr SemCreate(sem_t* _semAddr, unsigned int _initVal)
{
	if (-1 == sem_init(_semAddr, SHRD_BTWN_THRDS, _initVal))
	{
		perror("Error in sem_init");
		return ERR_SEM_INIT_FAILED;
	}

	return ERR_OK;
}

ADTErr SemUp(sem_t* _semAddr)
{
	if (-1 == sem_post(_semAddr))
	{
		perror("Error in sem_post");
		return ERR_SEM_UP_FAILED;
	}
	return ERR_OK;
}

ADTErr SemDown(sem_t* _semAddr)
{
	if (-1 == sem_wait(_semAddr))
	{
		perror("Error in sem_wait");
		return ERR_SEM_DOWN_FAILED;
	}
	return ERR_OK;
}

ADTErr SemDestroy(sem_t* _semAddr)
{
	if (-1 == sem_destroy(_semAddr))
	{
		perror("Error in sem_destroy");
		return ERR_SEM_DESTROY_FAILED;
	}
	return ERR_OK;
}
