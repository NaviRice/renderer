CC = gcc
LDFLAGS = -shared
CFLAGS = -fPIC -Wall -Ofast -fstrict-aliasing -march=native
OBJECTS = gc.o bunbun.o

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

gamecode.so: $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o ../../build/$@ $(LDFLAGS)
valgrind: CFLAGS = -fPIC -Wall -O0 -g -fstrict-aliasing -D DEBUGTIMESTEP
valgrind: $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o ../../build/gamecode.so $(LDFLAGS)
debug: CFLAGS = -fPIC -Wall -O0 -g -fstrict-aliasing
debug: $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o ../../build/gamecode.so $(LDFLAGS)
clean:
	@echo cleaning oop
	@rm -f $(OBJECTS)
