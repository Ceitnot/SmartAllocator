#make temp/test_lib
CC = g++
CFLAGS = -std=gnu++0x -Iinclude -Isrc
OBJ = obj/
all: %

create_dir:
	mkdir -p $(OBJ)

$(OBJ)smart_allocator.o: create_dir src/allocator.cpp
	$(CC) $(CFLAGS) -c src/smart_allocator.cpp -o $(OBJ)smart_allocator.o

$(OBJ)allocator.o: src/allocator.cpp
	$(CC) $(CFLAGS) -c src/allocator.cpp -o $(OBJ)allocator.o

%: $(OBJ)smart_allocator.o $(OBJ)allocator.o  %.cpp
	$(CC) $(CFLAGS)  $^  -o $@

.PHONY: clean

clean:
	rm -f $(OBJ)*.o 
