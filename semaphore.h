/***************************************************************************************
	Description:  -- Semaphore Header file -- 
***************************************************************************************/
#ifndef __SEMAPHORE_H_
#define __SEMAPHORE_H_

#include <semaphore.h>

ADTErr SemCreate  (sem_t* _semAddr, unsigned int _initVal);
ADTErr SemUp	  (sem_t* _semAddr);
ADTErr SemDown	  (sem_t* _semAddr);
ADTErr SemDestroy (sem_t* _semAddr);

#endif /* __SEMAPHORE_H_ */
