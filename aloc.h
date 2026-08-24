#ifndef ALOCATA
#define ALOCATA

#include <stddef.h>
//#define ENABLE_WEAK // uncoment to make the memory weak, i.e you can ovride them


#ifdef ENABLE_WEAK 
#define weak  __attribute__((weak))
#else
#define weak
#endif


struct meta {
	size_t magicnum; //the magic number, made on the run with copel of stack postition values and some other random number, to make it defrent at the run
			 //wich is prvent free injection code
	meta* nextblk;
	meta* prevblc;
	size_t datasize;
	
	unsigned char data[];
} __attribute__((packed))  ;



void* malloc(size_t s) weak
{


}



void free(void* mem) weak
{
	
}


#endif //ALOCATA
