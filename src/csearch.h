/*
 * File: csearch.h
 *
 * Author: Mike Swift
 * Email: theycallmeswift@gmail.com
 * Date Created: May 4th, 2011
 * Date Modified: May 4th, 2011
 */
 
#ifndef SWIFT_CSEARCH_H_
#define SWIFT_CSEARCH_H_

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "cache.h"
#include "tokenizer.h"
#include "words.h"

/********************************
 * 1. Constants                 *
 ********************************/
 
#define DEBUG 0

#define STRING_CHARS "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890"
#define FILE_CHARS "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890 /\\-_()[]."

#define DEFAULT_CACHE_SIZE "0KB"

/********************************
 * 2. Typedefs & Structs        *
 ********************************/

struct Result_;
typedef struct Result_* Result;

struct Filelist_;
typedef struct Filelist_* Filelist;


struct Result_ {
    int filenum;
    int frequency;
    int numfiles;
    double score;
    Result next;
};

struct Filelist_ {
    char** list;
    Result results;
    int numfiles;
};

/********************************
 * 3. File List Functions       *
 ********************************/
 
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

Filelist getFilelist(TokenizerT tok);

/* destroyFilelist
 *
 * Function that destroys a filelist object.
 *
 * @param   files       pointer to filelist
 *
 * @return  void
 */

void destroyFilelist(Filelist files);

/* resetResults
 *
 * Resets the results after a search has been completed.
 *
 * @param   files       filelist object
 *
 * @return  void
 */
 
void resetResults(Filelist files);

/* sortResults
 *
 * Sorts the results in order of frequency (Eventually this
 * will be score)
 *
 * @param   files       filelist object
 *
 * @return  void
 */

void sortResults(Filelist files);

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

double scoreFile(int totalFiles, int filescontword, int freq);

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

Word getWord(TokenizerT tok, char* searchterm);

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
 *
 * @return  void
 */

void search(char* action, TokenizerT tok, Filelist files, Cache cache);

/* Driver */
int runsearch( int argc, char** argv );

#endif
/* SWIFT_CSEARCH_H_ */
 
