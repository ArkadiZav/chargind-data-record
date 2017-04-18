/**************************************************************************************************************************************
Creation Date : 9.11.2015
Last modified date 9.11.2015
Description : Files - open all files from directory and send CDR lines to Q
								-- header file --
**************************************************************************************************************************************/
#ifndef __FILESREADER_H__
#define __FILESREADER_H__

ADTErr InitReaders(SafeQueue* _queue);

ADTErr EndReaders(SafeQueue* _queue);

#endif /* __FILESREADER_H__ */
