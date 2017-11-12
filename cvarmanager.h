#ifndef CVARMANAGERHEADER
#define CVARMANAGERHEADER

struct cvar_s;
typedef void (* cvarcallback_t)(struct cvar_s *c);

typedef struct cvar_s {
	int type;		//todo

	char * name;
	char * helptext; //maybe
	char * defaultvalue;

//	int numcallbacks;
//either NULL or NULL TERMINATED
	cvarcallback_t *onchanges;


	int valueint;
	float valuefloat;
	float valuevector[3];
	char * value;

	char isdefault; //todo ehh


	int myid;

} cvar_t;

//might get rid of this at some point
typedef struct cvarlist_s {
	cvar_t **list;
	int count;
} cvarlist_t;


#define CVAR_REGISTERED		1
#define CVAR_FREEABLE		2
#define CVAR_FREEONCHANGES	4
#define CVAR_SAVEABLE		8 //todo

//#include "idlist.h"
//cant use idlist for this because of the **

cvar_t ** cvar_list;
int cvar_count;
int cvar_arraysize;
int cvar_arraylasttaken;
int cvar_ok;

cvar_t * cvar_returnById(const int id);
cvar_t * cvar_findByNameRPOINT(const char * name);
int	 cvar_findByNameRINT(const char * name);


int cvar_init(void);


int cvar_register(cvar_t *c);
int cvar_unregister(const int id);
void cvar_print(cvar_t *c);
//todo
int cvar_unload(cvar_t *c);

void cvar_pset(cvar_t *c, const char * value);
void cvar_idset(const int c, const char * value);
void cvar_set(const char * c, const char * value);

int cvar_shutdown(void);


void cvar_forceNewlineEnd(cvar_t *c);


#endif
