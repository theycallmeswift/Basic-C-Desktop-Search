/*
 * File: index.c
 *
 * Author: Mike Swift
 * Email: theycallmeswift@gmail.com
 * Date Created: April 26th, 2011
 * Date Modified: May 3rd, 2011
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
HashTable wordTable;
Entry file_list;

/********************************
 *      3. Helper Functions     *
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

/* hash
 *
 * Simple hashing function for strings.
 *
 * @param   obj         void* object to hash (string)
 *
 * @return  unsigned long hash
 */

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

/* destroyString
 *
 * Void * wrapper for freeing strings. To be used with
 * the HashTables's internal destroy function.
 *
 * @param   str         string to free
 *
 * @return  void
 */

void destroyString(void *str)
{
    if(str != NULL) free( (char*)str );
}


/********************************
 *      4. Indexer Functions    *
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

SortedListT HTtoSL(HashTable table)
{
    HTIterator iter;
    int res;
    void *key, *val;
    char* str;
    Word word;
    SortedListT list;
    
    /* Create a new Sorted List */
    list = SLCreate(compWords, destroyWord);
    if(list == NULL)
    {
        fprintf(stderr, "Error: Could not allocate enough memory for list.\n");
        return NULL;
    }
    
    iter = createIterHT(table);
    
    while((res = HTNextItem(iter, &key, &val) == 1))
    {
        str = (char*)key;
        word = (Word)val;
        
        SLInsert(list, (void*)word);
    }
        
    destroyIterHT(iter);
    iter = NULL;
    
    return list;
}

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
int indexFiles(FILE* file, Entry list)
{    
    int i;
    char buffer[1024];
    
    if(file == NULL)
    {
        fprintf(stderr, "Error: Index file cannot be NULL.\n");
        return 0;
    }
    
    fputs("<files>\n", file);
    
    i = 0;
    
    while(list != NULL)
    {
        fputs("\t", file);
        
        /* Convert the file number to a string */
        sprintf(buffer, "%i", i);
        
        fputs(buffer, file);
        fputs(": ", file);
        fputs(list->filename, file);
        fputs("\n", file);
        
        list = list->next;
        i++;
    }
    
    fputs("</files>\n", file);
    return 1;    
}

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

int indexWord(FILE *file, Word word)
{
    Entry ent, currfile;
    char buffer[255];
    int i;
    
    if(file == NULL)
    {
        fprintf(stderr, "Error: Index file cannot be NULL.\n");
        return 0;
    }
    
    if(word == NULL)
    {
        fprintf(stderr, "Error: Word cannot be NULL.\n");
        return 0;
    }
    
    /* Write the <list> header */
    fputs("<list> ", file);
    fputs(word->word, file);
    fputs("\n", file);
    
    /* write each file */
    ent = word->head;
    
    while(ent != NULL)
    {
        fputs("\t", file);
        
        /* Convert filename to int */
        i = 0;
        currfile = file_list;
        
        while(currfile != NULL && strcmp(currfile->filename, ent->filename) != 0)
        {
            i++;
            currfile = currfile->next;
        }
        
        /* Validate that the file was found */
        assert(currfile != NULL);
        
        sprintf(buffer, "%i: ", i);
        
        fputs(buffer, file);
        
        /* Convert the frequency to a string */
        sprintf(buffer, "%i", ent->frequency);
        
        fputs(buffer, file);
        fputs("\n", file);
        
        ent = ent->next;
    }
    
    /* close the </list> */
    fputs("</list>\n", file);
    return 1;
}


int main( int argc, char** argv )
{    
    int i, res;
    Entry ent, next;
    void* ptr;
    Word word;
    SortedListT wordList;
    SortedListIterT iter;
    FILE *index;
    
    /* Validate the inputs */
    if( (argc == 2 && argv[1][0] == '-' && argv[1][1] == 'h') || argc < 3 )
    {
        fprintf(stderr, "Usage: %s <inverted-index filename> <file or directory>\n", argv[0]);
        return 1;
    }
    
    /* By default, set wordList = NULL */
    wordList = NULL;
    
    /* Create a HashTable to hold our entries. We are setting the destroy value
    method = NULL because we will use it once the sortedList is destroyed */
    wordTable = createHT(hash, compStrings, destroyString, NULL, printWordHT);
    assert(wordTable != NULL);
    
    if(DEBUG) printf("main: Created HashTable.\n");
    
    /* Set file_list = NULL because the list starts out empty */
    file_list = NULL;
    
    /* Recursivly walk through each file in a directory and tokenize */    
    ftw(argv[2], plist, 1);
    
    /* Print out the HT */
    if(DEBUG) toStringHT(wordTable);
    
    if(DEBUG)
    {
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
    }
    
    /* Convert our HT to a Sorted List */
    wordList = HTtoSL(wordTable);
    assert(wordList != NULL);
    
    /* Create the new index file */
    index = fopen(argv[1], "w");
    assert(index != NULL);
    
    res = indexFiles(index, file_list);
    assert(res != 0);
    
    /* Get ready to iterate over the table */
    iter = SLCreateIterator(wordList);
    i = 0;
    
    while((SLNextItem(iter, &ptr) == 1))
    {
        word = (Word) ptr;
        
        if(DEBUG) printf("[%i]: %s\n", i, word->word);
        
        res = indexWord(index, word);
        assert(res != 0);
        
        i++;
    }
    
    /* Close the file */
    fclose(index);
    index = NULL;
    
    /* Now we're done with the iterator, goodbye. */
    SLDestroyIterator(iter);
    iter = NULL;
    
    /* We're done with our HT, destroy it */
    destroyHT(wordTable);
    wordTable = NULL;

    /* We're done with our Sorted List, destroy it */
    SLDestroy(wordList);
    wordList = NULL; 
    
    /* We're done with our filelist as well, destroy that too */
    ent = file_list;
    
    while(ent != NULL)
    {
        next = ent->next;
        free(ent->filename);
        free(ent);
        ent = next;
    }
    
    return 1;
}
