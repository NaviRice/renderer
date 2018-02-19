#ifndef GAMECODEINCLUDESHEADER
#define GAMECODEINCLUDESHEADER


#define GAMECODEINCLUDEVERSION 1

#define DEFERREDFLAG 1
#define FORWARDFLAG 2

	typedef struct ecallheader_s {
		//console
//		int (*console_printf)(const char *fmt, ...);
//		int (*console_nprintf)(const size_t size, const char *fmt, ...);

		//cvar
		int (*cvar_register)(cvar_t *c);
		int (*cvar_unregister)(const int id);
		void (*cvar_print)(cvar_t *c);
		int (*cvar_unload)(cvar_t *c);
		int (*cvar_findByNameRINT)(const char *name);
		cvar_t * (*cvar_findByNameRPOINT)(const char *name);
		void (*cvar_pset)(cvar_t *c, const char *value);
		void (*cvar_idset)(const int id, const char *value);
		cvar_t * (*cvar_returnById)(const int id);
		void (*cvar_forceNewlineEnd)(cvar_t *c); //do i use this?



		//entity
		int (*entity_findByNameRINT)(const char *name);
		entity_t * (*entity_findByNameRPOINT)(const char *name);

		idlist_t (*entity_findAllByNameRINT)(const char *name);
		entitylist_t (*entity_findAllByNameRPOINT)(const char *name);

		entity_t * (* entity_returnById)(const int id);

		int (* entity_addRINT)(entity_t e);
		entity_t * (* entity_addRPOINT)(entity_t e);

		int (*entity_remove)(const int id);

		//file
		int (*file_loadString)(const char * filename, char ** output);
		//todo more
//		int (*file_loadStringNoLength)(const char * filename, char ** output, const int debugmode);

		//light
//		int (*light_addRINT)(const char * name);
//		light_t * (*light_addRPOINT)(const char * name);

		//shader
//		int (*shader_createAndAddRINT)(const char * name);

		//stringlib
		unsigned int (*string_toVec)(const char *s, vec_t *c, const unsigned int maxdem);

		//texture
//		int (*texture_createAndAddGroupRINT)(const char * name);

		//model
		int (*model_register)(const char * name);

		//todo
		double now;
	} ecallheader_t;

	typedef struct gcallheader_s {
		unsigned int apiver;

		int (*initgame)(void);
		//todo
	} gcallheader_t;

#endif
