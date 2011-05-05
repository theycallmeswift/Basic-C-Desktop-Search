/*
 * File: cache.c
 *
 * Author: Mike Swift
 * Email: theycallmeswift@gmail.com
 * Date Created: May 4th, 2011
 * Date Modified: May 4th, 2011
 */
 
/****************************
 * 1. Includes              *
 ****************************/
 
#include "cache.h"

/****************************
 * 2. Structs               *
 ****************************/

struct Block_ {
    Word word;
    unsigned long long size;
    Block next;
    Block prev;
};


struct Cache_ {
    Block front;
    Block last;
    int numBlocks;
    unsigned long long max_size;
    unsigned long long curr_size;
    HashTable table;
};

/****************************
 * 3. Helper Functions      *
 ****************************/

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

/****************************
 * 4. Cache Functions       *
 ****************************/

/* createCache
 *
 * Function to create a new cache struct.  Returns the new struct on success
 * and NULL on failure.
 *
 * @param   cache_size      size of cache in bytes
 *
 * @return  success         new Cache
 * @return  failure         NULL
 */
 
Cache createCache(char* cache_size)
{
    Cache cache;
    char bytesize;
    int counter, bytes;
    
    cache = (Cache) malloc(sizeof(struct Cache_));
    if(cache == NULL)
    {
        fprintf(stderr, "Error: Could not allocate space for Cache.\n");
        return NULL;
    }

    cache->front = NULL;
    cache->last  = NULL;
    cache->curr_size = 0;
    
    counter = strlen(cache_size);
    
    bytes = atol(cache_size);
    bytesize = cache_size[counter - 2];
    
    if(bytesize == 'K')
    {
        bytes = bytes * 1024;
    }
    else if(bytesize == 'M')
    {
        bytes = bytes * 1048576;
    }
    else if(bytesize == 'G')
    {
        bytes = bytes * 1073741824;
    }
    else
    {
        fprintf(stderr, "Error: Cache size must be in either KB, MB, or GB.\n");
        return NULL;
    }
    
    cache->max_size = bytes;
    cache->numBlocks = 0;
    
    cache->table = createHT(hash, compStrings, NULL, destroyWord, printWordHT);
    
    if(CACHE_DEBUG) printf("Max Size: %llu\n", cache->max_size);
    
    return cache;
}


/* destroyCache
 * 
 * Function that destroys a created cache. Frees all allocated memory. If 
 * you pass in NULL, nothing happens. So make sure to set your cache = NULL
 * after you destroy it to prevent a double free.
 *
 * @param   cache           cache object to be destroyed
 *
 * @return  void
 */
 
void destroyCache(Cache cache)
{
    Block curr, next;
    if(cache != NULL)
    {
        
        curr = cache->front;
        while(curr != NULL)
        {
            next = curr->next;
            free(curr);
            curr = next;            
        }
        
        destroyHT(cache->table);
        free(cache);
    }
}

/* printCache
 *
 * Function that prints out a detailed summary of the cache
 *
 * @param   cache       cache to print
 *
 * @return  void
 */
void printCache(Cache cache)
{
    int i;
    Block block;
    
    if(cache != NULL)
    {
        printf("Num Blocks: %i\n", cache->numBlocks);
        printf("Max Size: %llu\n", cache->max_size);
        printf("Curr Size: %llu\n\n", cache->curr_size);
        
        block = cache->front;
        
        printf("Forwards: \n");
        for(i = 0; i < cache->numBlocks; i++)
        {
            printf("[%i]: %s\n", i, block->word->word);
            block = block->next;
        }
        
        block = cache->last;
        
        printf("Backwards: \n");
        for(i = cache->numBlocks; i > 0; i--)
        {
            printf("[%i]: %s\n", i, block->word->word);
            block = block->prev;
        }
    }
}

/* insertWord
 * 
 * Inserts a word into the cache.  If the cache is full,
 * words are cleared out 1 at a time until either there
 * are no blocks left or there is enough room for the new
 * word.
 * 
 * @param   cache           Cache object
 * @param   word            word to insert
 *
 * @return  success         1 = nothing was removed, -1 = removed
 * @return  failure         0
 */

int insertWord(Cache cache, Word word)
{
    Block block;
    unsigned long long size, temp;
    int res;
    
    if(cache == NULL)
    {
        fprintf(stderr, "Error: Cannot insert into NULL cache.\n");
        return 0;
    }
    
    if(word == NULL)
    {
        fprintf(stderr, "Error: Cannot insert NULL word.\n");
        return 0;
    }
    
    
    size = 0;
    
    /* Size of the Word Pointer */
    temp = (unsigned long long) sizeof(Word);
    size += temp;
    
    /* Size of the Word Structure */
    temp = (unsigned long long) sizeof(struct Word_);
    size += temp;
    
    /* Size of the word string */
    temp = (unsigned long long) ( sizeof(char) * (strlen(word->word) + 1) );
    size += temp;
    
    /* Size of an Entry Structure * number of entries */
    temp = (unsigned long long) (sizeof(struct Entry_) * word->numFiles);
    size += temp;
    
    if(cache->max_size != 0 && (cache->curr_size + size > cache->max_size))
    {
        if(CACHE_DEBUG) printf("%llu + %llu (%llu) > %llu\n", cache->curr_size, size, cache->curr_size + size, cache->max_size);
        
        while(cache->numBlocks != 0 && (cache->curr_size + size > cache->max_size))
        {
            block = cache->last;
            
            if(block == NULL)
            printf("Block is NULL\n");
            
            if(CACHE_DEBUG) printf("Removing %s\n", block->word->word);
            
            if(block->prev == NULL)
            {
                /* Block is at the front of the list */
                cache->last = NULL;
                cache->front = NULL;
            }
            else
            {
                cache->last = block->prev;
                cache->last->next = NULL;
            }
            
            removeHT(cache->table, block->word->word);
            
            cache->numBlocks--;
            cache->curr_size -= block->size;
            
            free(block);
        }
        
    }
    
    /* Now make the block */
    block = (Block) malloc(sizeof(struct Block_));
    if(block == NULL)
    {
        fprintf(stderr, "Error: Could not allocate space for Block.\n");
        return 0;
    }
    
    /* put the word in the HT */
    res = insertHT(cache->table, word->word, word);
    
    block->next = cache->front;
    block->prev = NULL;
    block->size = size;
    block->word = word;
    
    cache->curr_size += size;
    cache->numBlocks++;
    if(cache->front != NULL) cache->front->prev = block;
    cache->front = block;
    if(cache->last == NULL)
    {
        cache->last = block;
    }
    
    if(CACHE_DEBUG) printf("Inserted a word of size %llu Bytes.\n", size);
    return 1;
}

Word searchCache(Cache cache, char* str)
{
    Word res;
    
    if(cache == NULL)
    {
        fprintf(stderr, "Error: Cannot insert into NULL cache.\n");
        return NULL;
    }
    
    if(str == NULL)
    {
        fprintf(stderr, "Error: Cannot search for NULL string.\n");
        return NULL;
    }
    
    res = (Word) searchHT(cache->table, (void*)str);
    return res;
}


