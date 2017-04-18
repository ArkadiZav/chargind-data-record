/*****************************************************************************************************************************************
Creation Date : 10.11.2015
Last modified date 10.11.2015
Description : General Data
*****************************************************************************************************************************************/
#ifndef __GDATA_H__
#define __GDATA_H__

typedef void* Item;
typedef void* Data;
typedef void (*PrintFunc)(Item);
typedef int (*DoFunc)(Item, void*);
typedef int (*DataComp)(Item, Item);

#endif /* __GDATA_H__ */
