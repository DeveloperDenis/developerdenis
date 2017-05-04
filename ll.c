//----------------------------------------------------
//    ll.c
// Written by Denis Levesque
//----------------------------------------------------

#include "ll.h"

#include <stdlib.h>

bool ll_addToFront(Node** list, void* data)
{
    bool added = FALSE;

    if (list && data)
    {
	//TODO(denis): HEAP_ALLOC macro?
	Node* newNode = (Node*) malloc(sizeof(Node));
	if (newNode)
	{
	    newNode->data = data;
	
	    if (!(*list))
		newNode->next = NULL;
	    else
		newNode->next = *list;

	    *list = newNode;

	    added = TRUE;
	}
    }
    return added;
}

void ll_destroy(Node* list)
{
    Node* current = list;
    
    while (current)
    {
	Node* next = current->next;

	//TODO(denis): HEAP_FREE macro?
	free(current->data);
	free(current);
	current = next;
    }
}
