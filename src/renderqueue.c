#include <epoxy/gl.h>


#include "globaldefs.h"
#include "glmanager.h" // for checkglerror
#include "ubomanager.h" //for the ubos

#include "renderqueue.h"


//todo states





unsigned int renderqueue_cleanup(renderqueue_t *q){
	if(!q) return FALSE;
	renderlistitem_t *list = q->list;
	if(!list) return FALSE;
	unsigned int i, counter = 0;
	unsigned int place = q->place;
	for(i = 0; i < place; i++){
		renderlistitem_t * item= list+i;
		if((item->flags & 1) && item->data){
			free(item->data);
			item->data = 0;
			counter++;
		}
	}
	//todo debug prints?

	if(q->list)free(q->list);
	if(q->data)free(q->data);

	memset(q, 0, sizeof(renderqueue_t));
	return counter;

}

//modified ioquake3 code
static inline void renderqueue_radix(int byte, unsigned int size, renderlistitem_t *source, renderlistitem_t *dest){
	int count[256] = {0};
	int index[256];
	int i;

	unsigned char *sortKey = NULL;
	unsigned char *end = NULL;
	sortKey = ( (unsigned char *) &source->sort) + byte;
	end = sortKey + (size * sizeof(renderlistitem_t) );
	for( ; sortKey < end; sortKey += sizeof(renderlistitem_t ) )
		 count[*sortKey]++;

	index[0] = 0;

	for(i = 1; i < 256; i++)
		index[i] = index[i-1] + count[i-1];
	sortKey = ( (unsigned char *)&source->sort) + byte;

	for(i = 0; i < size; i++, sortKey += sizeof(renderlistitem_t) )
		dest[ index[*sortKey]++ ] = source[i];
}

//modified ioquake3 code
//todo look into quicksort and other sorts when the size is small
void renderqueue_sort(renderqueue_t *q){
	if(!q) return;
	if(q->scratchsize < q->place){
		q->scratch = realloc(q->scratch, q->place * sizeof(renderlistitem_t));
		q->scratchsize = q->place;
	}
	renderqueue_radix(0, q->place, q->list, q->scratch);
	renderqueue_radix(1, q->place, q->list, q->scratch);
	renderqueue_radix(2, q->place, q->list, q->scratch);
	renderqueue_radix(3, q->place, q->list, q->scratch);
	renderqueue_radix(4, q->place, q->list, q->scratch);
	renderqueue_radix(5, q->place, q->list, q->scratch);
	renderqueue_radix(6, q->place, q->list, q->scratch);
	renderqueue_radix(7, q->place, q->list, q->scratch);
	renderqueue_radix(8, q->place, q->list, q->scratch);
	renderqueue_radix(9, q->place, q->list, q->scratch);
//	renderqueue_radix(10, q->place, q->list, q->scratch);
//	renderqueue_radix(11, q->place, q->list, q->scratch);
	//todo figure out a way to preprocessor this
}


void renderqueue_draw(renderqueue_t *q){
	unsigned int i = 0;
	unsigned int place= q->place;
	renderlistitem_t *list = q->list;
	while( i < place){
		renderlistitem_t *item = list+i;
		//DRAW THAT SHIT
		unsigned int cmax, counter = item->counter;
		//check, but whatever
//		if(counter < 1) counter =1;
		item->draw(q, item, counter);
		//cleanup
		//and free it... instanceable too!
		for(cmax = i+counter; i < cmax; i++, item++){
			if(item->flags & 1 && item->data){
				free(item->data);
				item->data = 0;
			}
		}
	}
	//RESETTI
	q->place =0;
	q->dataplace = 0;
}


void renderqueue_setup(const renderqueue_t *q){
	unsigned int i = 0;
	unsigned int place = q->place;
	renderlistitem_t * list = q->list;
	while(i < place){
		renderlistitem_t * item = list + i;
		//update data pointer to now point to the copied area (if it was copied)
		//cant do this "on da fly" since item->data is realloced a lot before this
		if(item->flags & 2) item->data = q->data + item->dataoffset;
		//if instanceable
		if(item->flags & 4){
			//approximate instancing by checking the callbacks and instance flag
			renderqueueCallback_t ccalls = item->setup;
			renderqueueCallback_t ccalld = item->draw;
			unsigned int counter, max = place-i;
			renderlistitem_t *current = item+1;
			for(counter = 1; counter < max && (current->flags & 4) && current->draw == ccalld && current->setup == ccalls; counter++, current++){
				//gotta update the data also, since i didnt bother to do a prepass to do it...
				if(current->flags & 2) current->data = q->data + current->dataoffset;
			}
			//todo this should be set in ->setup, so i dont need to set it here?
			item->counter = counter;
			item->setup(q, item, counter);
			i+= counter;
		} else {
			item->counter = 1;
			item->setup(q, item, 1);
			i++;
		}
	}
	//flushing vert and ubo stuff done in a separet function since this function has to be "thread safe"
}

void renderqueue_flushbuffers(const renderqueue_t *q){
	//todo
}

void renderqueue_addtoqueue(renderqueue_t *q, renderlistitem_t item){
	if(!item.setup || !item.draw) return;
	unsigned int rqnsize = q->place +1;
	if(rqnsize > q->size){
		q->list = realloc(q->list, rqnsize * sizeof(renderlistitem_t));
		q->size = rqnsize;
	}
	//copyable data
	if(item.flags & 2){
		size_t dataplace = q->dataplace;
		size_t datanewsize = dataplace + item.datasize;
		if(datanewsize > q->datasize){
			q->data = realloc(q->data, datanewsize);
			q->datasize = datanewsize;
		}
		memcpy(q->data + dataplace, item.data, item.datasize);
		//if it was freeable, free it, since we copied it
		if(item.flags & 1){
			free(item.data);
			item.flags &= ~1; //no longer freeable
		}
		item.data = 0;

		item.dataoffset = dataplace;
		q->dataplace += item.datasize;
	}
	memcpy(q->list + q->place, &item, sizeof(renderlistitem_t));
	q->place++;
}
