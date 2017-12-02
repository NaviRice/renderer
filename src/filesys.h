#ifndef FILESYSHEADER
#define FILESYSHEADER



typedef struct file_s {
	FILE *f; //lol
	//TODO more junk
} file_t;


//returns a pointer to a string containing the line, or NULL if no more lines to read
//you must manually free the line after use
//the line might be empty (just a nully), but that still counts as a line
//no newlines are included
char * file_getEntireLine(file_t *f);
file_t file_open(const char * name, const char * mode);

int file_close(file_t *f);

size_t file_read(void *ptr, size_t size, size_t nmemb, file_t *f);
size_t file_write(const void *ptr, size_t size, size_t nmemb, file_t *f);

size_t file_getSize(file_t *f);

//ease of use... loads file at filename into an allocated buffer.
int file_loadString(const char * filename, char ** output);



#endif
