#ifndef NAVINCLUDESHEADER
#define NAVINCLUDESHEADER

#define NAVCODEINCLUDEVERSION 2


	typedef struct navwaypoint_s {
		float x;
		float y;
		char * desc;
		char * icon;
	} navwaypoint_t ;

	typedef struct navecallheader_s {
		//cvar?
		//todo more
		int (*nav_setCurrentWaypoint)(navwaypoint_t *n);
	} navecallheader_t;

	typedef struct navgcallheader_s {
		unsigned int apiver;
		int (*initnav)(void);
	} navgcallheader_t;


#endif
