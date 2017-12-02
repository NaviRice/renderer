#ifndef HASHTABLESHEADER
#define HASHTABLESHEADER

typedef struct hashbucket_s {
	char * name;
	int id;
	struct hashbucket_s * next;
} hashbucket_t;

//todo actually implement and use this in the shader cvar stuff
typedef struct idbucket_s {
	int *key;
	int id;
	struct idbucket_s * next;
} idbucket_t;

int hash_get(const char * string);
int hash_getCI(const char * string);
int hash_addToTable(const char * name, int id, hashbucket_t * ht);
int hash_removeFromTable(const char * name, int id, hashbucket_t * ht);
int hash_findByNameRINT(const char * name, hashbucket_t * ht);
#endif
