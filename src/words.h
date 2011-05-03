/*
 * File: words.h
 *
 * Author: Mike Swift
 * Email: theycallmeswift@gmail.com
 * Date Created: May 3rd, 2011
 * Date Modified: May 3rd, 2011
 */

#ifndef SWIFT_WORDS_H_
#define SWIFT_WORDS_H_

/********************************
 *          1. Structs          *
 ********************************/

/* Entry_
 *
 * @param   filename    filename and path
 * @param   frequency   how often the word appears
 * @param   next        next entry in list
 */

struct Entry_ {
    char *filename;
    int frequency;
    struct Entry_* next;
};

typedef struct Entry_* Entry;

/* Word_
 *
 * @param   word                word string
 * @param   head                pointer to head of entry list
 * @param   numFiles            The number of files the word appears in
 * @param   totalAppearances    The total number of appearances
 */

struct Word_ {
    char *word;
    Entry head;
    int numFiles;
    int totalAppearances;
};

typedef struct Word_* Word;

/********************************
 *      2. Word Functions       *
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

/* createEntry
 *
 * Creates a brand new entry object.
 *
 * @param   filename        the filename where the entry occured
 *
 * @return  success         new Entry
 * @return  failure         NULL
 */
Entry createEntry(char *filename);

/* sortEntries
 *
 * Sorts the entries in a word by frequency (desc).
 *
 * @param   word        the word whose entries should be sorted
 *
 * @return  success     1
 * @return  failure     0
 */
int sortEntries(Word word);

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

#endif
/* SWIFT_WORDS_H_ */
