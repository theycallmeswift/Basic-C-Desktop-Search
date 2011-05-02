/*
 * File: index.c
 *
 * Author: Mike Swift
 * Email: theycallmeswift@gmail.com
 * Date Created: April 26th, 2011
 * Date Modified: May 2nd, 2011
 */
 
/********************************
 *          1. Includes         *
 ********************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <ftw.h>
#include "index.h"

/********************************
 *          2. Globals          *
 ********************************/
SortedListT entrySL;

/********************************
 *          3. Structs          *
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
    Entry next;
};

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
            free(curr->filename);
            free(curr);
            curr = next;
        }
        
        free(word);
    }
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
    ent = (Entry) malloc( sizeof(struct Entry_) );
    if(ent == NULL)
    {
        fprintf(stderr, "Error: Could not allocate memory for Entry.\n");
        return 0;
    }
    
    ent->filename = (char*) malloc( sizeof(char) * (strlen(filename) + 1));
    if(ent->filename == NULL)
    {
        free(ent);
        fprintf(stderr, "Error: Could not allocate memory for Entry.\n");
        return 0;
    }
    
    strcpy(ent->filename, filename);
    
    ent->frequency = 1;
    word->numFiles++;
    word->totalAppearances++;
    
    return 2;
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
            printf("[%s, %i]->", ent->filename, ent->frequency);
        }
        
        printf("NULL}), ");
    }
}

/********************************
 *      5. Helper Functions     *
 ********************************/
 
/* Walk through a directory and index its files */

int plist(const char *name, const struct stat *status, int type) {

    if(type == FTW_NS)
    {
        return 0;
    }

    if(type == FTW_F)
    {
        if(DEBUG) printf("plist: Attempting to tokenize %s.\n", (char *) name);
        tokenizeFile( entrySL, (char *) name );
    }

    return 0;
}

unsigned long hash(void *obj)
{
	char *key;
	int h;
	
	key = (char *) obj;
	h=0;
	while(*key) h=33*h + *key++;
	return h;
}

/* compStrings
 *
 * Void* wrapper for strcmp. To be used with 
 * the HashTable's internal comparison function.
 *
 * @param   str1        1st string
 * @param   str2        2nd string
 *
 * @return  -1          str1 < str2
 * @return  0           str1 = str2
 * @return  1           str1 > str2
 */

int compStrings(void *str1, void *str2)
{
    return strcmp((char*)str1, (char*)str2);
}

void destroyString(void *str)
{
    free( (char*)str );
}


/********************************
 *      Primary Functions       *
 ********************************/

int tokenizeFile( SortedListT list, char* filename )
{
    TokenizerT tok;
    HashTable table;
    Word word;
    char *str;
    int res;
    
    /* Create a HashTable to hold our entries. */
    table = createHT(hash, compStrings, destroyString, destroyWord, printWord);
    assert(table != NULL);
    
    if(DEBUG) printf("tokenizeFile: Created HashTable.\n");
    
    /* Create a Tokenizer for the file */
    tok = TKCreate(STRING_CHARS, filename);
    assert(tok != NULL);
    
    if(DEBUG) printf("tokenizeFile: Created Tokenizer.\n");
    
    /* Parse the file */
    while((str = TKGetNextToken(tok)) != 0)
    {        
        /* Search the hash table for the key/file combo */
        if(DEBUG) { printf("Searching for %s\n", str); }
        
    }
    
    TKDestroy(tok);
    tok = NULL;
    
    toStringHT(table);
    
    if(DEBUG) printf("tokenizeFile: Destroyed Tokenizer.\n");
    
    destroyHT(table);
    table = NULL;
    
    if(DEBUG) printf("tokenizeFile: Destroyed HashTable.\n");
    
    return 0;
}


int main( int argc, char** argv )
{    
    /* Validate the inputs */
    if( (argc == 2 && argv[1][0] == '-' && argv[1][1] == 'h') || argc < 3 )
    {
        fprintf(stderr, "Usage: %s <inverted-index filename> <file or directory>\n", argv[0]);
        return 1;
    }
    
    /* entrySL = SLCreate(compEntries, destroyEntry);
    assert(entrySL != NULL); */
    
    if(DEBUG) printf("main: Created Sorted List for Entries.\n");
    
    ftw(argv[2], plist, 1);
    
    /*
    SLDestroy(entrySL);
    entrySL = NULL; */
    
    if(DEBUG) printf("main: Entry list successfully destroyed.\n");
    
    return 1;
}
