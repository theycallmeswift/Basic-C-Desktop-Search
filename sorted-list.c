/*
 * File: sorted-list.c
 * Author: Mike Swift
 * Modified: Feb 15th, 2011
 */

#include <stdio.h>
#include <stdlib.h>
#include "sorted-list.h"

/*
 * The SLNode_ struct is a basic node object of a Linked List
 * It holds a pointer to the data (void *) and a pointer to the next Node_
 */

struct SLNode_ {
	SLItemT data;
	struct SLNode_ *next;
};

typedef struct SLNode_* SLNode;

/*
 * The SList_ struct holds a pointer to the head of a sorted list, a
 * pointer to the comparison function, and an int to store version information.
 */

struct SList_ {
	SLNode head;
	CompareFuncT func;
	int version;
};

typedef struct SList_* SList;

/*
 * The SortedListIterT_ struct holds a pointer to the SortedList, current node of a sorted list,
 * and an int to store version information.
 */

struct Iterator_ {
	SList list;
	SLNode current;
	int version;
};

typedef struct Iterator_* Iterator;

SortedListT SLCreate(CompareFuncT cf) {
	SList sl;

	sl = (SList) malloc( sizeof( struct SList_ ) );

	if( sl == NULL )
	{
		printf("ERROR: Malloc failed. \n");
		return NULL;
	}

	sl->head = NULL;

	if( cf == NULL )
	{
		printf("ERROR: Must point to a valid callback function.\n");
		return NULL;
	}

	sl->func = cf;

	sl->version = 0;

	return (SortedListT) sl;
}

void SLDestroy(SortedListT list)
{
	SLNode curr, next;
	SList sl;

	if(list == NULL)
	{
		printf("Passed in NULL list. \n");
		exit(0);
	}

	sl = (SList) list;

	curr = sl->head;

	while(curr != NULL)
	{
		next = curr->next;
		free(curr);
		curr = next;
	}

	free(sl);
}

int SLInsert(SortedListT list, SLItemT newObj) {
	SList sl;
	SLNode temp, curr, prev;
	int comp;

	if(list == NULL)
	{
		printf("Passed in NULL list. \n");
		return 0;
	}

	sl = (SList) list;

	temp = (SLNode) malloc( sizeof( struct SLNode_ ) );

	if( temp == NULL )
	{
		printf("ERROR: Malloc failed. \n");
		return 0;
	}

	temp->data = newObj;
	temp->next = NULL;

	curr = sl->head;
	prev = NULL;

	if(curr == NULL)
	{
		sl->version++;
		sl->head = temp;
		return 1;
	}
	else
	{
		while(curr != NULL)
		{
			comp = sl->func(curr->data, temp->data);
			if( comp <= 0 )
			{
				if( comp == 0 )
				{
				    free(temp);
				    return 0;
				}
				
				if( prev == NULL )
				{
					temp->next = curr;
					sl->head = temp;
				}
				else
				{
					temp->next = prev->next;
					prev->next = temp;
				}
				sl->version++;
				return 1;
			}
			prev = curr;
			curr = curr->next;
		}
		sl->version++;
		prev->next = temp;
		return 1;
	}
	return 0;
}

int SLRemove(SortedListT list, SLItemT newObj)
{
	SList sl;
	SLNode curr, prev;

	if(list == NULL)
	{
		printf("Passed in NULL list. \n");
		return 0;
	}

	sl = (SList) list;

	curr = sl->head;
	prev = NULL;

	while(curr != NULL)
	{
		if(curr->data == newObj)
		{
			if(prev == NULL)
			{
				sl->head = curr->next;
			}
			else
			{
				prev->next = curr->next;
			}
			sl->version++;
			free(curr);
			return 1;
		}
		prev = curr;
		curr = curr->next;
	}

	return 0;
}

SortedListIterT SLCreateIterator(SortedListT list)
{
	SList sl;
	Iterator iter;

	if(list == NULL)
	{
		printf("Passed in NULL list. \n");
		return NULL;
	}

	sl = (SList) list;

	iter = (Iterator) malloc( sizeof( struct Iterator_ ) );

	if( iter == NULL )
	{
		printf("ERROR: Malloc failed. \n");
		return NULL;
	}

	iter->list = sl;
	iter->current = NULL;
	iter->version = sl->version;

	return (SortedListIterT) iter;
}

void SLDestroyIterator(SortedListIterT liter)
{
	Iterator iter;

	if(liter == NULL)
	{
		printf("Passed in NULL iterator. \n");
		exit(0);
	}

	iter = (Iterator) liter;

	free(iter);
}

int SLNextItem(SortedListIterT iter, SLItemT *obj)
{
	Iterator temp;

	if(iter == NULL)
	{
		printf("Passed in NULL iterator. \n");
		return 0;
	}

	temp = (Iterator) iter;

	if(temp->version != temp->list->version) {
		return -1;
	}

	if(temp->current == NULL)
	{
		temp->current = temp->list->head;
		if(temp->current == NULL)
		{
			return 0;
		}
		*obj = temp->current->data;
		return 1;
	}
	else
	{
		temp->current = temp->current->next;
		if(temp->current == NULL)
		{
			return 0;
		}

		*obj = temp->current->data;
		return 1;
	}

	return -1;
}

