/*
 * File: sorted-list.c
 *
 * Author: Mike Swift
 * Email: theycallmeswift@gmail.com
 * Date Created: March 8th, 2011
 * Date Modified: April 26rd, 2011
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "sorted-list.h"

/* Internal Structs */

/* SLItemT
 *
 * SLItemTs are nodes in the sorted list.  Each node holds some data
 * which is stored as void * and a pointer to the next node in the 
 * sorted list.
 */
 
struct SLItemT_ {
    void* data;
    struct SLItemT_* next;
};

/* SortedListT
 *
 * SortedListT is the actual struct for the sorted list. It holds pointers 
 * to the first node in the list and a comparison function.  Also, there is
 * an integer that keeps track of changes to the list so the iterator can 
 * work properly.
 */

struct SortedListT_ {
    SLItemT head;
    CompareFuncT comp;
    DestroyDataFuncT destroy;
    int version;
};

/* SortedListIterT
 *
 * Iterator for a sorted list. Holds a pointer to the list, a pointer to the 
 * current item, and an integer that keeps track of the list's version when
 * the iterator was created. 
 */

struct SortedListIterT_ {
    SortedListT sl;
    SLItemT curr;
    int version;
};

/*
 * SLCreate creates a new, empty sorted list.  The caller must provide
 * a comparator function that can be used to order objects that will be
 * kept in the list.
 *
 * If the function succeeds, it returns a (non-NULL) SortedListT object.
 * Else, it returns NULL.
 */

SortedListT SLCreate(CompareFuncT cf, DestroyDataFuncT destroy)
{
    SortedListT sl;
    
    sl = (SortedListT) malloc( sizeof( struct SortedListT_ ) );
    
    if( sl == NULL )
    {
        fprintf(stderr, "Error: Could not malloc sorted-list. Out of memory.\n");
        return NULL;
    }
        
    sl->comp = cf;
    sl->destroy = destroy;
    sl->version = 0;
    sl->head = NULL;
    
    return sl;
}

/*
 * SLDestroy destroys a list, freeing all dynamically allocated memory.
 * Ojects on the list should NOT be deallocated, however.  That is the
 * responsibility of the user of the list.
 */
void SLDestroy(SortedListT list)
{
    SLItemT curr, next;
    if(list != NULL)
    {
        curr = list->head;
        
        while(curr != NULL)
        {
            next = curr->next;
            
            if(list->destroy)
            {
                list->destroy(curr->data);
            }
            
            free(curr);
            curr = next;
        }
        
        free(list);
    }
    
    return;
}


/*
 * SLInsert inserts a given object into a sorted list, maintaining sorted
 * order (largest at front of the list, smallest at back) of all objects
 * in the list.  If the new object is equal to a subset
 * of existing objects in the list, then the subset can be kept in any
 * order.
 *
 * Note that you need to check ordering of objects using the comparator
 * function.  You should NOT do the comparison yourself using the pointers
 * to the objects.
 *
 * If the function succeeds, it returns 1.  Else, it returns 0.
 */

int SLInsert(SortedListT list, void* data)
{
    SLItemT newNode, curr, prev;
    
    if(list == NULL)
    {
        fprintf(stderr, "Cannot insert into NULL list.\n");
        return 0;
    }
    
    newNode = (SLItemT) malloc( sizeof( struct SLItemT_ ) );
    if(newNode == NULL)
    {
        fprintf(stderr, "Error: Could not malloc new node. Out of memory.\n");
        return 0;
    }
    
    newNode->data = data;
    
    /* If we don't have a comparison function or the head is empty. Insert the
     * new node at the front of the list. 
     */
    
    if(list->comp == NULL || list->head == NULL)
    {
        newNode->next = list->head;
        list->head = newNode;
    }
    else
    {
        curr = list->head;
        prev = NULL;
        
        while(curr != NULL && list->comp(data, curr->data) != -1)
        {
            prev = curr;
            curr = curr->next;
        }
        
        if( prev == NULL )
        {
            newNode->next = list->head;
            list->head = newNode;
        }
        else
        {
            prev->next = newNode;
            newNode->next = curr;
        }
    }
    
    list->version++;
    
    return 1;
}


/*
 * SLRemove removes a given object from a sorted list.  Sorted ordering
 * should be maintained.
 *
 * Different from SLInsert, for this function, you should locate the object
 * by the pointer given.
 *
 * If the function succeeds, it returns 1.  Else, it returns 0.
 */

int SLRemove(SortedListT list, void* data)
{
    SLItemT curr, prev;
    
    if(list == NULL)
    {
        fprintf(stderr, "Cannot remove from NULL list.\n");
        return 0;
    }
    
    if(list->head == NULL)
    {
        return 0;
    }
    
    curr = list->head;
    prev = NULL;
    
    while(curr != NULL)
    {
        if(list->comp(data, curr->data) == 0)
        {
            /* Match Found! */
            if(prev == NULL)
            {
                list->head = curr->next;
            }
            else
            {
                prev->next = curr->next;                    
            }
            
            if(list->destroy)
            {
                list->destroy(curr->data);
            }
            
            free(curr);
            
            list->version++;
            return 1;
        }
        prev = curr;
        curr = curr->next;
    }
    
    /* Couldn't find what we were looking for. Peace! */
    
    return 0;
}


/*
 * SLCreateIterator creates an iterator object that will allow the caller
 * to "walk" through the list from beginning to the end using SLNextItem.
 *
 * If the function succeeds, it returns a non-NULL SortedListIterT object.
 * Else, it returns NULL.
 */

SortedListIterT SLCreateIterator(SortedListT list)
{
    SortedListIterT iter;
    
    if(list == NULL)
    {
        fprintf(stderr, "Cannot create iterator from a NULL list.\n");
        return NULL;
    }
    
    iter = (SortedListIterT) malloc( sizeof(struct SortedListIterT_) );
    if(iter == NULL)
    {
        fprintf(stderr, "Could not malloc memory for iterator.\n");
        return NULL;
    }
    
    iter->sl = list;
    iter->version = list->version;
    iter->curr = list->head;    
    
    return iter;
}

/*
 * SLDestroyIterator destroys an iterator object that was created using
 * SLCreateIterator().  Note that this function should destroy the
 * iterator but should NOT affectt the original list used to create
 * the iterator in any way.
 */

void SLDestroyIterator(SortedListIterT liter)
{
    free(liter);
    return;
}


/*
 * SLNextItem returns the next object in the list encapsulated by the
 * given iterator in the parameter obj.  It returns 1 when successful.
 * It returns 0 if it has reached the end of the list.  It returns -1
 * if it encounters an error (e.g., the list has changed--see below).
 *
 * One complication you MUST consider/address is what happens if a
 * sorted list encapsulated within an iterator is modified while that
 * iterator is active.  For example, what if an iterator is "pointing"
 * to some object in the list as the next one to be returned but that
 * object is removed from the list using SLRemove() before SLNextItem()
 * is called.
 */

int SLNextItem(SortedListIterT iter, void* *obj)
{
    if(iter == NULL)
    {
        fprintf(stderr, "Passed in a NULL iterator.\n");
        return -1;
    }
    
    if(iter->version != iter->sl->version)
    {
        fprintf(stderr, "Iterator and list version do not match!\n");
        return -1;
    }
    
    if(iter->curr == NULL)
    {
        return 0;
    }
    
    *obj = iter->curr->data;
    iter->curr = iter->curr->next;
    
    return 1;
}
