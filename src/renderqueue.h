//mostly enaengine renderqueue

#ifndef RENDERQUEUEHEADER
#define RENDERQUEUEHEADER







#define RADIXSORTSIZE 10 //whatever
#define MAXINSTANCESIZE4096 //?



struct renderlistitem_s;
struct renderqueue_s;

typedef void (*renderqueueCallback_t)(const struct renderqueue_s *q, struct renderlistitem_s *ilist, unsigned int count);
typedef struct renderlistitem_s {
	void *data;
	size_t datasize;
	size_t dataoffset;
	renderqueueCallback_t setup;
	renderqueueCallback_t draw;
	unsigned char flags; //1 is freeable, 2 is copyable, 4 is instanceable
	unsigned int counter; // for instancecounts;
	unsigned char sort[RADIXSORTSIZE];
} renderlistitem_t;

typedef struct renderqueue_s {
	unsigned int size;
	unsigned int place;
	renderlistitem_t *list;
	size_t datasize;
	size_t dataplace;
	void * data;

	//moved scratch here so it can be sorted multithreaded
	//todo maybe have a #define to do it old school for memory/performance
	size_t scratchsize;
	renderlistitem_t *scratch;
	//moved ubo here to allow for multithradable
	ubo_t ubo;
	GLuint uboid; //for ease of use? //todo figure it out
	//todo allow for many ubos
} renderqueue_t;



//needs to be main thread
void renderqueue_draw(renderqueue_t *queue); //its not const since it "resets" place and dataplace

//can be called from any thread
void renderqueue_setup(const renderqueue_t *queue);
//maybe i should just move this into draw?

//needs to be main thread
void renderqueue_flushbuffers(renderqueue_t *queue);
//todo prune and half funcs

unsigned int renderqueue_cleanup(renderqueue_t *queue);

void renderqueue_sort(renderqueue_t *queue);


void renderqueue_addtoqueue(renderqueue_t *q, renderlistitem_t item);


#endif
