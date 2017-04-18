/*******************************************************************************************************
Description:		Gets A CDR string and converts it to a struct that will be inserted 
					to a data structure.
					---  Parser Header File ---
*******************************************************************************************************/
#ifndef __PARSER_H__
#define __PARSER_H__

ADTErr Parse(char* _cdrString, CDR** _cdr);
ADTErr SendCDR2Queue(CDR* _cdr, SafeQueue* _queue);
ADTErr SendEndMsg2Queue(SafeQueue* _queue);

#endif /* __PARSER_H__ */
