/*
 * sorted-list.h
 */

#ifndef SORTED_LIST_H_
#define SORTED_LIST_H_

/*
 * Define SortedListT and SLItemT so caller code can look nicer than
 * using void*.
 */
typedef void* SLItemT;
typedef void* SortedListT;


/*
 * Iterator type for user to "walk" through the list item by item, from
 * beginning to end.
 * Define SortedListIterT so caller code can look nicer than using void*.
 */
typedef void* SortedListIterT;


/*
 * When your sorted list is used to store objects of some type, since the
 * type is opaque to you, you will need a comparator function to order
 * the objects in your sorted list.
 *
 * You can expect a comparator function to return -1 if the 1st object is
 * smaller, 0 if the two objects are equal, and 1 if the 2nd object is
 * smaller.
 *
 * Note that you are not expected to implement any comparator functions.
 * You will be given a comparator function when a new sorted list is
 * created.
 */

typedef int (*CompareFuncT)(SLItemT, SLItemT);


/*
 * SLCreate creates a new, empty sorted list.  The caller must provide
 * a comparator function that can be used to order objects that will be
 * kept in the list.
 *
 * If the function succeeds, it returns a (non-NULL) SortedListT object.
 * Else, it returns NULL.
 */

SortedListT SLCreate(CompareFuncT cf);

/*
 * SLDestroy destroys a list, freeing all dynamically allocated memory.
 * Ojects on the list should NOT be deallocated, however.  That is the
 * responsibility of the user of the list.
 */
void SLDestroy(SortedListT list);


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

int SLInsert(SortedListT list, SLItemT newObj);


/*
 * SLRemove removes a given object from a sorted list.  Sorted ordering
 * should be maintained.
 *
 * Different from SLInsert, for this function, you should locate the object
 * by the pointer given.
 *
 * If the function succeeds, it returns 1.  Else, it returns 0.
 */

int SLRemove(SortedListT list, SLItemT newObj);


/*
 * SLCreateIterator creates an iterator object that will allow the caller
 * to "walk" through the list from beginning to the end using SLNextItem.
 *
 * If the function succeeds, it returns a non-NULL SortedListIterT object.
 * Else, it returns NULL.
 */

SortedListIterT SLCreateIterator(SortedListT list);

/*
 * SLDestroyIterator destroys an iterator object that was created using
 * SLCreateIterator().  Note that this function should destroy the
 * iterator but should NOT affectt the original list used to create
 * the iterator in any way.
 */

void SLDestroyIterator(SortedListIterT liter);


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

int SLNextItem(SortedListIterT iter, SLItemT *obj);


#endif /* SORTED_LIST_H_ */
