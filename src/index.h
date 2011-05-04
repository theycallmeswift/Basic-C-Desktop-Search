/*
 * File: index.h
 *
 * Author: Mike Swift
 * Email: theycallmeswift@gmail.com
 * Date Created: April 26th, 2011
 * Date Modified: May 3rd, 2011
 */

#ifndef SWIFT_INDEX_H_
#define SWIFT_INDEX_H_

/********************************
 *          1. Includes         *
 ********************************/

#include "hashtable.h"
#include "tokenizer.h"
#include "sorted-list.h"
#include "words.h"

/********************************
 *          2. Constants        *
 ********************************/

#define DEBUG 0
#define STRING_CHARS "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890"


/****************************************
 *          3. Indexer Functions        *
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
 *      file#:filename
 *      file#:filename
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

