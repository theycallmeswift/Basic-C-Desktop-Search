/* test_hashtable.c
 * 
 * This file contains the unit tests for the HashTable Object.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "testing.h"
#include "../src/hashtable.h"
#include "../src/tokenizer.h"

#define ALLOWED_CHARS "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890/"

int tests_run, failures;

struct Entry_ {
    int frequency;
    char *filename;
};

typedef struct Entry_* Entry;


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
    return strcmp((char *)i, (char*)j);
}

void destroyString(void *str)
{
    free( (char*)str );
}

void destroyEntry(void *ptr)
{
    Entry ent = (Entry)ptr;
    free(ent->filename);
    free(ent);
}

void printPair(void* key, void* val)
{
    printf("(%s, %s, %d)->", (char *)key, ((Entry)val)->filename, ((Entry)val)->frequency);
}

/* Tests */

void run_tests()
{
    HashTable table, self_destruct_table;
    unsigned long hashres;
    int i;
    char* str, *filename;
    TokenizerT tok = NULL;
    Entry ent;
    
    /* Test table Creation */
    
    table = createHT(NULL, NULL, NULL, NULL, NULL);
    SW_ASSERT(table == NULL, "Hashing function must be defined.", tests_run, failures);
    
    table = createHT(hash, NULL, NULL, NULL, NULL);
    SW_ASSERT(table == NULL, "Comparison function must be defined.", tests_run, failures);
    
    table = createHT(hash, compStrings, NULL, NULL, NULL);
    SW_ASSERT(table != NULL, "All required inputs defined, No destroy functions.", tests_run, failures);
    
    self_destruct_table = createHT(hash, compStrings, destroyString, destroyEntry, printPair);
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
    SW_ASSERT(i == 1, "Insert normal data into table", tests_run, failures);
    
    tok = TKCreate(ALLOWED_CHARS, "/home/swift/Documents/Class/S2011/cs214/Project/bin/files/file1.txt");
    
    filename = "file1.txt";
    
    while((str = TKGetNextToken(tok)) != 0)
    {
        ent = (Entry) malloc( sizeof(struct Entry_) );
        ent->filename = strdup(filename);
        ent->frequency = 1;
        
        i = insertHT(self_destruct_table, (void *)str, (void *) ent);
    }
    
    TKDestroy(tok);
    tok = NULL;
    
    /* Testing Search Functions */
    searchHT(NULL, NULL);
    searchHT(table, NULL);
    
    ent = (Entry) searchHT(self_destruct_table, "varius");
    SW_ASSERT(ent != NULL, "Found valid key.", tests_run, failures);    
    
    ent = (Entry) searchHT(self_destruct_table, "");
    SW_ASSERT(ent == NULL, "Could not find invalid key.", tests_run, failures);   
    
    /* Testing removal functions */
    removeHT(self_destruct_table, "tempor");
    removeHT(self_destruct_table, "tempor");
    removeHT(self_destruct_table, "tempor");
    removeHT(self_destruct_table, "donec");
    
    
    /* Testing Print Functions */
    toStringHT(NULL);
    toStringHT(table);
    toStringHT(self_destruct_table);
    
    rehash(self_destruct_table);
    
    toStringHT(self_destruct_table);
    
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

