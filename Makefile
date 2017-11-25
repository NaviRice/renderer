CC = gcc
LDFLAGS = -lGL -lGLU -lepoxy -lglfw -lm -ldl
CFLAGS = -Wall -Ofast -fstrict-aliasing -fprofile-use -march=native -g
OBJECTS = renderer.o matrixlib.o mathlib.o glfwmanager.o vbomanager.o hashtables.o modelmanager.o shadermanager.o filesys.o cvarmanager.o stringlib.o glmanager.o tracegrid.o viewportmanager.o planebox.o

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

renderer: $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o $@ $(LDFLAGS)
##	cd testgcdll && $(MAKE)

debug:	CFLAGS= -Wall -O0 -g `sdl-config --cflags` -fstrict-aliasing -march=native
debug: 	$(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o renderer-$@ $(LDFLAGS)
##	cd testgcdll && $(MAKE) debug

valgrind:	CFLAGS= -Wall -O0 -g -fstrict-aliasing -march=native -D DEBUGTIMESTEP
valgrind: 	$(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o renderer-$@ $(LDFLAGS)
##	cd testgcdll && $(MAKE) valgrind



clean:
	@echo cleaning oop
	@rm -f $(OBJECTS)
##	cd testgcdll && $(MAKE) clean
purge:
	@echo purging oop
	@rm -f $(OBJECTS)
	@rm -f renderer
	@rm -f renderer-debug
	@rm -f renderer-valgrind
	@rm -f renderer-profile
	@rm -f ./*.gcda
profile:	CFLAGS= -Wall -Ofast -g -fstrict-aliasing -fprofile-generate -march=native
profile: 	$(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o renderer-$@ $(LDFLAGS)
	@echo
	@echo You must run make clean before trying to compile any other option than profile
