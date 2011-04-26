CC = gcc
CCFLAGS  = -ansi -pedantic -Wall -g

# UNIT TESTS

# Test 1 : Simple test to ensure that linking against the library succeeds
TEST1        =    test_hashtable
TEST1_SRC    =    tests/test_hashtable.c hashtable.o tokenizer.o

TESTS        =    $(TEST1)


all: hashtable.o tokenizer.o

hashtable.o: src/hashtable.c src/hashtable.h
	$(CC) $(CCFLAGS) -o hashtable.o -c src/hashtable.c
	
tokenizer.o: src/tokenizer.c src/tokenizer.h
	$(CC) $(CCFLAGS) -o tokenizer.o -c src/tokenizer.c


# Unit test declarations
$(TEST1): $(TEST1_SRC)
	$(CC) -ansi -Wall -g -o $@ $(TEST1_SRC)
	mv $(TEST1) bin/$(TEST1)
	mkdir -p bin/files
	cp tests/files/* bin/files

# Make all test files and then delete the dependancies.
tests: $(TESTS)
	-rm -f *.o

# Remove the .o files and clean the bin directory
clean:
	-rm -rf *.o bin/*
