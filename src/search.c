/*
 * File: search.c
 *
 * Author: Mike Swift
 * Email: theycallmeswift@gmail.com
 * Date Created: May 3rd, 2011
 * Date Modified: May 3rd, 2011
 */

/********************************
 *          1. Includes         *
 ********************************/

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "search.h"

/********************************
 *          2. Globals          *
 ********************************/

Entry file_list;


/********************************
 *      3. Helper Functions     *
 ********************************/
 
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

HashTable parseIndex(char* filename)
{
    HashTable table;
    TokenizerT tok;
    char* str;
    int res;
    Entry ent, curr;
    Word word;
    
    if(filename == NULL)
    {
        fprintf(stderr, "Error: Cannot parse NULL file.\n");
        return NULL;
    }
    
    table = createHT(hash, compStrings, destroyString, destroyWord, printWordHT);
    if(table == NULL)
    {
        fprintf(stderr, "Error: Could not allocate space for HashTable.\n");
        return NULL;
    }
    
    tok = TKCreate(FILE_CHARS, filename);
    if(tok == NULL)
    {
        fprintf(stderr, "Error: Could not allocate space for Tokenizer.\n");
        return NULL;
    }
    
    str = TKGetNextToken(tok);
    res = strcmp(str, "files");
    free(str);
    
    if(res != 0)
    {
        fprintf(stderr, "Error: Malformed index file.\n");
        return NULL;
    }
        
    /* Parse the file list */
    while((str = TKGetNextToken(tok)) != 0 && strcmp(str, "/files") != 0)
    {
        free(str);
        str = TKGetNextToken(tok);
        
        if(str == 0 || strcmp(str, "/files") == 0)
        {
            fprintf(stderr, "Error: Malformed index file.\n");
            return NULL;
        }
        
        ent = createEntry(str);
        
        if(file_list == NULL)
        {
            file_list = ent;
            curr = ent;
        }
        else
        {
            curr->next = ent;
            curr = curr->next;
        }
        
        free(str);
    }
    
    free(str);
    
    /* Now lets make a HashTable */
    adjustAllowedChars(tok, STRING_CHARS);
    
    while((str = TKGetNextToken(tok)) != 0)
    {
        /* list identifier */
        if(strcmp(str, "list") != 0)
        {
            fprintf(stderr, "Error: Malformed index file.\n");
            return NULL;
        }
        
        free(str);
        
        /* Next the word */
        str = TKGetNextToken(tok);
        
        word = createWord(str);
        res = insertHT(table, (void*)str, (void*)word);
        if(res == 0)
        {
            fprintf(stderr, "Error: Could not insert word into HashTable.\n");
            return NULL;
        }
        
        /* Now loop through the file entries */
        while((str = TKGetNextToken(tok)) != 0 && strcmp(str, "list") != 0)
        {
            printf("%s ", str);
            free(str);
        }
        
        free(str);
        printf("\n");
    }
    
    TKDestroy(tok);
    tok = NULL;
    
    return table;
}

int main( int argc, char** argv )
{
    HashTable table;
    Entry curr, next;
    int i;
    
    /* Validate the inputs */
    if( (argc == 2 && argv[1][0] == '-' && argv[1][1] == 'h') || argc != 2 )
    {
        fprintf(stderr, "Usage: %s <inverted-index filename>\n", argv[0]);
        return 1;
    }
    
    file_list = NULL;
    
    table = parseIndex(argv[1]);
    assert(table != NULL);
    
    curr = file_list;
    i = 0;
    
    while(curr != NULL)
    {
        next = curr->next;
        
        printf("[%i]: %s\n", i, curr->filename);
        
        free(curr->filename);
        free(curr);
        
        i++;
        curr = next;
    }
    
    destroyHT(table);
    table = NULL;
    
    return 1;
}
