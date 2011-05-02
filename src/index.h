/*
 * File: index.h
 *
 * Author: Mike Swift
 * Email: theycallmeswift@gmail.com
 * Date Created: April 26th, 2011
 * Date Modified: May 2nd, 2011
 */

#ifndef SWIFT_INDEX_H_
#define SWIFT_INDEX_H_

/********************************
 *          1. Includes         *
 ********************************/

#include "hashtable.h"
#include "tokenizer.h"
#include "sorted-list.h"

/********************************
 *          2. Constants        *
 ********************************/

#define DEBUG 1

#define STRING_CHARS "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890"

/********************************
 *          3. Typedefs         *
 ********************************/
/* Word holds the string and a pointer to a linked list of file Entries */
typedef struct Word_* Word;

/* Entry holds the filename where it was found and the frequency in the file */
typedef struct Entry_* Entry;

/********************************
 *      4. Word Functions       *
 ********************************/
 
/* createWord
 *
 * Create a new Word object from a word. Returns NULL on
 * failure.
 * 
 * @param   word        String being stored
 *
 * @return  success     new Word
 * @return  failure     NULL
 */

Word createWord(char *word);

/* destroyWord
 *
 * Destroys a word object and the list of file Entries. 
 * Expected type is void*. When NULL is passed to the 
 * function, no action will be taken.
 *
 * @param       wordptr          void* pointer to a word object
 *
 * @return      void
 */

void destroyWord(void *wordptr);

/* insertEntry
 *
 * Inserts an entry into a word object. If the file is
 * already in the list, the frequency will be incremented
 * instead of creating a new node.
 *
 * @param       word            word object
 * @param       filename        where the file was found
 *
 * @return      new Filename    2
 * @return      increased freq  1
 * @return      failure         0
 */

int insertEntry(Word word, char *filename);

/* printWord
 *
 * Simple toString method for word objects
 *
 * @param       ptr        pointer to word object
 *
 * @return      void
 */

void printWord(void* ptr);


/* This function produces the sorted list of entries.
 *
 * Steps:
 * 1) Create a Tokenizer for the file and an empty hash map
 * 2) Read a word and create a new entry
 * 3) Insert it into the hashtable
 * 4) If the entry already exists, find it in the hashtable and increment
 * 5) Otherwise Insert it into the list as well
 */

int tokenizeFile( SortedListT list, char* filename );


#endif /* SWIFT_INDEX_H_ */

