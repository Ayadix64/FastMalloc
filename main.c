#include <stddef.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include "aloc.h"
typedef long u64;

u64 alloctest(u64 chuncksize){
	printf("Alocating %d MB\n",chuncksize/(1024*1024));
	struct timespec befaure;
	clock_gettime(0,&befaure);
	void * mem = malloc(chuncksize);
	free(mem);
	struct timespec after;
	clock_gettime(0,&after);


	return after.tv_nsec - befaure.tv_nsec;
}


u64 memsettest(u64 chuncksize){
	printf("memset %d MB\n",chuncksize/(1024*1024));
	void * mem = malloc(chuncksize);
	
	struct timespec befaure;
	clock_gettime(0,&befaure);

		
	memset(mem,0,(size_t)chuncksize);
	
	struct timespec after;
	clock_gettime(0,&after);

	free(mem);
	return after.tv_nsec - befaure.tv_nsec;
}

#define TEST(func,size) alloctestsize = size;\
	allocres =  func(alloctestsize);\
	printf(#func" der %d µs (%ld MB/s)\n",(allocres * 1000000 )/1000000000,(u64)((float)alloctestsize/(float)((float)allocres/(float)1000000000))/(1024*1024));





int main(){
	u64 allocres=0;
	u64 alloctestsize=0;
	TEST(alloctest,0x1000*1024*10);
	TEST(memsettest,0x1000*1024*10);
}
