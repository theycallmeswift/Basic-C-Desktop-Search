/* test_hashtable.c
 * 
 * This file contains the unit tests for the HashTable Object.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "testing.h"
#include "../src/hashtable.h"

int tests_run, failures;


/* Misc.
 *
 * These functions are specific to the class being tested.
 */
 
unsigned long hash(void *obj)
{
	char *key;
	
	key = (char *) obj;
	int h=0;
	while(*key) h=33*h + *key++;
	return h;
}

int compStrings(void* i, void* j) 
{
    if(strcmp((char *)i, (char*)j ) == 0)
    {
        return 0;
    }
    return 1;
}

void destroyString(void *str)
{
    free( (char*)str );
}

/* Tests */

void run_tests()
{
    HashTable table, self_destruct_table;
    unsigned long hashres;
    int i;
    char* str;
    
    /* Test table Creation */
    
    table = createHT(NULL, NULL, NULL, NULL);
    SW_ASSERT(table == NULL, "Hashing function must be defined.", tests_run, failures);
    
    table = createHT(hash, NULL, NULL, NULL);
    SW_ASSERT(table == NULL, "Comparison function must be defined.", tests_run, failures);
    
    table = createHT(hash, compStrings, NULL, NULL);
    SW_ASSERT(table != NULL, "All required inputs defined, No destroy functions.", tests_run, failures);
    
    self_destruct_table = createHT(hash, compStrings, destroyString, NULL);
    SW_ASSERT(self_destruct_table != NULL, "All valid inputs produces a new HashTable.", tests_run, failures);
    
    /* Test the Hash / Comp functions */
    hashres = hash("Test String");
    SW_ASSERT(hashres == 631841783, "Basic string hash.", tests_run, failures);
    
    hashres = hash("REALLLY REALLLY LONG, out of Contr0l $tr1nG th@t g3tz h(@)$hEd 4 Wh*t3v34 rezzzion....;';");
    SW_ASSERT(hashres == 238349644, "Long string hash.", tests_run, failures);

    hashres = hash("");
    SW_ASSERT(hashres == 0, "Empty string hash.", tests_run, failures);
    
    /* Test insert function */
    i = insertHT(NULL, (void *)"REALLLY REALLLY LONG, out of Contr0l $tr1nG th@t g3tz h(@)$hEd 4 Wh*t3v34 rezzzion....;';", (void *)1);
    SW_ASSERT(i == 0, "Cannot insert into NULL table.", tests_run, failures);
    
    i = insertHT(table, (void *)"REALLLY REALLLY LONG, out of Contr0l $tr1nG th@t g3tz h(@)$hEd 4 Wh*t3v34 rezzzion....;';", (void *)1);
    SW_ASSERT(i == 1, "Inserted into table", tests_run, failures);
    
    str = (char *)strdup("Test String");
    i = insertHT(self_destruct_table, (void *)str, (void *)1);
    SW_ASSERT(i == 1, "Self destructing table uses free functions properly to release allocated memory.", tests_run, failures);
    
    /* Destroy the tables */
    destroyHT(table);
    table = NULL;
    
    destroyHT(self_destruct_table);
    self_destruct_table = NULL;
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

