#include <epoxy/gl.h>

#include "globaldefs.h"
#include "vertexattribs.h"
#include "shadermanager.h"
#include "stringlib.h"		//for idlist, stringTestEqual

#include "filesys.h"		//for file manipulation

#include "glmanager.h"		//for glerror


#include "hashtables.h"		//for idlist

#include "cvarmanager.h"	//for cvars
IDLIST_INTERNAL(shader, shader_t, shaderlist_t);

cvarcallback_t CVAR_shaderversionstring_callbacks[] = {cvar_forceNewlineEnd, 0};

cvar_t CVAR_shaderversionstring = {CVAR_SAVEABLE, "shaderversionstring", "glsl version that is put at the top of every shader compiled", "#version 330", CVAR_shaderversionstring_callbacks};

int shader_init(void){
	IDLIST_INIT(shader, shader_t, shaderlist_t);
	cvar_register(&CVAR_shaderversionstring);
	cvar_print(&CVAR_shaderversionstring);
	return TRUE;
}
//returns ID of shader program (either added or already there)
int shader_register(char * name){
	//check if shader exists
	int ret = shader_findByNameRINT(name);
	if(ret) return ret;
	shader_t s = {0};
	s.type = 1;
	s.name = strdup(name);
	return shader_addRINT(s);
}


int shader_deleteCvars(shader_t *s){

	shadercvar_t *c = s->cvars;
	int i;
	for(i = 0; c; i++){
		//todo remove from the cvar->shader list
		if(c->sbuf) free(c->sbuf);
		if(c->name) free(c->name);
		shadercvar_t *olds = c;
		c = c->next;
		free(olds);
	}
	return i;
}
int shader_deleteSources(shadersource_t *s){
	int i;
	for(i = 0; s; i++){
		if(s->filename) free(s->filename);
		if(s->helperstring) free(s->helperstring);
		if(s->data) free(s->data);
		shadersource_t *olds = s;
		s = s->next;
		free(olds);
	}
	return i;
}

int shader_loadSources(shadersource_t *s){
	int i;
	for(i = 0; s; s=s->next, i++){
		if(!s->filename){
			printf("SHADER/loadSources error shader source without a filename! (index %i)\n", i);
			continue;
		}
		if(s->helperstring) free(s->helperstring);
		s->helperstring = malloc(10 + strlen(s->filename));
		sprintf(s->helperstring, "\n//FILE:%s\n", s->filename);
		if(s->data) free(s->data); //todo maybe just keep it?
		s->length = file_loadString(s->filename, &s->data);
		if(!s->length) printf("SHADER/loadSources error shader source %s either doesnt exist or is empty! (index %i)\n", s->filename, i);
	}
	return i;
}


int shader_printProgramLogStatus(const int id){
	GLint blen = 0;
	glGetProgramiv(id, GL_INFO_LOG_LENGTH, &blen);
	if(blen > 1){
		GLchar * log = (GLchar *) malloc(blen);
		glGetProgramInfoLog(id, blen, 0, log);
		printf("Program %i log\n%s\n", id, log);
		free(log);
		return blen;
	}
	return FALSE;
}
int shader_printShaderLogStatus(const int id){
	GLint blen = 0;
	glGetShaderiv(id, GL_INFO_LOG_LENGTH, &blen);
	if(blen > 1){
		GLchar * log = (GLchar *) malloc(blen);
		glGetShaderInfoLog(id, blen, 0, log);
		printf("Shader %i log\n%s\n", id, log);
		free(log);
		return blen;
	}
	return FALSE;
}
int shader_printShaderSourceStatus(const int id){
	GLint blen = 0;
	glGetShaderiv(id, GL_SHADER_SOURCE_LENGTH, &blen);
	if(blen > 1){
		GLchar * log = (GLchar *) malloc(blen);
		glGetShaderSource(id, blen, 0, log);
		printf("Shader %i source\n%s\n", id, log);
		free(log);
		return blen;
	}
	return FALSE;
}

int shader_listSources(shadersource_t *s){
	int i;
	for(i = 0; s; s=s->next, i++){
		printf(i ? ", %s" : "%s", s->filename);
	}
	printf("\n");
	return i;
}

//todo slam some stuff
int shader_compile(shader_t *s){
	if(!s){
		printf("SHADER/compile error null shader!\n");
		return FALSE;
	}
	if(!s->vsources) printf("SHADER/compile error no vertex shader sources!\n");
	if(!s->fsources) printf("SHADER/compile error no fragment shader sources!\n");
	//get counts
	//todo double check for nulls in the shader sources
	GLsizei vsc = 0;
	GLsizei fsc = 0;
	GLsizei gsc = 0;
	GLsizei csc = 0;//cvar
	shadersource_t * ss;
	for(ss = s->vsources; ss; ss = ss->next) if(ss->data) vsc++;
	for(ss = s->fsources; ss; ss = ss->next) if(ss->data) fsc++;
	for(ss = s->gsources; ss; ss = ss->next) if(ss->data) gsc++;

	shadercvar_t *sc;
	for(sc = s->cvars; sc; sc = sc->next){
		cvar_t *cv;
		if(sc->name && (cv = cvar_returnById(sc->id)) && cv->value){
			//if buff hasnt been updated, update it
			if(!sc->sbuf){
				size_t sizzie  = strlen(sc->name) + strlen(cv->value) + 12;
				sc->sbuf = malloc(sizzie);
				sprintf(sc->sbuf, "\n#define %s %s\n", sc->name, cv->value);
			}
			csc++;
		//todo double check this (also set a flag to later go through the list and "clean up"
		} else if(sc->sbuf){ free(sc->sbuf); sc->sbuf = 0; } //just in case the cvar was removed and i forgot about it
	}

	if(s->vsources && !vsc){
		printf("SHADER/compile error vertex shader sources exist, but none were loaded!\n");
	}
	if(s->fsources && !fsc){
		printf("SHADER/compile error fragment shader sources exist, but none were loaded!\n");
	}
	if(s->gsources && !gsc){
		printf("SHADER/compile error geometry shader sources exist, but none were loaded!\n");
	}
	if(!s->vsources || !s->fsources || !vsc || !fsc) return FALSE;


	char * intermediatestrings[] = {"//Cvars\n", "//Sourcefiles\n"};


	//generate buffers, load shaders
	int i;
	if(vsc){
		s->vertid = glCreateShader(GL_VERTEX_SHADER);
		int lc = vsc+csc+1+2+vsc; //shader sources + cvars + versionstring + intermediate strings + helper strings
		const GLchar ** vstring = malloc(lc * sizeof(GLchar *));
		i = 0;
		if(CVAR_shaderversionstring.value)vstring[i++] = CVAR_shaderversionstring.value;
		vstring[i++] = intermediatestrings[0];
		for(sc = s->cvars;    sc && i < lc; sc = sc->next) if(sc->sbuf) vstring[i++] = sc->sbuf;
		vstring[i++] = intermediatestrings[1];
		for(ss = s->vsources; ss && i < lc; ss = ss->next) if(ss->data){vstring[i++] = ss->helperstring; vstring[i++] = ss->data;}
		glShaderSource(s->vertid, i, vstring, NULL);
		if(vstring) free(vstring);
	}
	if(fsc){
		s->fragid = glCreateShader(GL_FRAGMENT_SHADER);
		int lc = fsc+csc+1+2+fsc; //shader sources + cvars + versionstring + intermediate strings + helper strings
		const GLchar ** fstring = malloc(lc * sizeof(GLchar *));
		i = 0;
		if(CVAR_shaderversionstring.value)fstring[i++] = CVAR_shaderversionstring.value;
		fstring[i++] = intermediatestrings[0];
		for(sc = s->cvars;    sc && i < lc; sc = sc->next) if(sc->sbuf) fstring[i++] = sc->sbuf;
		fstring[i++] = intermediatestrings[1];
		for(ss = s->fsources; ss && i < lc; ss = ss->next) if(ss->data){fstring[i++] = ss->helperstring; fstring[i++] = ss->data;}
		glShaderSource(s->fragid, i, fstring, NULL);
		if(fstring) free(fstring);
	}
	if(gsc){
		s->geomid = glCreateShader(GL_GEOMETRY_SHADER);
		int lc = gsc+csc+1+2+gsc; //shader sources + cvars + versionstring + intermediate strings + helper strings
		const GLchar ** gstring = malloc(lc * sizeof(GLchar *));
		i = 0;
		if(CVAR_shaderversionstring.value)gstring[i++] = CVAR_shaderversionstring.value;
		gstring[i++] = intermediatestrings[0];
		for(sc = s->cvars;    sc && i < lc; sc = sc->next) if(sc->sbuf) gstring[i++] = sc->sbuf;
		gstring[i++] = intermediatestrings[1];
		for(ss = s->gsources; ss && i < lc; ss = ss->next) if(ss->data){gstring[i++] = ss->helperstring; gstring[i++] = ss->data;}
		glShaderSource(s->geomid, i, gstring, NULL);
		if(gstring) free(gstring);
	}
	//shader sources should be GOOD now
	//compile em
	glCompileShader(s->vertid);
	glCompileShader(s->fragid);
	if(gsc)glCompileShader(s->geomid);
	//TODO statuses
	int fail=0;
	int status = GL_FALSE;
	glGetShaderiv(s->vertid, GL_COMPILE_STATUS, &status);
	if(shader_printShaderLogStatus(s->vertid) || status == GL_FALSE){
		//todo other stuff?
		printf("Vertex shader FAILED to compile, sources:\t"); shader_listSources(s->vsources);
		shader_printShaderSourceStatus(s->vertid);

		fail++;
	}
	glGetShaderiv(s->fragid, GL_COMPILE_STATUS, &status);
	if(shader_printShaderLogStatus(s->fragid) || status == GL_FALSE){
		//todo other stuff?
		printf("Fragment shader FAILED to compile, sources:\t"); shader_listSources(s->fsources);
		shader_printShaderSourceStatus(s->fragid);
		fail++;
	}
	if(gsc){
		glGetShaderiv(s->geomid, GL_COMPILE_STATUS, &status);
		if(shader_printShaderLogStatus(s->geomid) || status == GL_FALSE){
			//todo other stuff?
			printf("Geom shader FAILED to compile, sources:\t"); shader_listSources(s->gsources);
			shader_printShaderSourceStatus(s->geomid);

			fail++;
		}
	}
	if(fail){
		printf("SHADER/compile error %i shaders failed to compile\n", fail);

//		glDeleteShader(s->vertid);
//		glDeleteShader(s->fragid);
//		if(gsc)glDeleteShader(s->geomid);
//		s->vertid = s->fragid = s->geomid = 0;
//		return FALSE; //todo clean up
	}

	s->programid = glCreateProgram();
	glAttachShader(s->programid, s->vertid);
	glAttachShader(s->programid, s->fragid);
	if(gsc)glAttachShader(s->programid, s->geomid);
	//todo bind data locations

	//todo?
	glBindFragDataLocation(s->programid, 0, "fragData0");
	glBindFragDataLocation(s->programid, 1, "fragData1");
	glBindFragDataLocation(s->programid, 2, "fragData2");
	glBindFragDataLocation(s->programid, 3, "fragData3");

	glBindAttribLocation(s->programid, 0, "attrib0");
	glBindAttribLocation(s->programid, 1, "attrib1");
	glBindAttribLocation(s->programid, 2, "attrib2");
	glBindAttribLocation(s->programid, 3, "attrib3");
	glBindAttribLocation(s->programid, 4, "attrib4");
	glBindAttribLocation(s->programid, 5, "attrib5");

	glLinkProgram(s->programid);
	glDeleteShader(s->vertid);
	glDeleteShader(s->fragid);
	if(gsc)glDeleteShader(s->geomid);
	//get the status

	glGetProgramiv(s->programid, GL_LINK_STATUS, &status);
	if(shader_printProgramLogStatus(s->programid) || status == GL_FALSE){
		printf("SHADER/compile error shader program %i failed to link\n", s->programid);
		printf("Vert sources:\t"); shader_listSources(s->vsources);
		printf("Frag sources:\t"); shader_listSources(s->fsources);
		if(gsc){ printf("Geom sources:\t"); shader_listSources(s->gsources); }
		glDeleteProgram(s->programid);
		return FALSE;
	}

	glUseProgram(s->programid); //todo state
	char texstring[16]; //little extra room
	for(i = 0; i < 16; i++){
		sprintf(texstring, "texture%i", i);
		GLuint texpos = glGetUniformLocation(s->programid, texstring);
		glUniform1i(texpos, i);
	}
	//temporary uniform setup
	for(i = 0; i < 16; i++){
		sprintf(texstring, "uniform%i", i);
		s->uniloc[i] = glGetUniformLocation(s->programid, texstring);
	}
	//temporary uniform block setup
	for(i = 0; i < 2; i++){
		sprintf(texstring, "uniblock%i", i);
		s->uniblock[i] = glGetUniformBlockIndex(s->programid, texstring);
		if(s->uniblock[i] > -1) glUniformBlockBinding(s->programid, s->uniblock[i], i);
	}
CHECKGLERROR
	return TRUE;
}

int shader_addSource(shader_t *s, char * source, int sourcetype){
	if(!s || !source){
		printf("SHADER/addSource error null shader or source!\n");
		return FALSE;
	}
	char * typestring = "error";
	shadersource_t ** sptr = 0;
	switch(sourcetype){
		case 1:
			typestring = "vertex";
			sptr = &s->vsources;
			break;
		case 2:
			typestring = "fragment";
			sptr = &s->fsources;
			break;
		case 3:
			typestring = "geometry";
			sptr = &s->gsources;
			break;
		default:
			printf("SHADER/addSource error unknown type\n");
			return FALSE;
	}
	printf("adding %s source %s for shader %s\n", typestring, source, s->name);

	//walk linked list till end
	while(*sptr) sptr = &((*sptr)->next); //ugly lol

	shadersource_t * k = malloc(sizeof(shadersource_t));
	k->filename = strdup(source);
	k->helperstring = 0;
	k->data = 0;
	k->length = 0;
	k->next = 0;
	*sptr = k;
	return TRUE;
}

void shader_cvarChange(cvar_t *c){
	//look up shaders this cvar effects in the cvar->shaders list (based on cvar id), recompile
	//TODO implement with a hashtable! (specifically idbucket)

	//currently does a linear search through all the shaders to find ones that use the cvar (gross and slow)
	//todo change to a "hashtable" of cvar->shaders

	int i;
	for(i = 0; i <= shader_arraylasttaken; i++){
		shader_t *s = &shader_list[i];
		shadercvar_t *sc;
		//check to make shader has already been compiled
		if(!s->myid || s->type != 4) continue;
		for(sc = s->cvars; sc ; sc = sc->next) if(sc->id == c->myid) break;
		//found it, force an update of its sbuf and set the shader to be recompiled
		if(sc){
			if(sc->sbuf) free(sc->sbuf);
			sc->sbuf = 0;
			s->type = 3;
		}
	}
}


cvarcallback_t shaderchanges[] = {shader_cvarChange, 0};

//todo figure out if this should be per-shader or per-program (currently per-program, applied to all shaders)
//todo figure out onchanges behavior... do i add on if there already exists? Do i need to realloc?
int shader_addCvar(shader_t *s, char * c){
	//cvar cvar_name shader_name type (default)
	//str will be cvar_name shader_name type (default)

	char *strs[4] = {0, 0, 0, 0};

	int numfields = string_splitND(c, ',', strs, 4);
	printf("parsed %i words: %s %s %s %s\n", numfields, strs[0], strs[1], strs[2], strs[3]);
	if(numfields < 2){
		printf("SHADER/addCvar error only parsed %i words in the cvar data, my syntax is \"cvar cvar_name shader_name (type) (default)\"\n", numfields);
		return 0;
	}
	if(!strs[3])strs[3] = "0";
	//TODO, stuff like uniform or not, saveable, etc
	if(!strs[2])strs[2] = "define";

	//check if cvar exists
	cvar_t *cv = cvar_findByNameRPOINT(strs[0]);

	//add it if not there
	if(!cv){
		cv = malloc(sizeof(cvar_t));
		cv->type = CVAR_SAVEABLE | CVAR_FREEABLE ; // CVAR_FREEONCHANGES;
		cv->name = strdup(strs[0]);
		cv->defaultvalue = strdup(strs[3]);
		cv->helptext = strdup("shader system cvar TODO figure out this helptext");
		cv->myid = 0;
		cv->value = 0;
		cv->onchanges = 0;
		cvar_register(cv);
		//i need to set the onchanges AFTER registering it, since register will call the onchanges and i havent set that stuff up yet
		//todo figure this out for good
		cv->onchanges = shaderchanges;
	}
	cvar_print(cv);

	//add cvar/shader to cvar->shader list
	//TODO
	//add cvar link to shader cvar list


	shadercvar_t **sptr = &s->cvars;

	//walk linked list till end
	while(*sptr) sptr = &((*sptr)->next); //ugly lol

	shadercvar_t * k = malloc(sizeof(shadercvar_t));
	k->name = strdup(strs[1]);
	k->sbuf = 0;
	k->id = cv->myid;
	k->next = 0;
	*sptr = k;

	return numfields;
}

int shader_parseProgramFile(shader_t *s){
			if(!s->name){
				printf("SHADER/parseProgramFile error shader doesnt have a name?\n");
				return 0;
			}
			//todo move all this parsing stuff to filesys or something idk
			//load shader support file
			file_t f = file_open(s->name, "r");
			char * line;
			for(line = file_getEntireLine(&f); line; free(line), line = file_getEntireLine(&f)){
				/*
				//find deliminator
				char * c;
				char * strend = strlen(line) + line;
				char * delim = strchr(line, ':');
				if(!delim) delim = strend;
				*delim = 0;
				char * varname = line;
				char * vardata = delim+1;

				//trim whitespace from start of varname
				while(varname < delim && ISWHITESPACE(*varname)) varname++;
				c = delim-1;
				//empty line
				if(varname >= delim) varname = 0;
				//trim whitespace from end of varname
				else while(c > varname && ISWHITESPACE(*c)) *c--=0; //todo make sure this is OK

				//trim whitespace from start of vardata
				while(vardata < strend && ISWHITESPACE(*vardata)) vardata++;
				c = strend-1;
				//empty line
				if(vardata >= strend) vardata = 0;
				//trim whitespace from end of varname
				else while(c > vardata && ISWHITESPACE(*c)) *c--=0; //todo make sure this is OK
				printf("Varname: \"%s\"\t\tVardata: \"%s\"\n", varname, vardata);
				if(varname && vardata){
					if(string_testEqualCI(varname, "VertexSource"))		shader_addSource(s, vardata, 1);
					else if(string_testEqualCI(varname, "FragmentSource"))	shader_addSource(s, vardata, 2);
					else if(string_testEqualCI(varname, "GeometrySource"))	shader_addSource(s, vardata, 3);
					else if(string_testEqualCI(varname, "Cvar"))		shader_addCvar(s, vardata);
				}
				*/
//				printf("Line:%s\n", line);
				char *vd[2] = {0, 0};
				string_splitND(line, ':', vd, 2);
				printf("Varname: \"%s\"\t\tVardata: \"%s\"\n", vd[0], vd[1]);
				if(vd[0] && vd[1]){
					if(string_testEqualCI(vd[0], "VertexSource"))		shader_addSource(s, vd[1], 1);
					else if(string_testEqualCI(vd[0], "FragmentSource"))	shader_addSource(s, vd[1], 2);
					else if(string_testEqualCI(vd[0], "GeometrySource"))	shader_addSource(s, vd[1], 3);
					else if(string_testEqualCI(vd[0], "Cvar"))		shader_addCvar(s, vd[1]);
				}

			}
			file_close(&f);
			s->type =1;
	return FALSE;
}


int shader_load(shader_t *s){
	//todo a lot here
	switch(s->type){
		case 1:
			shader_parseProgramFile(s);
			//todo add checks
			s->type = 2;
		case 2:
			shader_loadSources(s->vsources);
			shader_loadSources(s->fsources);
			shader_loadSources(s->gsources);
			s->type = 3;
		case 3:
			shader_compile(s);
			s->type = 4;
		default: return s->type;
	}
}

int shader_unload(shader_t *s){
	//free shader sources (and data)
	if(s->vsources)shader_deleteSources(s->vsources);
	if(s->fsources)shader_deleteSources(s->fsources);
	if(s->gsources)shader_deleteSources(s->gsources);
	//free cvars and remove cvar->shader match (but dont remove cvars from cvar system, they might be used by other shaders)
	if(s->cvars)shader_deleteCvars(s);
	s->vsources = s->fsources = s->gsources = 0;
	//todo check before deleting
	glDeleteProgram(s->programid);
	glDeleteShader(s->vertid);
	glDeleteShader(s->fragid);
	glDeleteShader(s->geomid);
	return FALSE;
}

/*
int shader_shutdown(void){
	//lots of TODO
	if(shader_list){
		int i;
		for(i = 0; i <= shader_arraylasttaken; i++){
			if(!shader_list[i].type) continue;
			shader_unload(&shader_list[i]);
			shader_remove(shader_list[i].myid);
		}
		free(shader_list);
	}
	shader_list = 0;
	shader_ok = 0;
	shader_count = 0;
	shader_roll = 1;
	shader_arraysize = 0;
	shader_arrayfirstopen = 0;
	shader_arraylasttaken = -1;
	return TRUE;
}
*/
IDLIST_CODE(shader, shader_t, shaderlist_t);
