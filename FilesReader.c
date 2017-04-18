/**************************************************************************************************************************************
Creation Date : 9.11.2015
Last modified date 9.11.2015
Description :  Files - open all files from directory and send CDR lines to Q 
**************************************************************************************************************************************/
#include <stdlib.h>
#include <dirent.h> /* to open directory */
#include <string.h> /* for strlen */
#include <stdio.h>
#include <fcntl.h> /*TODO check if needed */
#include <pthread.h>

#include "ADTErr.h"
#include "GData.h"
#include "safeQueue.h"
#include "GStack.h"
#include "cdr.h"
#include "parser.h"
#include "FilesReader.h"

#define CDR_LINE_SIZE 128
#define PATH_SIZE 64
#define NUM_OF_THREADS 10

static pthread_t s_threads[NUM_OF_THREADS];
static Stack* s_stack;
static pthread_mutex_t errFileMutex = PTHREAD_MUTEX_INITIALIZER;

static Stack* CreateStackFllFileNames(char* _directoryName,ADTErr* _err);
static void* FileReader(void* _queue);
static ADTErr DestroyFileNamesStack(void);

ADTErr InitReaders(SafeQueue* _queue)
{
	ADTErr err;
	int i;	

	if(NULL == _queue)
	{
		pthread_mutex_destroy(&errFileMutex);
		return ERR_NOT_INITIALIZED;
	}
	s_stack = CreateStackFllFileNames("Storage",&err);
	if(NULL == s_stack)
	{
		pthread_mutex_destroy(&errFileMutex);
		return err;
	}
	for(i = 0; i < NUM_OF_THREADS; i++)
	{
		if(pthread_create(&s_threads[i], NULL,FileReader,(void*)_queue) != 0)
		{
			DestroyFileNamesStack();
			pthread_mutex_destroy(&errFileMutex);
			return ERR_THREAD_CANT_CREATE;
		}
	}
	return ERR_OK;
}

ADTErr EndReaders(SafeQueue* _queue)
{
	int i;
	ADTErr err;

	if(NULL == _queue)
	{
		return ERR_NOT_INITIALIZED;
	}
	for(i = 0; i < NUM_OF_THREADS; i++)
	{
		if(pthread_join(s_threads[i], NULL) != 0)
		{
			return ERR_THREAD_CANT_JOIN;
		}
	}
	pthread_mutex_destroy(&errFileMutex);
	DestroyFileNamesStack();
	if(ERR_OK != (err = SendEndMsg2Queue(_queue)))
	{
		return err;
	}
	return ERR_OK;
}

static Stack* CreateStackFllFileNames(char* _directoryName,ADTErr* _err)
{
	DIR* directoryP = NULL;
	struct dirent* dirData = NULL;
	Stack* stackLocal = NULL;
	int strSize = 0;
	char* path = NULL;

	if(NULL == _directoryName )
	{
		return NULL;
	}
	if(NULL == (stackLocal = StackCreate()))
	{
		if(NULL != _err)
		{
			*_err = ERR_ALLOCATION_FAILED;
		}
		return NULL;
	}
	if(NULL == (directoryP = opendir(_directoryName)))
	{
		StackDestroy(stackLocal);
		if(NULL != _err)
		{
			*_err = ERR_CANT_OPEN_DIR;
		}
		return NULL;
	}
	while((dirData = readdir(directoryP)) != NULL)
	{
		strSize = strlen(dirData->d_name);
		/* ignore tmp files */
		if(dirData->d_name[0] != '.' && dirData->d_name[strSize -1] != '~')
		{
			path = malloc(PATH_SIZE);
			sprintf(path,"./%s/",_directoryName);
			if(NULL == path)
			{
				if(NULL != _err)
				{	
					*_err = ERR_ALLOCATION_FAILED;
				}
				closedir(directoryP);
				StackDestroy(stackLocal);
				return NULL;
			} 
			strcat(path,dirData->d_name);
			if((StackPush(stackLocal, path)) != ERR_OK)
			{
				StackDestroy(stackLocal);
				closedir(directoryP);
				return NULL;
			}
		}
	}
	closedir(directoryP);
	if(NULL != _err)
	{
		*_err = ERR_OK;
	}
	return stackLocal;
}

static ADTErr DestroyFileNamesStack(void)
{	
	char* filePath = NULL;

	if(NULL == s_stack)
	{
		return ERR_NOT_INITIALIZED;
	}
	while(ERR_OK == StackPop(s_stack,(void**)&filePath))
	{
		free(filePath);
	}
	StackDestroy(s_stack);
	return ERR_OK;
}

static void* FileReader(void* _queue)
{
	char* filePath = NULL;
	SafeQueue* queue = NULL;
	char cdrLine[CDR_LINE_SIZE];
	CDR* cdr;
	FILE* fp = NULL;
	FILE* fpFileErr = NULL;

	if(NULL == _queue)
	{
		return NULL;
	}
	queue = ((SafeQueue*)_queue);
	while(ERR_OK == StackPop(s_stack,(void**)&filePath))
	{
		if((fp = fopen(filePath,"r")) == NULL)
		{
			return NULL;
		}
		free(filePath);
		while(fgets(cdrLine,CDR_LINE_SIZE,fp))
		{	
			if(ERR_OK != Parse(cdrLine,&cdr))
			{
				pthread_mutex_lock(&errFileMutex);
				fpFileErr = fopen("ErrorLinesFile","a");
				fprintf(fpFileErr,"ERR LINE - %s",cdrLine);
				fclose(fpFileErr);
				pthread_mutex_unlock(&errFileMutex);
				continue;
			}
			SendCDR2Queue(cdr,queue);
		}
		fclose(fp);
	}
	pthread_exit(NULL);
}
