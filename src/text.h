#ifndef TEXTHEADER
#define TEXTHEADER


typedef struct textitem_s {
	char * text;
	vec3_t offset3d; //from ent
	vec2_t offset2d;
	vec3_t rotation3d; //from ent
	vec2_t rotation2d;
	float size;
	//why dont i just make it a matrix?
	char orientflags; // 1 is viewport size, 2 is viewport aligned rotation. 2 also ignores any rotation3d
//	struct textitem_t *next;
} textitem_t;

/*
typedef struct textlist_s {
	textitem_t t;
	textitem_t *next;
} textlist_t;
*/


int text_init();

#endif
