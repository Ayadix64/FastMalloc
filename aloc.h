#ifndef ALOCATA
#define ALOCATA

#include <stdio.h>
#include <stdatomic.h>
#include <stddef.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
//#define ENABLE_WEAK // uncoment to make the memory weak, i.e you can ovride them


#ifdef ENABLE_WEAK 
#define weak  __attribute__((weak));
#else
#define weak
#endif

typedef unsigned char au8       __attribute__((weak));
typedef unsigned short au16     __attribute__((weak));
typedef unsigned int au32       __attribute__((weak));
typedef unsigned long long au64 __attribute__((weak));

#define PAGESZ 0x1000

struct chunck {
	void* self;      //Hmm, what is this i hear you ask ? will, if we alocate a memory inside a chunck of memory
			 //and want to free it, we have to know wher the meta data is, right?
			 //if it is algined (and it is ) to 4096, what if whe have a big chunk, how we gona finde the meta?
			 //will, to make free, freing, we have to chuck out the last pointer, and that is it. prety usfule

	size_t magicnum; //the magic number, made on the run with copel of stack postition values and some other random number, to make it defrent at the run
			 //wich is prvent free injection code
	struct chunck* nextchun; //next chunk
	struct chunck* prevchun; // privuce chunk

	atomic_char lock; // the lock mutext, one bit
	au16 alocatedcount;
	size_t allocatoffset;
	size_t datasize; // simple and strate forward
	
	unsigned char data[]; // int the most of times, this is 
} __attribute__((packed))  ;



#define MALLOC_CHUNCK_HEADER_SIZE sizeof(struct chunck)



struct  ac_mlctx /*malloc context*/  {
	atomic_char lock;
	atomic_char intilised;	
	size_t chuncksNumber;
	struct chunck* firstchnck;
	size_t magic;
} __attribute__((packed,aligned(64))); // a none algiment context, is a good call to profrmence hell

struct ac_mlctx _mlctx_ = {.lock=0,.intilised=0,.chuncksNumber=0,.firstchnck=NULL};


/******************* utilitis ******************/


void ac_lock(atomic_char * lock) weak{
	while (*lock ) {/*waite untile the other thread unlock it*/}
	*lock = 1;
}
void ac_unlock(atomic_char* lock) weak {*lock= 0;}

/**********************************************/



size_t acGenrateMagicNumber(){
	size_t stackelemnt;
	return stackelemnt * 489132 * rand();

}



int alocatenewchunck(struct chunck** cnk, size_t s  ){ // if malloc didnt finde a usble chunck , it calls this
	void* curnetbrk = sbrk(0);
	if (curnetbrk == (void*)-1){return -1;}
	size_t add2brk = ( (size_t)curnetbrk+MALLOC_CHUNCK_HEADER_SIZE+PAGESZ-1) &((~(size_t)0)<<12); // algine to the next page
	
	if(brk((void*)(((size_t)add2brk+s+PAGESZ-1)&((~(size_t)0)<<12)) )){ // if it is refuces to add the data segment size, we retuen 
		return -1;	
	}
	*cnk = (struct chunck*)((size_t)add2brk);
	struct chunck *_chunck = *cnk;
	_chunck->allocatoffset=0;
	_chunck->alocatedcount=1;
	_chunck->self = *cnk;
	_chunck->datasize=(s+PAGESZ-1)&((~(size_t)0)<<12);
	_chunck->allocatoffset=s;
	_chunck->lock=0;
	//printf("allocate %x\n",_chunck);
	/*for (size_t i = 0 ; i < (s+PAGESZ-1) & ((~(size_t)0)<<12); i++){
		_chunck->data[i]=0;
	}*/
	return 0;
}



/***************** HOW IT WORKS *****************
 * +----------------------------------------------------------------------------+
 * +A|H|      Data            |H|              Data            |H|  DATA        +
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
 * it will call alocatenewchunck, wich is work is requast more memory frome the OS, if this last also faile, it will return NULL
 * if the alocated memory is smaler than 4096 (aka. page size) and ther is a free memory in the last chunck, it is just add the header adress, and then
 * addit to the the chunck, with adding a one to alocatedcount and adding the size of it + sizerof(void*) to the allocateoffset
 * */



void* amalloc(size_t s) weak
{
	void* ret=NULL;
	if(!_mlctx_.intilised){
		if(!_mlctx_.lock){ // no thread are intilising the context, we are good
			ac_lock(&_mlctx_.lock);
		}else {
			while(_mlctx_.lock);;
			goto ___aclc___; /*some one intilising it, trust him and waite to untile he done is work*/
		}
		/*we are supsud to intilisre the context, so we will*/
		_mlctx_.chuncksNumber=1;
		if (alocatenewchunck(&_mlctx_.firstchnck,s) == -1){
			return NULL;
		}
		
		ret = _mlctx_.firstchnck->self  + MALLOC_CHUNCK_HEADER_SIZE;
		_mlctx_.intilised=true;
		for (size_t j = 0 ; j < s ; j++){
			_mlctx_.firstchnck->data[j]=0;
		}

		return ret;
		
	}
	
___aclc___:
	struct chunck* cchunck=_mlctx_.firstchnck;
	
	au32 syclesbefauregiveup = 0; 
	// heh, dont give up!, the give up hear is reaquasting a new memory frome the kernel, maxmating 5 sycles, evry sycle demand one Migabyte of data, more than 5, constant
	if (_mlctx_.chuncksNumber >= 3){
		syclesbefauregiveup = s / (1024*1024) < 5? s /(1024*1024):5;
	}
	au32 sycls=0;

	for (int i = 0 ; i < _mlctx_.chuncksNumber ; i++){
		if (!cchunck->lock){ //if for what ever reasen this was locked, we are trust the other thread that it will be ether use or already used
			ac_lock(&cchunck->lock); // lock it
			if(!cchunck->alocatedcount){ //if it was free, dare i ask if the next cchunck is also free?
				if(cchunck->nextchun && !cchunck->nextchun->lock)
					ac_lock(&cchunck->nextchun->lock);//lock it as well
					if(!cchunck->nextchun->alocatedcount){ //if this also free, fuce them
						cchunck->datasize = (MALLOC_CHUNCK_HEADER_SIZE + cchunck->nextchun->datasize);
						cchunck->nextchun = cchunck->nextchun->nextchun;
					}
					ac_unlock(&cchunck->nextchun->lock);
				}else { // if the next chunck is not free, just return this
					for (size_t j = 0 ; j < s ; j++){
						cchunck->data[j]=0;
					}
					cchunck->alocatedcount++;
					ac_unlock(&cchunck->lock);

					return cchunck + MALLOC_CHUNCK_HEADER_SIZE;
				}
			}else if (cchunck->allocatoffset+sizeof(void*)+s < cchunck->datasize){
				
			}
			ac_unlock(&cchunck->lock);
		}
		cchunck = cchunck->nextchun;
	}
	return ret;

}



void afree(void* mem) weak
{
		
}


#endif //ALOCATA
