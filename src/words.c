/*
 * File: words.c
 *
 * Author: Mike Swift
 * Email: theycallmeswift@gmail.com
 * Date Created: May 3rd, 2011
 * Date Modified: May 3rd, 2011
 */
 
/********************************
 *      1. Includes             *
 ********************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "words.h"

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

Word createWord(char *word)
{
    Word newWord;
    newWord = (Word) malloc( sizeof( struct Word_ ) );
     
    if( newWord == NULL )
    {
        fprintf(stderr, "Error: Could not allocate memory for Word.\n");
        return NULL;
    }
    
    newWord->word = (char *) malloc( sizeof( char ) * ( strlen(word) + 1 ) );
    if( newWord->word == NULL )
    {
        free(newWord);
        fprintf(stderr, "Error: Could not allocate memory for Word.\n");
        return NULL;
    }
    strcpy(newWord->word, word);
    
    /* Set the number of files and total appearances to 0 */
    newWord->numFiles = 0;
    newWord->totalAppearances = 0;
    
    newWord->head = NULL;

    return newWord;
}


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

void destroyWord(void *wordptr)
{
    Word word;
    Entry curr, next;
    
    if( wordptr != NULL )
    {
        word = (Word) wordptr;
        free(word->word);
        
        curr = word->head;
        
        while(curr != NULL)
        {
            next = curr->next;
            if(curr->filename != NULL)
            {
                free(curr->filename);
            }
            free(curr);
            curr = next;
        }
        
        free(word);
    }
}

/* createEntry
 *
 * Creates a brand new entry object. If NULL is passed in for 
 * filename, it will be stored as NULL.
 *
 * @param   filename        the filename where the entry occured
 * @param   filenum         alternative to name is number
 * @param   frequency       frequency of the word in file
 *
 * @return  success         new Entry
 * @return  failure         NULL
 */
Entry createEntry(char *filename, int filenum, int frequency)
{
    Entry ent;
    
    ent = (Entry) malloc( sizeof(struct Entry_) );
    if(ent == NULL)
    {
        fprintf(stderr, "Error: Could not allocate memory for Entry.\n");
        return NULL;
    }
    
    if(filename != NULL)
    {
        ent->filename = (char*) malloc( sizeof(char) * (strlen(filename) + 1));
        if(ent->filename == NULL)
        {
            free(ent);
            fprintf(stderr, "Error: Could not allocate memory for Entry.\n");
            return NULL;
        }
        strcpy(ent->filename, filename);
    }
    else
    {
        ent->filename = NULL;
    }
    
    ent->frequency = frequency;
    ent->filenumber = filenum;
    
    ent->next = NULL;
    
    return ent;
}


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

int insertEntry(Word word, char *filename)
{
    Entry ent;
    int res;
    
    if(word == NULL)
    {
        fprintf(stderr, "Error: Word cannot be NULL.\n");
        return 0;
    }
    
    if(filename == NULL)
    {
        fprintf(stderr, "Error: Filename cannot be NULL.\n");
        return 0;
    }
    
    ent = word->head;
    
    /* Check if the filename is already in the list, if so increment and return */
    while(ent != NULL)
    {
        res = strcmp(filename, ent->filename);
        if( res == 0 )
        {
            ent->frequency++;
            word->totalAppearances++;
            return 1;
        }
        ent = ent->next;
    }
    
    /* Filename is not in the list, create a new entry and insert it at the head */
    ent = createEntry(filename, -1, 1);
    
    ent->next = word->head;
    word->head = ent;
    
    word->numFiles++;
    word->totalAppearances++;
    
    return 2;
}

/* sortEntries
 *
 * Sorts the entries in a word by frequency (desc).
 *
 * @param   word        the word whose entries should be sorted
 *
 * @return  success     1
 * @return  failure     0
 */
int sortEntries(Word word)
{
    Entry curr, next, sorted, scurr, sprev;
    int diff, inserted;
    
    if(word == NULL)
    {
        fprintf(stderr, "Error: Cannot sort entries of NULL word.\n");
        return 0;
    }
    
    curr = word->head;
    sorted = NULL;
    
    while(curr != NULL)
    {
        next = curr->next;
        
        /* Set scurr to the front of the list, sprev to NULL, and inserted to 0 */
        scurr = sorted;
        sprev = NULL;
        inserted = 0;
        
        while(scurr != NULL && inserted == 0)
        {
            diff = curr->frequency - scurr->frequency;
            
            if(diff >= 0)
            {
                /* The new one is bigger than the current node */
                if(sprev == NULL)
                {
                    /* Insert it at the front of the list */
                    curr->next = scurr;
                    sorted = curr;
                }
                else
                {
                    sprev->next = curr;
                    curr->next = scurr;
                }
                inserted = 1;
            }
            
            sprev = scurr;
            scurr = scurr->next;
        }
        
        if(inserted == 0)
        {
            /* We reached the end of the list, put it there */
            if(sprev == NULL)
            {
                /* Insert it at the front of the list */
                curr->next = scurr;
                sorted = curr;
            }
            else
            {
                sprev->next = curr;
                curr->next = scurr;
            }
        }
        
        curr = next;
    }
    
    word->head = sorted;
    
    return 1;
}

/* printWord
 *
 * Simple toString method for word objects
 *
 * @param       ptr        pointer to word object
 *
 * @return      void
 */

void printWord(void* ptr)
{
    Word word;
    Entry ent;
    
    word = (Word) ptr;
    
    if(word != NULL)
    {
        printf("(%s, # Files: %i, Freq: %i, {", word->word, word->numFiles, word->totalAppearances);
        
        ent = word->head;
        
        while(ent != NULL)
        {
            if(ent->filename != NULL)
            {
                printf("[%s (%i), %i]->", ent->filename, ent->filenumber, ent->frequency);
            }
            else
            {
                printf("[%i, %i]->", ent->filenumber, ent->frequency);
            }
            
            ent = ent->next;
        }
        
        printf("NULL})");
    }
}

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
 
int compWords(void* word1, void* word2)
{
    Word w1, w2;
    
    w1 = (Word) word1;
    w2 = (Word) word2;
    
    return strcmp(w1->word, w2->word);
}
