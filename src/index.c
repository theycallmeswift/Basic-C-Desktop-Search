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
SortedListT wordSL;
HashTable wordTable;
Entry file_list;


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
            free(curr->filename);
            free(curr);
            curr = next;
        }
        
        free(word);
    }
}

/* createEntry
 *
 * Creates a brand new entry object.
 *
 */
Entry createEntry(char *filename)
{
    Entry ent;
    
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
    if(DEBUG) printf("insertEntry: Setting filename = %s.\n", filename);
    strcpy(ent->filename, filename);
    
    ent->frequency = 1;
    
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
        if(DEBUG) printf("insertEntry: (%s) Comparing %s and %s.\n", word->word, filename, ent->filename);
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
    ent = createEntry(filename);
    
    ent->next = word->head;
    word->head = ent;
    
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

/********************************
 *      5. Helper Functions     *
 ********************************/
 
/* plist
 *
 * Walk through a directory and tokenize each of its files.
 *
 * @param   name        name of file or directory
 * @param   status      unused param
 * @param   type        type of object (file, dir, ect)
 *
 * @return  0
 */

int plist(const char *name, const struct stat *status, int type) {

    if(type == FTW_NS)
    {
        return 0;
    }

    if(type == FTW_F)
    {
        if(DEBUG) printf("plist: Attempting to tokenize %s.\n", (char *) name);
        tokenizeFile( (char *) name );
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

/* printWordHT
 *
 * Wrapper for printing words with the HT print function.
 *
 * @param   key     Not used
 * @param   val     the word to print.
 *
 * @return  void
 */

void printWordHT(void *key, void* val)
{
    if(val != NULL)
    {
        printWord((Word) val);
        printf("->");
    }
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
    if(str != NULL) free( (char*)str );
}


/********************************
 *      6. Indexer Functions    *
 ********************************/
 
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

int tokenizeFile( char* filename )
{
    TokenizerT tok;
    Word word;
    char *str;
    int res;
    Entry file;
    
    /* Insert filename into the file_list */
    file = createEntry(filename);
    file->next = file_list;
    file_list = file;
        
    /* Create a Tokenizer for the file */
    tok = TKCreate(STRING_CHARS, filename);
    assert(tok != NULL);
    
    if(DEBUG) printf("tokenizeFile: Created Tokenizer.\n");
    
    /* Parse the file */
    while((str = TKGetNextToken(tok)) != 0)
    {        
        /* Search the hash table for the key/file combo */
        if(DEBUG) { printf("Searching for %s\n", str); }
        word = (Word) searchHT(wordTable, (void*)str);
        
        if(word == NULL)
        {
            if(DEBUG) printf("tokenizeFile: Couldn't find %s in HT.\n", str);
            
            /* Create the word */
            word = createWord(str);
            assert(word != NULL);
            
            /* Append the file entry */
            res = insertEntry(word, filename);
            assert(res != 0);
            
            /* Insert it into the HT */
            res = insertHT(wordTable, (void*) str, (void*) word);
            assert(res != 0);
        }
        else
        {
            if(DEBUG) printf("tokenizeFile: Found %s in HT.\n", str);
            
            res = insertEntry(word, filename);
            assert(res != 0);

            if(str != NULL) free(str);
        }
    }
    
    TKDestroy(tok);
    tok = NULL;
    
    if(DEBUG) printf("tokenizeFile: Destroyed Tokenizer.\n");
    
    return 0;
}


int main( int argc, char** argv )
{    
    int i;
    Entry ent;
    
    /* Validate the inputs */
    if( (argc == 2 && argv[1][0] == '-' && argv[1][1] == 'h') || argc < 3 )
    {
        fprintf(stderr, "Usage: %s <inverted-index filename> <file or directory>\n", argv[0]);
        return 1;
    }
    
    /* Create a new Sorted List */
    wordSL = SLCreate(compWords, destroyWord);
    assert(wordSL != NULL);
    
    if(DEBUG) printf("main: Created Sorted List for Entries.\n");
    
    /* Create a HashTable to hold our entries. */
    wordTable = createHT(hash, compStrings, destroyString, destroyWord, printWordHT);
    assert(wordTable != NULL);
    
    if(DEBUG) printf("main: Created HashTable.\n");
    
    /* Set file_list = NULL because the list starts out empty */
    file_list = NULL;
    
    /* Recursivly walk through each file in a directory and tokenize */    
    ftw(argv[2], plist, 1);
    
    /* Print out the HT */
    toStringHT(wordTable);
    
    /* Print out the file list */
    printf("Files:\n");
    ent = file_list;
    i = 0;
    
    while(ent != NULL)
    {
        printf("[%i]: %s\n", i, ent->filename);
        i++;
        ent = ent->next;
    }
    
    /* We're done with our HT, destroy it */
    destroyHT(wordTable);
    wordTable = NULL;
    
    if(DEBUG) printf("main: Destroyed HashTable.\n");
    
    /* We're done with our Sorted List, destroy it */
    SLDestroy(wordSL);
    wordSL = NULL; 
    
    if(DEBUG) printf("main: Entry list successfully destroyed.\n");
    
    return 1;
}