CC = gcc
CCFLAGS  = -ansi -pedantic -Wall -g

# UNIT TESTS

# Test 1 : A variety of tests against the HashTable class
TEST1        =    test_hashtable
TEST1_SRC    =    tests/test_hashtable.c hashtable.o tokenizer.o

# Test 2 : Professor Thu's base tests for the Sorted List
TEST2        =    test_sortedlist
TEST2_SRC    =    tests/test_sortedlist.c sorted-list.o

TESTS        =    $(TEST1) $(TEST2)


all: index search cleanobjs

index: hashtable.o tokenizer.o sorted-list.o words.o index.o
	$(CC) $(CCFLAGS) -o index hashtable.o tokenizer.o sorted-list.o words.o index.o
	mv index bin/index
	mkdir -p bin/files
	cp tests/files/* bin/files

search: hashtable.o tokenizer.o sorted-list.o words.o search.o
	$(CC) $(CCFLAGS) -o search hashtable.o tokenizer.o sorted-list.o words.o search.o
	mv search bin/search

search.o: src/search.c src/search.h src/sorted-list.h src/hashtable.h src/tokenizer.h src/words.h
	$(CC) $(CCFLAGS) -o search.o -c src/search.c
	
index.o: src/index.c src/index.h src/sorted-list.h src/hashtable.h src/tokenizer.h src/words.h
	$(CC) $(CCFLAGS) -o index.o -c src/index.c

hashtable.o: src/hashtable.c src/hashtable.h
	$(CC) $(CCFLAGS) -o hashtable.o -c src/hashtable.c
	
tokenizer.o: src/tokenizer.c src/tokenizer.h
	$(CC) $(CCFLAGS) -o tokenizer.o -c src/tokenizer.c
	
sorted-list.o: src/sorted-list.c src/sorted-list.h
	$(CC) $(CCFLAGS) -o sorted-list.o -c src/sorted-list.c
	
words.o: src/words.c src/words.h
	$(CC) $(CCFLAGS) -o words.o -c src/words.c

# Unit test declarations
$(TEST1): $(TEST1_SRC)
	$(CC) -ansi -Wall -g -o $@ $(TEST1_SRC)
	mv $(TEST1) bin/$(TEST1)

$(TEST2): $(TEST2_SRC)
	$(CC) -ansi -Wall -g -o $@ $(TEST2_SRC)
	mv $(TEST2) bin/$(TEST2)

# Make all test files and then delete the dependancies. 
tests: $(TESTS)
	-rm -f *.o
	mkdir -p bin/files
	cp tests/files/* bin/files

# Remove the .o files and clean the bin directory
cleanobjs:
	-rm -f *.o

clean:
	-rm -rf *.o 
	-rm -rf bin/index bin/files bin/search
