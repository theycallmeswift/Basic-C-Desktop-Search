/*
 * File: search.h
 *
 * Author: Mike Swift
 * Email: theycallmeswift@gmail.com
 * Date Created: March 24th, 2011
 * Date Modified: March 24th, 2011
 *
 * Description:
 * This is the header for the search file.
 */

#ifndef SWIFT_SEARCH_H_
#define SWIFT_SEARCH_H_

    #define ALLOWED_CHARS "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890/"
    #define ALLOWED_FILE_CHARS "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890 /\\-_()[]."
    
    struct FileEntry_ {
        int filenumber;
        int frequency;
    };
    
    typedef struct FileEntry_* FileEntry;
    
    struct SearchEntry_ {
        char *word;
        FileEntry* files;
        int numFiles;
    };
    
    typedef struct SearchEntry_* SearchEntry;

#endif
/* END SWIFT_SEARCH_H_ */

