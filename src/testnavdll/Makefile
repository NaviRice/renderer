CC = gcc
LDFLAGS = -shared
CFLAGS = -fPIC -Wall -Ofast -fstrict-aliasing -march=native
OBJECTS = nav.o

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

navcode.so: $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o ../../build/$@ $(LDFLAGS)
clean:
	@echo cleaning oop
	@rm -f $(OBJECTS)
