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
 
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "hashtable.h"
#include "words.h"
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
    unsigned long long max_size;
    unsigned long long curr_size;
    HashTable table;
};


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
    if(cache != NULL)
    {
        
        free(cache);
    }
}






