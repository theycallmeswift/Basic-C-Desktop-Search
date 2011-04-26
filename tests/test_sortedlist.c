/*
 * main.c
 *
 */

#include <stdio.h>

#include "../src/sorted-list.h"

struct IntPairT_ {
  int a, b;
};
typedef struct IntPairT_* IntPairT;

int compIntPair(void* i, void *j) {
  int valI, valJ;
  IntPairT iImpl, jImpl;

  iImpl = (IntPairT)i;
  jImpl = (IntPairT)j;

  valI = iImpl->a + iImpl->b;
  valJ = jImpl->a + jImpl->b;

  if (valI < valJ) {
    return -1;
  } else if (valI == valJ) {
    return 0;
  } else {
    return 1;
  }
}

int compInt(void* i, void* j) {
  if ((int)i < (int)j) {
    return -1;
  } else if ((int)i == (int)j) {
    return 0;
  } else {
    return 1;
  }
}

int main(int argc, char **argv)
{
  int i, res;
  IntPairT ip;
  void* item;
  int nums[] = {1, 5, 9, -3, 24, 45, 3, 3, 5, 5, 10, 10, 10, 12, 45};
  struct IntPairT_ numPairs[] = {{1,1},{2,2},{1,2},{3,4},{-1,1},{0,9},{2,2},{2,3}};

  /* Create list of integers */
  SortedListT sl = SLCreate(compInt);

  /* Insert test case */
  for (i = 0; i < 15; i++) {
    SLInsert(sl, (void *)nums[i]);
  }
  /* Create iterator and print out list */
  SortedListIterT iter = SLCreateIterator(sl);
  while ((res = SLNextItem(iter, &item)) == 1) {
    printf("%d\n", (int)item);
  }
  SLDestroyIterator(iter);

  /* Try removing some of the items */
  SLRemove(sl, (void *)45);
  SLRemove(sl, (void *)9);
  SLRemove(sl, (void *)10);
  SLRemove(sl, (void *)10);

  /* Create iterator and print out list */
  iter = SLCreateIterator(sl);
  while ((res = SLNextItem(iter, &item)) == 1) {
    printf("%d\n", (int)item);
  }
  SLDestroyIterator(iter);

  /* Create iterator, change list, try print out list */
  /* Testing whether code detects change after iterator has been created */
  iter = SLCreateIterator(sl);
  SLRemove(sl, (SLItemT)24);
  while ((res = SLNextItem(iter, &item)) == 1) {
    printf("%d\n", (int)item);
  }
  SLDestroyIterator(iter);

  /* Deallocate list */
  SLDestroy(sl);

  /* Create list of integer pairs */
  sl = SLCreate(compIntPair);

  /* Insert test case */
  for (i = 0; i < 8; i++) {
    SLInsert(sl, (SLItemT)&numPairs[i]);
  }

  /* Try removing some of the items */
  SLRemove(sl, (SLItemT)&numPairs[0]);
  SLRemove(sl, (SLItemT)&numPairs[1]);

  /* Create iterator and print out list */
  iter = SLCreateIterator(sl);
  while ((res = SLNextItem(iter, &item)) == 1) {
    ip = (IntPairT)item;
    printf("(%d, %d)\n", ip->a, ip->b);
  }
  SLDestroyIterator(iter);

  /* Destroy list */
  SLDestroy(sl);

  return 1;
}
