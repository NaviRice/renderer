#include <epoxy/gl.h>

#include "globaldefs.h"
#include "contextmanager.h"
#include "vbomanager.h"
#include "modelmanager.h"
#include "stringlib.h"	//for idlist


#include "filesys.h"	//for file ops

#include "iqm.h"	//for iqm handling


#include "hashtables.h"	//for idlist
#include "mathlib.h"	//for bbox stuff currently


#include "glmanager.h"	//for checkglerror
IDLIST_INTERNAL(model, model_t, modellist_t);

int model_init(void){
	IDLIST_INIT(model, model_t, modellist_t);
	return TRUE;
}
//returns ID of model (either added or already there)
int model_register(char * name){
	//check if model exists
	int ret = model_findByNameRINT(name);
	if(ret) return ret;
	model_t m = {0};
	m.type = 1;
	m.vbo.type = 1; //trick the vbo manager to thinking its tracked
	m.name = strdup(name);
	return model_addRINT(m);
}

//VERY UNTESTED
int genIQMBBoxes(model_t *m, const struct iqmheader hdr, unsigned char * buf){
	//TODO
	return TRUE;
}

//VERY UNTESTED
int loadIQMBBoxes(model_t *m, const struct iqmheader hdr, unsigned char * buf){
	struct iqmbounds *b = (struct iqmbounds *) &buf[hdr.ofs_bounds];
	m->bbox[0] = b->bbmin[0];
	m->bbox[1] = b->bbmax[0];
	m->bbox[2] = b->bbmin[1];
	m->bbox[3] = b->bbmax[1];
	m->bbox[4] = b->bbmin[2];
	m->bbox[5] = b->bbmax[2];

	getBBoxPFromBBox(m->bbox, m->bboxp);
	return TRUE;
}


int loadIQMMeshes(model_t *m, const struct iqmheader hdr, unsigned char * buf){
	//todo multiple meshes support (would be helpful)
	int numfaces =	hdr.num_triangles;
	int numverts =	hdr.num_vertexes;
	if(numverts<1){
		printf("MODEL/loadIQMMeshes error vertex count is 0 (or less) for mesh # %i of model %s\n", 0, m->name);
		return FALSE;
	}
	float * dataptrs[MAXATTRIBS] = {0};
	int i;
	struct iqmvertexarray *vas = (struct iqmvertexarray *) &buf[hdr.ofs_vertexarrays];
	for(i = 0; i < (int)hdr.num_vertexarrays; i++){
		struct iqmvertexarray va = vas[i];
		switch(va.type){
			case IQM_POSITION:	if(va.format == IQM_FLOAT && va.size == 3) dataptrs[0] = (float *)&buf[va.offset]; m->vbo.datawidth[0] = 3; break;
			case IQM_NORMAL:	if(va.format == IQM_FLOAT && va.size == 3) dataptrs[1] = (float *)&buf[va.offset]; m->vbo.datawidth[1] = 3; break;
			case IQM_TANGENT:	if(va.format == IQM_FLOAT && va.size == 3) dataptrs[2] = (float *)&buf[va.offset]; m->vbo.datawidth[2] = 3; break; //todo double check that its actually 3?
//			case IQM_COLOR:		if(va.format == IQM_FLOAT && va.size == 3) dataptrs[3] = (float *)&buf[va.offset]; m->vbo.datawidth[3] = 3; break; //todo color support (current float in vbos)
			case IQM_TEXCOORD:	if(va.format == IQM_FLOAT && va.size == 2) dataptrs[4] = (float *)&buf[va.offset]; m->vbo.datawidth[4] = 2; break;
		}
	}
	if(!dataptrs[0]){
		printf("MODEL/loadIQMMeshes error no position data found for mesh #%i of model %s\n", 0, m->name);
		return FALSE;
	}

	if(vbo_setup(&m->vbo) != 2){
		printf("MODEL/loadIQMMeshes error VBO failed to initialize for mesh #%i of model %s (vbo->type is %i)\n", 0, m->name, m->vbo.type);
		return FALSE;
	}
	vbo_bind(&m->vbo);
	for(i = 0; i < MAXATTRIBS; i++){
		int dwidth = m->vbo.datawidth[i];
		if(!dwidth) continue;
		glBindBuffer(GL_ARRAY_BUFFER, m->vbo.vertsid[i]);
		glBufferData(GL_ARRAY_BUFFER, numverts * dwidth * sizeof(GLfloat), dataptrs[i], GL_STATIC_DRAW);
	}
	CHECKGLERROR
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m->vbo.facesid);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, numfaces * 3 * sizeof(GLuint), (GLuint *) &buf[hdr.ofs_triangles], GL_STATIC_DRAW);
	CHECKGLERROR
	m->vbo.numverts =numverts;
	m->vbo.numfaces =numfaces;
	return TRUE;
}




int loadModelIQM(model_t *m){
	//somewhat copied from the SDK
	file_t f = file_open(m->name, "rb");
	if(!f.f) return FALSE;

	unsigned char * buf = NULL;
	struct iqmheader hdr;
	if(file_read(&hdr, 1, sizeof(hdr), &f) != sizeof(hdr) || memcmp(hdr.magic, IQM_MAGIC, sizeof(hdr.magic)))
	goto error; //spaghetti!
	if(hdr.version != IQM_VERSION)goto error;
	if(hdr.filesize > (16<<20)) goto error; // sanity check... don't load files bigger than 16 MB
	buf = malloc(hdr.filesize);
	if(file_read(buf + sizeof(hdr), 1, hdr.filesize - sizeof(hdr), &f) != hdr.filesize - sizeof(hdr))
		goto error;
//todo actually load
	if(hdr.num_meshes > 0 && !loadIQMMeshes(m, hdr, buf)) goto error;
//todo handle joints
//	if(hdr.num_joints > 0 && !loadiqmjoints(m, hdr, buf)) goto error;
//Not handling poses here

	//first try to manually load the pregenerated bbox, then try to generate them from mesh data if that fails
	if(!loadIQMBBoxes(m,hdr, buf) && !genIQMBBoxes(m, hdr, buf)) goto error;

	file_close(&f);
	free(buf);
	return TRUE;

	error:

	//TODO check if the VBO has been messed with, and if it has, get rid of the alloc
	printf("MODEL/loadModelIQM error while loading %s\n", m->name);
	free(buf);
	file_close(&f);
	return FALSE;
}

int model_load(model_t *m){
	//TODO
//	printf("MODEL/model_load, %s type %i\n", m->name, m->type);
	switch(m->type){
		case 1:
			if(!loadModelIQM(m)) return FALSE;
			m->type = 2;
		case 2:
			m->type = 3;
			printf("loaded model %s with %i faces and %i verts\n", m->name, m->vbo.numfaces, m->vbo.numverts);
		case 3:
		default:
		return m->type;
	}
	return FALSE;
}

int model_bind(model_t *m){
	if(m->type != 3) model_load(m);
	return vbo_bind(&m->vbo);
}

int model_unload(model_t *m){
//TODO
	//unload vbo
	vbo_unload(&m->vbo);
	return TRUE;
}
/*
int model_shutdown(void){
	//lots of TODO
	if(model_list){
		int i;
		for(i = 0; i < model_arraylasttaken; i++){
			if(!model_list[i].type)continue;
			model_unload(&model_list[i]);
			model_remove(model_list[i].myid);
		}
		free(model_list);
	}
	model_list = 0;
	model_ok = 0;
	model_count = 0;
	model_roll = 1;
	model_arraysize = 0;
	model_arrayfirstopen = 0;
	model_arraylasttaken = -1;
	return TRUE;
}
*/
IDLIST_CODE(model, model_t, modellist_t);
