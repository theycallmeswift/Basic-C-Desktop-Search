/*
 * File: tokenizer.c
 *
 * Author: Mike Swift
 * Email: theycallmeswift@gmail.com
 * Date Created: February 27th, 2011
 * Date Modified: March 9th, 2011
 *
 * Description:
 * Takes in the name of a file and returns the tokens (strings) from it.
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "tokenizer.h"


void TKDebug( char *fn, int list )
{
    FILE *file;
    char c;
    int i;

    printf("*** DEBUG START ***\n");
    file = fopen(fn, "rb");
    if (file == NULL) {
        printf("Failed to open %s\n", fn);
        exit(-1);
    }

    i = 0;
    while ((c = (char) fgetc(file)) != EOF)
    {
        if(list > 0)
        {
            if(c == '\0')
            {
                printf("%i: \\0\n", i);
            }
            else
            {
                printf("%i: %c\n", i, c);
            }
        }
        i++;
    }
    printf("File: \"%s\" (%i characters) \n", fn, i - 1);
    fclose(file);
    printf("*** DEBUG END ***\n");
}


/*
 * TKCreate creates a new TokenizerT object.
 *     tc: a string containing the characters that can be in a token.
 *     fn: name of the file that should be read and tokenized.
 *
 * TKCreate will open the file and cache the FILE object.  If it cannot
 * open the given file, it will return an error.
 *
 * If the function succeeds, it returns a non-NULL TokenizerT.
 * Else it returns NULL.
 */
TokenizerT TKCreate(char *tc, char *fn)
{
    TokenizerT tok;
    FILE *file;

    if(fn == NULL)
    {
        return NULL;
    }

    file = fopen(fn, "rb");
    if (file == NULL) {
        fprintf(stderr, "Error: Failed to open %s\n", fn); 
        return NULL;
    }

    tok = (TokenizerT) malloc( sizeof( struct TokenizerT_ ) );

    if( tok == NULL )
	{
		printf("ERROR: Malloc failed");
		exit(-1);
	}

    tok->file = file;
    tok->allowedCharacters = (char *) malloc( sizeof( char ) * ( strlen(tc) + 1 ) );

    if( tok->allowedCharacters == NULL )
    {
        printf("ERROR: Malloc failed");
		exit(-1);
    }
    
    strcpy( tok->allowedCharacters, tc );
    
    tok->filename = (char*) malloc(sizeof(char) * (strlen(fn) + 1));
    if(tok->filename == NULL)
    {
        fprintf(stderr, "ERROR: Malloc failed.\n");
        exit(-1);
    }
    strcpy(tok->filename, fn);

    return tok;
}

/* TKReset
 *
 * Resets the tokenizer to the start of the current file.
 *
 * @param   tok         Tokenizer object
 *
 * @return  void
 */

void TKReset(TokenizerT tok)
{
    rewind(tok->file);
}

/* adjustAllowedChars
 *
 * Function that resets the allowed characters to whatever the 
 * user specifies.
 *
 * @param   tok         Tokenizer object
 * @param   allowed     String of allowed characters
 *
 * @return  void
 */

void adjustAllowedChars(TokenizerT tok, char* allowed)
{
    free(tok->allowedCharacters);
    
    tok->allowedCharacters = (char *) malloc( sizeof( char ) * ( strlen(allowed) + 1 ) );
    
    if( tok->allowedCharacters == NULL )
    {
        printf("ERROR: Malloc failed");
		exit(-1);
    }

    strcpy( tok->allowedCharacters, allowed );
}

/*
 * TKDestroy destroys a TokenizerT object.  It should free all dynamically
 * allocated memory that is part of the object being destroyed.
 *
 * You need to fill in this function as part of your implementation.
 */

void TKDestroy(TokenizerT tk)
{
    if(tk != NULL)
    {
        if(tk->file != NULL)
        {
            fclose(tk->file);
        }

        if(tk->allowedCharacters != NULL)
        {
            free(tk->allowedCharacters);
        }
        
        if(tk->filename != NULL)
        {
            free(tk->filename);
        }

        free(tk);
    }
}


/*
 * TKGetNextToken returns the next token from the token stream as a
 * character string.  Space for the returned token should be dynamically
 * allocated.  The caller is responsible for freeing the space once it is
 * no longer needed.
 *
 * If the function succeeds, it returns a C string (delimited by '\0')
 * containing the token.  Else it returns 0.
 *
 * You need to fill in this function as part of your implementation.
 */

char *TKGetNextToken(TokenizerT tk)
{
    int characterCounter, bufferSize, bufferLocation;
    char *buffer, *bufferPtr, c;

    if(tk == NULL)
    {
        printf("ERROR: Must pass in a valid TokenizerT to TKGetNextToken");
		exit(-1);
    }

    if(tk->file == NULL)
    {
        return 0;
    }

    bufferSize = 256;

    buffer = (char *) malloc( sizeof(char) * bufferSize );
    bufferLocation = 0;

    while ((c = (char) fgetc(tk->file)) != EOF)
    {
        if(bufferLocation >= bufferSize)
        {
            if( bufferSize >= MAX_BUFFER_SIZE )
            {
                /* printf("String too big for Buffer!\n");*/
                free(buffer);
                return 0;
            }

            bufferSize *= 2;

            bufferPtr = buffer;

            buffer = (char *) malloc( sizeof(char) * bufferSize );
            strcpy(buffer, bufferPtr);

            free(bufferPtr);
            bufferPtr = NULL;
        }

        characterCounter = 0;
        while(tk->allowedCharacters[characterCounter] != c && tk->allowedCharacters[characterCounter] != '\0')
        {
            characterCounter++;
        }

        if(tk->allowedCharacters[characterCounter] == '\0')
        {
            if(bufferLocation != 0)
            {
                buffer[bufferLocation] = '\0';
                return buffer;
            }
            bufferLocation = 0;
        }
        else
        {
            buffer[bufferLocation] = tolower(c);
            bufferLocation++;
        }
    }
    free(buffer);
    return 0;
}

