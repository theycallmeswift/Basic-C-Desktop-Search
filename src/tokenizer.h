/*
 * File: tokenizer.h
 *
 * Author: Mike Swift
 * Email: theycallmeswift@gmail.com
 * Date Created: March 9th, 2011
 * Date Modified: March 9th, 2011
 *
 * Description:
 * This is the header file for the tokenizer made during CS214.
 */

#ifndef SWIFT_TOKENIZER_H_
#define SWIFT_TOKENIZER_H_

#define MAX_BUFFER_SIZE 1024

struct TokenizerT_
{
    FILE *file;
    char *filename;
    char *allowedCharacters;
};
typedef struct TokenizerT_* TokenizerT;

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
TokenizerT TKCreate(char *tc, char *fn);

/*
 * TKDestroy destroys a TokenizerT object.  It should free all dynamically
 * allocated memory that is part of the object being destroyed.
 *
 */

void TKDestroy(TokenizerT tk);

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

char *TKGetNextToken(TokenizerT tk);

/* TKReset
 *
 * Resets the tokenizer to the start of the current file.
 *
 * @param   tok         Tokenizer object
 *
 * @return  void
 */

void TKReset(TokenizerT tok);

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

void adjustAllowedChars(TokenizerT tok, char* allowed);

#endif
/* END SWIFT_TOKENIZER_H_ */

