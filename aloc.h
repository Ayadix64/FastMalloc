#ifndef ALOCATA
#define ALOCATA

#include <stdatomic.h>
#include <stddef.h>
//#define ENABLE_WEAK // uncoment to make the memory weak, i.e you can ovride them


#ifdef ENABLE_WEAK 
#define weak  __attribute__((weak))
#else
#define weak
#endif

typedef unsigned char au8       __attribute__((weak));
typedef unsigned short au16     __attribute__((weak));
typedef unsigned int au32       __attribute__((weak));
typedef unsigned long long au64 __attribute__((weak));

struct chunck {
	void* self;      //Hmm, what is this i hear you ask ? will, if we alocate a memory inside a chunck of memory
			 //and want to free it, we have to know wher the meta data is, right?
			 //if it is algined (and it is ) to 4096, what if whe have a big chunk, how we gona finde the meta?
			 //will, to make free, freing, we have to chuck out the last pointer, and that is it. prety usfule

	size_t magicnum; //the magic number, made on the run with copel of stack postition values and some other random number, to make it defrent at the run
			 //wich is prvent free injection code
	chunck* nextchun; //next chunk
	chunck* prevchun; // privuce chunk

	atomic_char lock; // the lock mutext, one bit
	au16 alocatedcount;
	size_t allocateoffset;
	size_t datasize; // this is actioly is ofseted by  12 bits, to get the real size, datasize << 12
	
	unsigned char data[]; // int the most of times, this is 
} __attribute__((packed))  ;




struct  ac_mlctx /*malloc context*/  {
	atomic_char lock;
	au8 intilised;	
	size_t chuncksNumber;
	struct chunck firstchnck;
} __attribute__((packed,aligned(64))); // a none algiment context, is a good call to profrmence hell

struct ac_mlctx _mlctx_ = { 0 };



void ac_lock(au8* lock) weak{
	while (*lock ) {/*waite untile the other thread unlock it*/}
	*lock = 1;
}
void ac_unlock(au8* lock) weak {*lock= 0;}



int ac_newmem(struct chunck* last, size_t size){ // if malloc didnt finde a usble chunck , it calls this
	
}



/***************** HOW IT WORKS *****************\
 * +----------------------------------------------------------------------------+
 * +A|H|      Data            |A|H|                 Data       |H|  DATA        +
 * +----------------------------------------------------------------------------+
 *   |________________________|
 * 		|
 * 	   A Chunck (Header+Data)
 *
 * A: algin offset :
 * 	most of proceseors , ispatioly x86 ones,
 * 	have a digrade in profrmence when the memory is not aligne proparly,
 * 	by default, we are siting it to 16 bytes aligne
 *
 * H: Header:
 * 	right befaure the requasted data, hold some information like the data size, is it free, 
 * 	his it locked by a other thread, and the privuce (NULL if it is the first) chunck,
 * 	and the next chunck (NULL if it is the last),
 * What is malloc doing?:
 * malloc sycle over all the exicting chuncks, if it is free, and in the same or in smaler size, it will requast it, if it nor
 * it wil lcall ac_newmem, wich is work is requast more memory frome the OS, if this last also faile, it will return NULL
 * */



void* malloc(size_t s) weak
{
		
}



void free(void* mem) weak
{
	
}


#endif //ALOCATA
