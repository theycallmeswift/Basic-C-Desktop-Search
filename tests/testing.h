/*
 * File: testing.h
 *
 * Author: Mike Swift
 * Email: theycallmeswift@gmail.com
 * Date Created: April 24th, 2011
 * Date Modified: April 24th, 2011
 */

#ifndef SWIFT_TESTING_H_
#define SWIFT_TESTING_H_

#define SW_ASSERT(exp, text, tests, fails) { \
            tests++; \
            if( !(exp) ) { \
                fprintf(stderr, "\e[0;31mFAILED: %s in \"%s\" on line %d.\e[0m\n", text, __FILE__, __LINE__); \
                fails++; \
            } else { \
                fprintf(stderr, "\e[1;32mPASSED: %s in \"%s\" on line %d.\e[0m\n", text, __FILE__, __LINE__); \
            } \
        }

#endif /* SWIFT_TESTING_H_ */
