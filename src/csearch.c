/*
 * File: csearch.c
 *
 * Author: Mike Swift
 * Email: theycallmeswift@gmail.com
 * Date Created: May 4th, 2011
 * Date Modified: May 4th, 2011
 */
 
/****************************
 * 1. Includes              *
 ****************************/

#include "csearch.h"

/****************************
 * 2. Structs               *
 ****************************/



/****************************
 * 3. File List Functions   *
 ****************************/
 
/* getFilelist
 *
 * This function takes in a tokenizer object that points
 * to the start of an inverted index. It then parses the 
 * list of files between <files> and </files> and returns
 * an object that contains the total number of files, an
 * array that maps a number to the filename, and an array 
 * of integers that will hold the search results.
 *
 * @param   tok         Tokenizer object (pointing to top of inverted index)
 * 
 * @return  success     Filelist
 * @return  failure     NULL
 */

Filelist getFilelist(TokenizerT tok)
{
    Filelist files;
    char **file_list, *str;
    int counter, numfiles;
    
    /* Validate inputs */
    
    if(tok == NULL)
    {
        fprintf(stderr, "Error: Cannot generate filelist from NULL tokenizer.\n");
        return NULL;
    }
    
    /* Make sure we are looking at the top of the file */
    
    str = TKGetNextToken(tok);
    if(str == 0 || strcmp(str, "files") != 0)
    {
        fprintf(stderr, "Error: Malformed index file.\n");
        exit(-1);
    }
    free(str);
    
    /* get the total number of files */
    
    str = TKGetNextToken(tok);
    numfiles = atoi(str);
    free(str);
    
    if(DEBUG) printf("Total Files: %i\n", numfiles);
    
    /* Allocate space for the file array */
    
    file_list = (char**) malloc(sizeof(char*) * numfiles);
    if(file_list == NULL)
    {
        fprintf(stderr, "Error: Could not allocate space for file list.\n");
        return NULL;
    }
    
    /* Parse the list of files */
    
    counter = 0;
    
    while((str = TKGetNextToken(tok)) != 0 && strcmp(str, "/files") != 0)
    {
        /* First str is just the int index (equal to counter) */
        free(str);
        str = TKGetNextToken(tok);
        
        file_list[counter] = (char*) malloc(sizeof(char) * (strlen(str) + 1));
        if(file_list[counter] == NULL)
        {
            fprintf(stderr, "Error: Could not allocate space for filename.\n");
            return NULL;
        }
        strcpy(file_list[counter], str);
        
        free(str);
        
        counter++;
    }
    
    free(str);
    
    /* Now allocate the actual struct */
    
    files = (Filelist) malloc(sizeof(struct Filelist_));
    if(files == NULL)
    {
        fprintf(stderr, "Error: Could not allocate space for filelist.\n");
        return NULL;
    }
    
    files->list = file_list;
    files->numfiles = numfiles;
    
    files->results = NULL;
    
    return files;
}

/* destroyFilelist
 *
 * Function that destroys a filelist object.
 *
 * @param   files       pointer to filelist
 *
 * @return  void
 */

void destroyFilelist(Filelist files)
{
    int counter;
    
    if(files != NULL)
    {
        for(counter = 0; counter < files->numfiles; counter++)
        {
            free(files->list[counter]);
        }
        
        free(files->list);
        resetResults(files);
        
        free(files);
    }
}

/* resetResults
 *
 * Resets the results after a search has been completed.
 *
 * @param   files       filelist object
 *
 * @return  void
 */
 
void resetResults(Filelist files)
{
    Result curr, next;
    
    curr = files->results;
    while(curr != NULL)
    {
        next = curr->next;
        
        free(curr);
        
        curr = next;
    }
    
    files->results = NULL;
}

/* sortResults
 *
 * Sorts the results in order of frequency (Eventually this
 * will be score)
 *
 * @param   files       filelist object
 *
 * @return  void
 */

void sortResults(Filelist files)
{
    Result curr, next, sorted, scurr, sprev;
    int inserted;
    double diff;
    
    curr = files->results;
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
            diff = curr->score - scurr->score;
            
            if(diff >= 0.0)
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
    
    files->results = sorted;

}

/* scoreFile
 *
 * Function that computes the Term frequency and inverse document frequency
 *
 * @param   totalFiles              the total number of files in the index
 * @param   filescontword           total number of files containing the word
 * @param   freq                    frequency of the word in the document in question
 *
 * @return  double                  score
 */

double scoreFile(int totalFiles, int filescontword, int freq)
{
    /* N = Total files, Nt = Number of files containing the result, Ft = frequency in the file */
    int Nt, N, Ft;
    double IDFt, TFt;
    
    Nt = totalFiles;
    N = filescontword;
    Ft = freq;
    
    
    IDFt = log((1.0 + ((double)N / Nt)));
    TFt = 1 + log((double)Ft);
    
    if(DEBUG) printf("%f = %f * %f\n",IDFt * TFt, IDFt, TFt);
    
    return IDFt * TFt;
}

/* getWord
 *
 * This is the function that is responsible for retriving the
 * Word objects from the inverted index. The first method of 
 * searching is by specifying a specific searchterm.  The function
 * walks through the tokenizer until it either finds the term and 
 * parses it into a word object or it reaches the end of the list.
 * If the end is reached, it resets the list to the top and continues
 * until it reaches the first term it parsed. If you pass in NULL as 
 * the search term, the function will return the next word it
 * encounters in the list. If the term is not encountered in the 
 * list or an error occurs, the function returns NULL.
 *
 * @param   tok           Tokenizer pointing to a <list> element in an inverted index
 * @param   searchterm    Either term to search for or NULL
 *
 * @return  success       Word
 * @return  failure       NULL
 */  

Word getWord(TokenizerT tok, char* searchterm)
{
    Word word;
    Entry ent;
    char *str, *start;
    int res, cont, reset, filenum, frequency;
    
    start = NULL;
    ent = NULL;
    cont = 1;
    reset = 0;
    
    while(cont)
    {
        str = TKGetNextToken(tok);
        if(str == 0)
        {
            if(DEBUG) printf("Reseting list!\n");
            adjustAllowedChars(tok, FILE_CHARS);
    
            TKReset(tok);
            
            /* Skip the files */
            str = TKGetNextToken(tok);
            if(str == 0 || strcmp(str, "files") != 0)
            {
                fprintf(stderr, "Error: Malformed index file.\n");
                exit(-1);
            }
            free(str);
            
            while((str = TKGetNextToken(tok)) != 0 && strcmp(str, "/files") != 0)
            {
                free(str);
            }
            free(str);
            
            adjustAllowedChars(tok, STRING_CHARS);
            str = TKGetNextToken(tok);
            
            /* Set the reset flag to TRUE */
            reset = 1;
        }
        
        /* Verify that we started on a list */
        if(str == 0 || strcmp(str, "list") != 0)
        {
            fprintf(stderr, "Error: Malformed index file.\n");
            exit(-1);
        }
        
        free(str);
        
        /* Next get the term */
        str = TKGetNextToken(tok);
        if(str == 0)
        {
            fprintf(stderr, "Error: Malformed index file.\n");
            exit(-1);
        }
        
       
        res = 0;
        if(searchterm != NULL)
        {
            res = strcmp(searchterm, str);
        }
        
        if(res == 0)
        {
            
            /* Found it */
            if(DEBUG) printf("Found Term: %s\n", str);
            
            word = createWord(str);
            if(word == NULL)
            {
                fprintf(stderr, "Error: Could not create word.\n");
                return NULL;
            }
            
            free(str);
            
            /* Get the total files */
            str = TKGetNextToken(tok);
            word->numFiles = atoi(str);
            free(str);
            
            /* Move on */
            while((str = TKGetNextToken(tok)) != 0 && strcmp(str, "list") != 0)
            {
                filenum = atoi(str);
                free(str);
                
                str = TKGetNextToken(tok);
                frequency = atoi(str);
                word->totalAppearances += frequency;
                free(str);
                
                if(ent == NULL)
                {
                    word->head = createEntry(NULL, filenum, frequency);
                    ent = word->head;
                }
                else
                {
                    ent->next = createEntry(NULL, filenum, frequency);
                    ent = ent->next;
                }
            }
            free(str);
            free(start);
            
            return word;
        }
        else
        {
            if(start == NULL)
            {
                start = str;
            }
            else
            {
                if(strcmp(start, str) == 0)
                {
                    if(DEBUG) printf("Looped back to %s\n", start);
                    cont = 0;
                    free(start);
                }
                free(str);
            }
            /* Move on */
            while((str = TKGetNextToken(tok)) != 0 && strcmp(str, "list") != 0)
            {
                free(str);
            }
            free(str);
        }
    }
    
    if(DEBUG) printf("Reached the end of the list.\n");
    
    return NULL;
    
}

/* search
 *
 * This function searchs for all the terms entered by the user.
 * It first checks the cache to see if the term in question is
 * present, and if not it searches the index file for the word.
 * It creates a linked list of results and then sorts them based
 * on score and the logical operation being performed.
 *
 * @param   action          string containing the search type and terms
 * @param   tok             tokenizer object
 * @param   files           filelist object
 * @param   cache           Cache object
 *
 * @return  void
 */

void search(char* action, TokenizerT tok, Filelist files, Cache cache)
{    
    char term[1024];
    int acounter, tcounter, numterms, cont, stype, rfound;
    Word found;
    Entry ent;
    Result result;
    
    result = NULL;
    acounter = 3;
    tcounter = 0;
    numterms = 0;
    stype = 0;
    
    /* stype (0 = Logical OR, 1 = Logical AND) */
    if(action[1] == 'a')
    {
        stype = 1;
    }
    
    cont = 1;
    
    while(cont)
    {
        if(action[acounter] == ' ' || action[acounter] == '\0' || action[acounter] == '\n')
        {
            if(tcounter > 0)
            {
                term[tcounter] = '\0';
                
                numterms++;
                
                found = searchCache(cache, term);
                if(found == NULL)
                {
                    found = getWord(tok, term);
                    if(found != NULL) insertWord(cache, found);
                }
                else
                {
                    if(DEBUG) printf("Found %s in cache.\n", term);
                }
                
                if(found != NULL)
                {
                    if(DEBUG) printWord(found);
                                        
                    ent = found->head;
                    
                    while(ent != NULL)
                    {
                        if(files->results == NULL)
                        {
                            files->results = (Result) malloc(sizeof(struct Result_));
                            if(files->results == NULL)
                            {
                                fprintf(stderr, "Error: Could not allocate space for results.\n");
                                return;
                            }
                            result = files->results;
                            result->filenum = ent->filenumber;
                            result->numfiles = 1;
                            result->frequency = ent->frequency;
                            result->score = scoreFile(files->numfiles, found->numFiles, ent->frequency);
                            result->next = NULL;
                        }
                        else
                        {
                            rfound = 0;
                            result = files->results;
                            while(result != NULL)
                            {
                                if(result->filenum == ent->filenumber)
                                {
                                    /* Found it */
                                    result->numfiles++;
                                    result->frequency += ent->frequency;
                                    result->score += scoreFile(files->numfiles, found->numFiles, ent->frequency);
                                    rfound = 1;
                                }
                                
                                if(result->next == NULL && rfound == 0)
                                {
                                    result->next = (Result) malloc(sizeof(struct Result_));
                                    if(result->next == NULL)
                                    {
                                        fprintf(stderr, "Error: Could not allocate space for results.\n");
                                        return;
                                    }
                                    result = result->next;
                                    result->filenum = ent->filenumber;
                                    result->numfiles = 1;
                                    result->frequency = ent->frequency;
                                    result->score = scoreFile(files->numfiles, found->numFiles, ent->frequency);
                                    result->next = NULL;
                                }
                                result = result->next;
                            }
                            
                        }
                        
                        ent = ent->next;
                    }
                }
            }
            /* always set the tcounter to 0 */
            tcounter = 0;
            
            if(action[acounter] == '\0' || action[acounter] == '\n')
            {
                cont = 0;
            }
        }
        else
        {
            term[tcounter] = action[acounter];
            tcounter++;
        }
        acounter++;
    }
    
    sortResults(files);
    
    result = files->results;
    while(result != NULL)
    {
        if(stype == 0 || (stype == 1 && result->numfiles == numterms))
        {
            /* printf("%s\n", files->list[result->filenum]); */
        }
        else
        {
            result->frequency = -1;
        }
        result = result->next;
    }

}


int runsearch( int argc, char** argv )
{
    Cache cache;
    TokenizerT tok;
    int counter;
    char *cachesize, action[1024];
    Filelist files;
    Result result;
    
    /* Check for the help flag */
    if(argc >= 2 && argv[1][0] == '-' && argv[1][1] == 'h')
    {
        fprintf(stderr, "Usage: %s <inverted-index filename>\n", argv[0]);
        return 1;
    }
    
    cachesize = DEFAULT_CACHE_SIZE;
    
    /* Parse any flags */
    if(argc > 2)
    {
        for(counter = 1; counter < argc; counter++)
        {
            if(argv[counter][0] == '-')
            {
                if(argv[counter][1] == 'm')
                {
                    if(DEBUG) printf("Cache Limit: %s.\n", argv[counter+1]);
                    
                    cachesize = argv[counter+1];
                }
            }
        }
    }
    
    /* Create a tokenizer */
    tok = TKCreate(FILE_CHARS, argv[argc-1]);
    if(tok == NULL)
    {
        fprintf(stderr, "Error: Could not allocate space for Tokenizer.\n");
        return 0;
    }
    
    if(DEBUG) printf("Getting files\n");
    
    /* Get the file list */
    files = getFilelist(tok);
    if(files == NULL)
    {
        return 0;
    }
    
    /* Create a cache */
    cache = createCache(cachesize);
    if(cache == NULL)
    {
        fprintf(stderr, "Error: Could not allocate space for Cache.\n");
        return 0;
    }
    
    /* Update the allowed characters */
    adjustAllowedChars(tok, STRING_CHARS);
    
    /* Main Loop */
    printf("search> ");
    fgets(action, 1024, stdin);
        
    while(action[0] != 'q')
    {
        if(action[0] == 's' && (action[1] == 'o' || action[1] == 'a'))
        {
            search(action, tok, files, cache);
        }
        else
        {
            printf("Command not found.\n");
        }
        
        result = files->results;
        while(result != NULL)
        {
            if(result->frequency > 0)
            {
                printf("%s\n", files->list[result->filenum]);
            }
            result = result->next;
        }
        
        resetResults(files);
        
        printf("search> ");
        fgets(action, 1024, stdin);
    } 
    
    /* Ok, now we're done. Burn it down */
    destroyCache(cache);
    cache = NULL;
    
    destroyFilelist(files);
    files = NULL;
    
    TKDestroy(tok);
    tok = NULL;
    
    return 1;
}
