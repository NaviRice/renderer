#include "globaldefs.h"
#include <string.h> //well duh

#include "stringlib.h"


/*
vec3_t string_toVec3(const char *s){
	unsigned int i;
	vec3_t v;
	for(i = 0; i < 3; i++){
		while(*s && ISWHITESPACE(*s)) s++;
		if(!*s) break;
		v[i] = atof(s);
		while(!ISWHITESPACE(*s)) s++;
	}
	return v;
}
*/
//returns TRUE if both strings are equal, false otherwise
//suitable replacement for !strcmp() if you only want to check if they are the same
int string_testEqual(const char * one, const char * two){
	if(!one || !two) return FALSE;
	char * m1 = (char *)one;
	char * m2 = (char *)two;
	//i dont test both in the while because checking weather they are equal does it for me
	while(*m1) if(*m1++ != *m2++) return FALSE;
	//now check if m1 stopped early and m2 still goes
	if(*m2) return FALSE;
	return TRUE;
}
//same as above except case insensitive
int string_testEqualCI(const char * one, const char * two){
	if(!one || !two) return FALSE;
	char * m1 = (char *)one;
	char * m2 = (char *)two;
	//i dont test both in the while because checking weather they are equal does it for me
	while(*m1){
		if(TOLOWER(*m1) != TOLOWER(*m2)) return FALSE;
		m1++;
		m2++;
	}
	//now check if m1 stopped early and m2 still goes
	if(*m2) return FALSE;
	return TRUE;
}

//returns TRUE if both strings are equal, false otherwise. tests only first n characters
//suitable replacement for !strcmp() if you only want to check if they are the same
//unsigned int because strings might be >2gb. Call the string length police i dont care.
int string_testEqualN(const char * one, const char * two, const unsigned int n){
	if(!one || !two) return FALSE;
	char * m1 = (char *)one;
	char * m2 = (char *)two;
	unsigned int i;
	//i dont test both in the while because checking weather they are equal does it for me
	for(i = 0; i < n && *m1; i++) if(*m1++ != *m2++) return FALSE;
	return TRUE;
}
//same as above but case insensitive
int string_testEqualNCI(const char * one, const char * two, const unsigned int n){
	if(!one || !two) return FALSE;
	char * m1 = (char *)one;
	char * m2 = (char *)two;
	unsigned int i;
	//i dont test both in the while because checking weather they are equal does it for me
	for(i = 0; i < n && *m1; i++, m1++, m2++)if(TOLOWER(*m1) != TOLOWER(*m2)) return FALSE;
	return TRUE;
}


//todo figure out how to parse it out of "vec3(blah,blah,blah)"
unsigned int string_toVec(const char *s, vec_t *v, const unsigned int maxdem){
	unsigned int i;
	for(i = 0; i < maxdem; i++){
		while(*s && ISWHITESPACE(*s)) s++;
		if(!*s) break;
		v[i] = atof(s);
		while(!ISWHITESPACE(*s)) s++;
	}
	//set rest of them to 0.0
	unsigned int j;
	for(j = i; j < maxdem; j++) v[j] = 0.0;
	return j;
}

unsigned int string_countWords(const char *s){
	unsigned int i = 0;
	char * mystr = (char *) s;
	while(*mystr){
		while(*mystr && ISWHITESPACE(*mystr)) mystr++;
		if(*mystr && !ISWHITESPACE(*mystr)) i++, mystr++;
		while(*mystr && !ISWHITESPACE(*mystr)) mystr++;
	}
	return i;
}
//DESTRUCTIVE
char * string_trim(char * in){
		//trim whitespace at beginning
		while(*in && ISWHITESPACE(*in)) in++;
		//find end
		char *e = strlen(in) + in;
		//trim whitespace from end
		while(e >= in && ISWHITESPACE(*e)) *e--=0;
		return in;
}
//DESTRUCTIVE
//returns 0 if string is entirely whitespace
char * string_trimD(char * in){
		//trim whitespace at beginning
		while(*in && ISWHITESPACE(*in)) in++;
		//find end
		char *e = strlen(in) + in;
		//trim whitespace from end
		while(e >= in && ISWHITESPACE(*e)) *e--=0;
		return e < in ? 0 : in;
}



//TODO functions that dont fuck with the origional string
//TODO splits that dynamically allocate more space and dont have a maxnum



//essentially strtok but also trims

//DESTRUCTIVE. fucks with the origional string
int string_splitN(char *s, const char delim, char ** outstring, int num){
	if(num <1)return 0; //why bother?
	int ret;
	outstring[0] = s;
	//preprocess, find start of each word and null the delim
	for(ret = 1; ret < num; ret++){
		char * d = strchr(s, delim);
		if(!d) break;
		s = outstring[ret] = d+1;
		*d = 0;
	}
	int i;
	for(i = 0; i < ret; i++){
		/*
		//trim whitespace at beginning
		while(*(outstring[i]) && ISWHITESPACE(*(outstring[i]))) outstring[i]++;
		//find end
		char *e = strlen(outstring[i]) + outstring[i];
		//trim whitespace from end
		while(e >= outstring[i] && ISWHITESPACE(*e)) *e--=0;
		*/
		outstring[i] = string_trim(outstring[i]);
	}

	return ret;
}

//DESTRUCTIVE. fucks with the origional string
//also sets any empty strings to null
int string_splitND(char *s, const char delim, char ** outstring, int num){
	if(num <1)return 0; //why bother?
	int ret;
	outstring[0] = s;
	//preprocess, find start of each word and null the delim
	for(ret = 1; ret < num; ret++){
		char * d = strchr(s, delim);
		if(!d) break;
		s = outstring[ret] = d+1;
		*d = 0;
	}
	int i;
	for(i = 0; i < ret; i++){
		outstring[i] = string_trimD(outstring[i]);
	}

	return ret;
}
