#include "globaldefs.h"

#include "stringlib.h"		//for "idlist"
#include "hashtables.h"		//for "idlist"

#include "cvarmanager.h"


cvar_t ** cvar_list = 0;
int cvar_count = 0;
int cvar_roll = 1;
int cvar_arraysize = 0;
int cvar_arrayfirstopen = 0;
int cvar_arraylasttaken = -1;
int cvar_ok = 0;

hashbucket_t cvar_hashtable[MAXHASHBUCKETS];

int cvar_init(void){
	//todo
	if(cvar_list)free(cvar_list);
	cvar_list	=0;
	cvar_count	=0;
	cvar_roll	=1;
	cvar_arraysize	=0;
	cvar_arraylasttaken =-1;
	memset(cvar_hashtable, 0, MAXHASHBUCKETS * sizeof(hashbucket_t));


	return TRUE;
}

void cvar_prun(cvar_t *c){
	if(!c) return;
	cvarcallback_t *pr = c->onchanges;
	if(!pr) return;
	for(; *pr; pr++) (*pr)(c);
}


void cvar_print(cvar_t *c){
	if(!c) return;

	printf("cvar_print: %i \"%s\", \"%s\", \"%s\", {%i, %f, %f:%f:%f, \"%s\"}\n", c->type, c->name, c->helptext, c->defaultvalue,
		c->valueint, c->valuefloat,
		c->valuevector[0], c->valuevector[1], c->valuevector[2],
		c->value);
}



void cvar_pset(cvar_t *c, const char * value){
	if(!c) return;
	//if(c && string_testEqual(c->value, value)) return; //check if setting to the same thing
//	unsigned int inlength = strlen(value)+1;

	if(c->value) free(c->value);
	c->value = strdup(value);

	float vf = atof(value);
	c->valueint = (int)vf;
	c->valuefloat = vf;
	string_toVec(value, c->valuevector, 3);
	if(!value == !c->defaultvalue) c->isdefault = FALSE;
	else c->isdefault = string_testEqual(value, c->defaultvalue);
	cvar_prun(c);
}
void cvar_set(const char * c, const char * value){
	cvar_pset(cvar_findByNameRPOINT(c), value);
}
void cvar_idset(const int c, const char * value){
	cvar_pset(cvar_returnById(c), value);
}

//only for manually allocated cvars
int cvar_unload(cvar_t *c){
	//todo
	return TRUE;
}

cvar_t * cvar_findByNameRPOINT(const char * name){
	int id = hash_findByNameRINT(name, cvar_hashtable);
	return id ? cvar_returnById(id) : 0;
}

int cvar_findByNameRINT(const char * name){
	return hash_findByNameRINT(name, cvar_hashtable);
}


cvar_t * cvar_returnById(const int id){
//	if(!id) return FALSE;
        int index = (id & 0xFFFF);
	printf("index is %i, id is %i\n", index, id);
        cvar_t * ret = cvar_list[index];
        if(ret->myid == id) return ret;
        return FALSE;
}

void cvar_pruneList(void){
	if(cvar_arraysize == cvar_arraylasttaken) return;
	cvar_arraysize = cvar_arraylasttaken+1;
	cvar_list = realloc(cvar_list, cvar_arraysize * sizeof(cvar_t *));
}

//TODO
//a modified remove
int cvar_unregister(const int id){
	int index = (id & 0xFFFF);
	cvar_t * ret = cvar_list[index];
	if(ret->myid != id) return FALSE;
	//todo should i check the registered flag here?
	cvar_count--;
	if(ret->name) hash_removeFromTable(ret->name, id, cvar_hashtable);
	if(ret->type & CVAR_FREEONCHANGES){
		if(ret->onchanges) free(ret->onchanges);
		ret->onchanges= 0;
	}
	if(ret->type & CVAR_FREEABLE){
		if(ret->name)	 	free(ret->name);
		if(ret->helptext)	free(ret->helptext);
		if(ret->defaultvalue)	free(ret->defaultvalue);
		ret->name = ret->helptext = ret->defaultvalue = 0;
	}

	cvar_list[index] = 0;
	if(index < cvar_arrayfirstopen) cvar_arrayfirstopen = index;
	for(; cvar_arraylasttaken > 0 && !cvar_list[cvar_arraylasttaken]; cvar_arraylasttaken--);
	ret->type &= (~CVAR_REGISTERED);
	return TRUE;
}

int cvar_shutdown(void){
	int i = 0;
	if(cvar_list){
		for(i = 0; i <= cvar_arraylasttaken; i++){
			if(!cvar_list[i]->myid)continue;
			cvar_unregister(cvar_list[i]->myid);
			//TODO
//			_CONCAT(NAME, _unload)(&_CONCAT(NAME, _list)[i]);
//			_CONCAT(NAME, _remove)(_CONCAT(NAME, _list)[i].myid);
		}
		free(cvar_list);
	}
	cvar_list = 0;
	cvar_ok = 0;
	cvar_count = 0;
	cvar_roll = 1;
	cvar_arraysize = 0;
	cvar_arrayfirstopen = 0;
	cvar_arraylasttaken = -1;
	return i;
}
//aka cvar_addRINT
int cvar_register(cvar_t * inst){
	if(!inst || !inst->name || inst->type & CVAR_REGISTERED) return FALSE;
	cvar_count++;
	cvar_roll++;
	if(cvar_roll > 65535) cvar_roll = 1;//no possible way for IDs to be 0 now
	for(; cvar_arrayfirstopen < cvar_arraysize && cvar_list[cvar_arrayfirstopen]; cvar_arrayfirstopen++);
	if(cvar_arrayfirstopen == cvar_arraysize){
		cvar_arraysize++;
		cvar_list = realloc(cvar_list, cvar_arraysize * sizeof(cvar_t*));
	}
	cvar_list[cvar_arrayfirstopen] = inst;
	int returnid = (cvar_roll << 16) | cvar_arrayfirstopen;
	inst->myid = returnid;

	hash_addToTable(inst->name, returnid, cvar_hashtable);
	if(cvar_arraylasttaken < cvar_arrayfirstopen) cvar_arraylasttaken = cvar_arrayfirstopen;

	inst->type |= CVAR_REGISTERED;

	if(inst->defaultvalue){
		cvar_pset(inst, inst->defaultvalue);
	}

	return returnid;
}//dont need rpoint because you already got rpoint lol




//todo if i add any more of these ease of use ones, put them in a seperate file, like "cvarcallbacks.c"
//ease of use callback to automatically add a newline to the end of a cvar if it does not exist
//DOES NOT CALL PSET
//does not mess with the default flag, so the newline modified one will still be default
void cvar_forceNewlineEnd(cvar_t *s){
	if(!s || !s->value) return;
	size_t l = strlen(s->value);
	if(!l || s->value[l-1] == '\n') return;
	char * newwy = malloc(l+2);
	strcpy(newwy, s->value);
	newwy[l] = '\n';
	newwy[l+1] = 0;
	free(s->value);
	s->value = newwy;
}
