//----------------------------------------------------
//    ll.h
// Written by Denis Levesque
//----------------------------------------------------

#ifndef LL_H_
#define LL_H_

#include "denis_types.h"

typedef struct Node_struct
{
    struct Node_struct* next;
    void* data;
} Node;

bool ll_addToFront(Node** list, void* data);
void ll_destroy(Node* list);

#endif
