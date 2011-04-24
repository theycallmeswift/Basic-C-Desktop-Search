CC = gcc
CCFLAGS  = -ansi -pedantic -Wall -g

# UNIT TESTS

# Test 1 : Simple test to ensure that linking against the library succeeds
TEST1        =    test_hashtable
TEST1_SRC    =    tests/test_hashtable.c hashtable.o

TESTS        =    $(TEST1)


all: hashtable.o

hashtable.o: src/hashtable.c src/hashtable.h
	$(CC) $(CCFLAGS) -o hashtable.o -c src/hashtable.c


# Unit test declarations
$(TEST1): $(TEST1_SRC)
	$(CC) -ansi -Wall -g -o $@ $(TEST1_SRC)
	mv $(TEST1) bin/$(TEST1)

# Make all test files and then delete the dependancies.
tests: $(TESTS)
	-rm -f *.o

# Remove the .o files and clean the bin directory
clean:
	-rm -f *.o bin/*
