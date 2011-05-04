/*
 * File: search.h
 *
 * Author: Mike Swift
 * Email: theycallmeswift@gmail.com
 * Date Created: May 3rd, 2011
 * Date Modified: May 3rd, 2011
 */

#ifndef SWIFT_SEARCH_H_
#define SWIFT_SEARCH_H_

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
#define FILE_CHARS "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890 /\\-_()[]."

/********************************
 *      3. Parsing Functions    *
 ********************************/

/* parseIndex
 *
 * Function that takes in an inverted index and returns
 * a HashTable containing all the words and their entries.
 * Returns NULL on failure.
 *
 * @param   filename        name of the inverted index
 *
 * @return  success         new HashTable
 * @return  failure         NULL
 */

HashTable parseIndex(char* filename);

#endif
/* SWIFT_SEARCH_H_ */
