#ifndef NAVINCLUDESHEADER
#define NAVINCLUDESHEADER

#define NAVCODEINCLUDEVERSION 1


	typedef struct navecallheader_s {
		//cvar?
		//todo more
		int yolo;
	} navecallheader_t;

	typedef struct navgcallheader_s {
		unsigned int apiver;
		int (*initnav)(void);
	} navgcallheader_t;


#endif
