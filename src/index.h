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

#define DEBUG 0
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

/* compWords
 *
 * Function that comparses two words. Arguments are type void*.
 *
 * @param       word1       first word
 * @param       word2       second word
 *
 * @result      -1          word1 > word2
 * @result      0           word1 = word2
 * @result      1           word1 < word2
 */
 
int compWords(void* word1, void* word2);
 

/****************************************
 *          5. Indexer Functions        *
 ****************************************/
 
/* tokenizeFile
 *
 * Takes in a filename and inserts word entries into the global
 * wordTable object. Automatically increments frequency and adds
 * new files to the global filelist.
 *
 * @param   filename        the file to index
 *
 * @return  0
 */

int tokenizeFile( char* filename );

/* HTtoSL
 *
 * Function that converts a HashTable to a Sorted List.
 * Returns a new list on success and NULL on failure.
 *
 * @param   table       hashtable
 *
 * @return  success     SortedListT
 * @return  failure     NULL
 */

SortedListT HTtoSL(HashTable table);

/* indexFiles
 *
 * Writes the file list to an inverted index in the following 
 * format:
 *
 * <files>
 *      file#: filename
 *      file#: filename
 *      ... etc ...
 * </files>
 *
 * Returns a 1 on success, 0 on failure.
 *
 * @param   file        pointer to the file
 * @param   list        list of file entries
 *
 * @result  success     1
 * @result  failure     0
 */
int indexFiles(FILE* file, Entry list);

/* indexWord
 *
 * Writes a Word to an inverted index in the following format:
 *
 * <list> Word
 *      file#: frequency
 *      file#: frequency
 *      ... etc ...
 * </list>
 *
 * Returns a 1 on success, 0 on failure.
 *
 * @param   file        pointer to the file
 * @param   word        Word object to write
 *
 * @return  success     1
 * @return  failure     0
 */

int indexWord(FILE *file, Word word);


#endif /* SWIFT_INDEX_H_ */

