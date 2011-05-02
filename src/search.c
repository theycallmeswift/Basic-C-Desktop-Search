/*
 * File: search.c
 *
 * Author: Mike Swift
 * Email: theycallmeswift@gmail.com
 * Date Created: March 24th, 2011
 * Date Modified: March 24th, 2011
 *
 * Description:
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <setjmp.h>
#include "hashtable.h"
#include "tokenizer.h"
#include "search.h"

/* Gloabal Variables */
char *filenames[1024];

/* Takes in a string, produces a hash */
unsigned long hash(const char *key)
{
	int h=0;
	while(*key) h=33*h + *key++;
	return h;
}

/*
 * Takes in an int error code and prints the corresponding error to stderr.
 */

void throwError(int errorCode)
{
    /* A little HTTP Request Humor with my Errors */

    switch(errorCode)
    {
        case 400:
            fprintf(stderr, "Error: 400 Bad Request\n");
            break;
        case 401:
            fprintf(stderr, "Error: 401 Unauthorized\n");
            break;
        case 403:
            fprintf(stderr, "Error: 403 Forbidden\n");
            break;
        case 404:
            fprintf(stderr, "Error: 404 Not Found\n");
            break;
        case 405:
            fprintf(stderr, "Error: 405 Method Not Allowed\n");
            break;
        case 406:
            fprintf(stderr, "Error: 406 Not Acceptable\n");
            break;
        case 408:
            fprintf(stderr, "Error: 408 Request Timeout\n");
            break;
        case 409:
            fprintf(stderr, "Error: 409 Conflict\n");
            break;
        case 410:
            fprintf(stderr, "Error: 410 Gone\n");
            break;
        case 412:
            fprintf(stderr, "Error: 412 Precondition Failed\n");
            break;
        case 413:
            fprintf(stderr, "Error: 413 Request Entity Too Large\n");
            break;
        case 415:
            fprintf(stderr, "Error: 415 Unsupported Media Type\n");
            break;
        case 417:
            fprintf(stderr, "Error: 417 Expectation Failed\n");
            break;
        case 503:
            fprintf(stderr, "Error: 503 Service Unavailable\n");
            break;
        default:
            fprintf(stderr, "Error: 500 Internal Server Error\n");
            break;
    }

}

void destroySearchEntry(SearchEntry ent)
{
    int i;
    
    if(ent != NULL)
    {
        free(ent->word);
        
        if(ent->files != NULL)
        {
            for(i = 0; i < ent->numFiles; i++)
            {
                free(ent->files[i]);
            }
            free(ent->files);
        }
        
        free(ent);
    }
}

void printSearchEntry(SearchEntry ent)
{
    int i;
    if(ent != NULL)
    {
        printf("Entry: %s\n", ent->word);
        printf("\tFound in: %i files.\n", ent->numFiles);
        for(i = 0; i < ent->numFiles; i++)
        {
            printf("\t\tFile #%i: %i times\n", (ent->files[i])->filenumber, (ent->files[i])->frequency);
        }
    }
}

HashTable parseIndex(char* filename)
{
    TokenizerT tok = NULL;
    char *currentToken;
    SearchEntry entry;
    FileEntry file;
    int numFilesSize, res, filenum, counter;
    
    HashTable table = NULL;
    float load;
    
    tok = TKCreate(ALLOWED_CHARS, filename);
    if( tok == NULL )
    {
        throwError(412);
        exit(-1);
    }
    
    table = createHT(8191, 2.0, hash);
    if( table == NULL )
    {
        throwError(412);
        TKDestroy(tok);
        exit(-1);
    }
    
    while((currentToken = TKGetNextToken(tok)) != 0)
    {
        
        if(strcmp(currentToken, "list") == 0)
        {
            entry = (SearchEntry) malloc( sizeof(struct SearchEntry_) );
            if(entry == NULL)
            {
                throwError(412);
                TKDestroy(tok);
                destroyHT(table);
                exit(-1);
            }
            
            free(currentToken);
            
            if((currentToken = TKGetNextToken(tok)) == 0)
            {
                destroySearchEntry(entry);
                throwError(503);
                TKDestroy(tok);
                destroyHT(table);
                exit(-1);
            }
            
            entry->word = (char*) malloc( sizeof(char) * (strlen(currentToken) + 1) );
            if(entry->word == NULL)
            {
                destroySearchEntry(entry);
                throwError(503);
                TKDestroy(tok);
                destroyHT(table);
                exit(-1);
            }
            strcpy(entry->word, currentToken);
            
            numFilesSize = 256;
            entry->files = (FileEntry*) malloc( sizeof(FileEntry) * numFilesSize );
            if(entry->files == NULL)
            {
                destroySearchEntry(entry);
                throwError(503);
                TKDestroy(tok);
                destroyHT(table);
                exit(-1);
            }
            
            free(currentToken);
            
            entry->numFiles = 0;
            
            while((currentToken = TKGetNextToken(tok)) != 0 && strcmp(currentToken, "/list") != 0)
            {
                file = (FileEntry) malloc(sizeof(struct FileEntry_));
                if( file == NULL )
                {
                    destroySearchEntry(entry);
                    throwError(503);
                    TKDestroy(tok);
                    destroyHT(table);
                    exit(-1);
                }
                file->filenumber = atoi(currentToken);
                
                free(currentToken);
                
                if((currentToken = TKGetNextToken(tok)) == 0)
                {
                    free(file);
                    destroySearchEntry(entry);
                    throwError(503);
                    TKDestroy(tok);
                    destroyHT(table);
                    exit(-1);
                }
                
                file->frequency = atoi(currentToken);
                
                free(currentToken);
                
                entry->files[entry->numFiles] = file;
                entry->numFiles++;
            }
            res = insertHT(table, entry->word, (genericItem) entry);
            load = (float) table->totalNodes/table->numSlots;
            if( load > table->loadLimit )
            {
                table = rehash( table );

                if( table == NULL )
                {
                    destroySearchEntry(entry);
                    throwError(503);
                    TKDestroy(tok);
                    destroyHT(table);
                    exit(-1);
                }
            }
            
        }
        else if(strcmp(currentToken, "files") == 0)
        {
            adjustAllowedChars(tok, ALLOWED_FILE_CHARS);
            free(currentToken);
            while((currentToken = TKGetNextToken(tok)) != 0 && strcmp(currentToken, "/files") != 0)
            {
                filenum = currentToken[0] - '0';
                filenames[filenum] = (char *) malloc( sizeof(char) * (strlen(currentToken) + 1) );
                if(filenames[filenum]  == NULL)
                {
                    destroySearchEntry(entry);
                    throwError(503);
                    TKDestroy(tok);
                    destroyHT(table);
                    exit(-1);
                }
                for(counter = 2; counter < strlen(currentToken) + 1; counter++)
                {
                    filenames[filenum][counter - 2] = currentToken[counter];
                }
                
                free(currentToken);
            }
            
        }
        
        free(currentToken);
    }
    
    
    TKDestroy(tok);
    return table;
}

void orSearch(char* action, HashTable table)
{
    int foundfiles[1024];
    char term[1024];
    int i, acounter, tcounter;
    
    SearchEntry ent;

    acounter = 3;
    tcounter = 0;
    
    for(i = 0; i < 1024; i++)
    {
        foundfiles[i] = -1;
    }
    
    while(action[acounter] != '\0' && action[acounter] != '\n')
    {
        if(action[acounter] == ' ')
        {
            if(tcounter != 0)
            {
                term[tcounter] = '\0';
                if((ent = (SearchEntry)getHT(table, term)) != NULL)
                {
                    for(i = 0; i < ent->numFiles; i++)
                    {
                        if(foundfiles[(ent->files[i])->filenumber] == -1)
                        {
                            foundfiles[(ent->files[i])->filenumber] = 1;
                        }
                    }
                }
            }
            tcounter = 0;
        }
        else
        {
            term[tcounter] = action[acounter];
            tcounter++;
        }
        acounter++;
    }
    if(tcounter != 0)
    {
        term[tcounter] = '\0';
        if((ent = (SearchEntry)getHT(table, term)) != NULL)
        {
            for(i = 0; i < ent->numFiles; i++)
            {
                if(foundfiles[(ent->files[i])->filenumber] == -1)
                {
                    foundfiles[(ent->files[i])->filenumber] = 1;
                }
            }
        }
    }
    
    for(i = 0; i < 1024; i++)
    {
        if(foundfiles[i] != -1)
        {
            printf("%s\n", filenames[i], i);
        }
    }
}


void andSearch(char* action, HashTable table)
{
    int foundfiles[1024];
    char term[1024];
    int i, acounter, tcounter, numterms;
    
    SearchEntry ent;

    acounter = 3;
    tcounter = 0;
    numterms = 0;
    
    for(i = 0; i < 1024; i++)
    {
        foundfiles[i] = -1;
    }
    
    while(action[acounter] != '\0' && action[acounter] != '\n')
    {
        if(action[acounter] == ' ')
        {
            if(tcounter != 0)
            {
                term[tcounter] = '\0';
                if((ent = (SearchEntry)getHT(table, term)) != NULL)
                {
                    for(i = 0; i < ent->numFiles; i++)
                    {
                        if(foundfiles[(ent->files[i])->filenumber] == -1)
                        {
                            if(numterms == 0)
                            {
                                foundfiles[(ent->files[i])->filenumber] = 1;
                            }
                        }
                        else
                        {
                            foundfiles[(ent->files[i])->filenumber]++;
                        }
                       
                    }
                    numterms++;
                }
            }
            tcounter = 0;
        }
        else
        {
            term[tcounter] = action[acounter];
            tcounter++;
        }
        acounter++;
    }
    if(tcounter != 0)
    {
        term[tcounter] = '\0';
        if((ent = (SearchEntry)getHT(table, term)) != NULL)
        {
            for(i = 0; i < ent->numFiles; i++)
            {
                if(foundfiles[(ent->files[i])->filenumber] == -1)
                {
                    if(numterms == 0)
                    {
                        foundfiles[(ent->files[i])->filenumber] = 1;
                    }
                }
                else
                {
                    foundfiles[(ent->files[i])->filenumber]++;
                }
            }
            numterms++;
        }
    }

    for(i = 0; i < 1024; i++)
    {
        if(foundfiles[i] != -1 && foundfiles[i] == numterms)
        {
            printf("%s\n", filenames[i], i);
        }
    }
}


int main( int argc, char** argv )
{
    int catch, i;
    jmp_buf jump;
    HashTable table;
    HashIterator hiter;
    SearchEntry ent;    
    char action[1024];
    
    catch = setjmp(jump);
    
    if( argc != 2 )
    {
        printf("Usage: %s <index file>\n", argv[0]);
        exit(-1);
    }

    if( catch == 0 )
    {
        table = parseIndex(argv[1]);
        hiter = createIteratorHT( table );
        
        printf("search> ");
        fgets(action, 1024, stdin);
        
        while(action[0] != 'q')
        {
            if(action[0] == 's' && action[1] == 'o')
            {
                orSearch(action, table);
            }
            else if(action[0] == 's' && action[1] == 'a')
            {
                andSearch(action, table);
            }
            else
            {
                printf("Command not found.\n");
            }
            
            
            printf("search> ");
            fgets(action, 1024, stdin);
        }
        
        while( nextRowIteratorHT(hiter) > 0 )
        {
            while( (ent = (SearchEntry)nextItemIteratorHT(hiter)) != NULL)
            {
                /* printSearchEntry(ent); */
                destroySearchEntry(ent);
            }
        }
        destroyIteratorHT(hiter);
        destroyHT(table);
        for(i = 0; i < 1024; i++)
        {
            free(filenames[i]);
        }
    }
    else
    {
        throwError(catch);
        return 0;
    }
    return 1;
}
