#include "globaldefs.h"
#include "stringlib.h"
#include "filesys.h"





char * file_getEntireLine(file_t *f){
	char * line = 0;

	size_t lsize = 0;
	int lplace =0;
	char c;
	for(c = fgetc(f->f); c != EOF && c != '\n'; c=fgetc(f->f)){
		if(lplace >= lsize) lsize = (lplace + 32) & ~(32-1); //TODO FIX
		//		if(lplace >= lsize) lsize = lplace + 32;
		line = realloc(line, lsize);
		line[lplace++] = c;
	}
	//check to make sure we arent reading nothing
	if(c == EOF && !lplace) return 0;

	//tighten up the alloc, add a nully
	if(lsize != lplace+1){
		line = realloc(line, lplace+1);
	}
	line[lplace++] = 0;

	return line;
}





//untested. todo
char * file_getEntireDelim(file_t *f, char * delims){
	return 0;
}


file_t file_open(const char * name, const char * mode){
	//todo more
	file_t f;
	f.f = fopen(name, mode);
	if(!f.f){
		printf("Couldn't open: %s\n", name);
	}
	return f;
}


int file_close(file_t * f){
	//TODO more
	//TODO set the f->f to 0 after?
	return fclose(f->f);
}



size_t file_read(void *ptr, size_t size, size_t nmemb, file_t *f){
	//TODO more
	return fread(ptr, size, nmemb, f->f);
}
size_t file_write(const void *ptr, size_t size, size_t nmemb, file_t *f){
	//TODO more
	return fwrite(ptr, size, nmemb, f->f);
}
size_t file_getSize(file_t *f){
	//TODO more
	size_t ret;
	fseek(f->f, 0, SEEK_END);
	ret = ftell(f->f);
	rewind(f->f);
	return ret;
}
//ease of use function... read filename into string
int file_loadString(const char * filename, char ** output){
	int length;
	file_t f = file_open(filename, "r");
	if(!f.f){
		printf("ERROR -- file %s does not exist\n", filename);
		return FALSE;
	}
	length = file_getSize(&f);
	if(!length) return FALSE;
	*output = malloc(length+1);
	//memset(*output, 0, *length+1);
	file_read(*output, 1, length, &f);
	(*output)[length] = 0; //nully just in case
	file_close(&f);
	return length; //will return false if length is 0
}
