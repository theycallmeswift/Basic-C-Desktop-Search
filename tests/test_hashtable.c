/* test_hashtable.c
 * 
 * This file contains the unit tests for the HashTable Object.
 */

#include <stdio.h>
#include "testing.h"
#include "../src/hashtable.h"
#include <string.h>

int tests_run, failures;
HashTable table;

/* Misc.
 *
 * These functions are specific to the class being tested.
 */
 
unsigned long hash(const char *key)
{
	int h=0;
	while(*key) h=33*h + *key++;
	return h;
}

int compStrings(void* i, void* j) {
     if(strcmp((char *)i, (char*)j ) == 0)
     {
        return 0;
    }
    return 1;
}


/* Tests */

void run_tests()
{
    /* Test table Creation */
    
    table = createHT(0, 0.0, NULL, NULL);
    SW_ASSERT(table == NULL, "Size cannot be zero.", tests_run, failures);
    
    table = createHT(10, 0.0, NULL, NULL);
    SW_ASSERT(table == NULL, "Load Factor cannot be zero.", tests_run, failures);
    
    table = createHT(10, 0.65, NULL, NULL);
    SW_ASSERT(table == NULL, "Hashing function must be defined.", tests_run, failures);
    
    table = createHT(10, 0.65, hash, NULL);
    SW_ASSERT(table == NULL, "Comparison function must be defined.", tests_run, failures);
    
    table = createHT(10, 0.65, hash, compStrings);
    SW_ASSERT(table != NULL, "All valid inputs produces a new HashTable.", tests_run, failures);
    
    /* Test table Deleting */
}


int main(int argc, char **argv) {

    tests_run = 0;
    failures = 0;
    
    printf("Starting tests for Hashtable...\n");
    
    run_tests();
    
    printf("Ran %d tests, with %d failures.\n", tests_run, failures);
    if(failures == 0)
    {
        printf("ALL TESTS PASSED.\n");
    }
    return 0;
}

