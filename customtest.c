#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>	
#include "mymalloc.h"

#define MEMLENGTH 4096
#define HEADERSIZE 16
#define OBJECTS 64
#define OBJSIZE (MEMLENGTH / OBJECTS - HEADERSIZE)


//calls free with a pointer not from malloc 
//returns error
void test1(){
    int x;
    free(&x);
}

//calls free with an address not at the start of of a chunk
//returns error
void test2(){
    int *p = malloc(sizeof(int)*2);
    free(p+1);
    free(p);
}

//double frees the pointer
//returns error
void test3(){
    int *p = malloc(sizeof(int)*100);
    int *q = p;
    free(q);
    free(p);
}

//test what happens when memory is full
void test4(){

	char *ptr[OBJECTS];
    for (int i = 0; i < OBJECTS; i++)
    {
        ptr[i] = malloc(OBJSIZE);
    }

    //should return error
    malloc(sizeof(OBJSIZE));
   
   for(int i = 0; i < OBJECTS; i++){
   	free(ptr[i]);
   } 
}

// checks for overlapping objects and to make sure all spots are freed correctly
void test5(){
    for(int i = 0; i < OBJECTS; i++){
        char *ptr = malloc(OBJSIZE);
        free(ptr);
    }

    char *ptr[OBJECTS];
    int errors=0;
    for(int i = 0; i <OBJECTS; i++){
        ptr[i] = malloc(OBJSIZE);
    }

    for(int i = 1; i <OBJECTS; i++){
        if(ptr[i] == ptr[i-1]){
            printf("There are overlapping pointers: %p and %p", ptr[i], ptr[i-1]);
	    errors++;
        }

    }
    
    printf("There are %d errors\n", errors);

    for(int i=0; i<OBJECTS; i++){
	free(ptr[i]);
    }
}

//Make sure writing to one pointer does not affect another pointer
void test6(){

    int *newnum[OBJECTS];
    for(int i = 0; i < OBJECTS; i++){
        newnum[i] = malloc(OBJSIZE);
        *newnum[i] = i;
    }

    for(int i = 0; i < OBJSIZE; i++){
        printf("Pointer %d contains %d\n", i, *newnum[i]);
    }

}

int main(int argc, char** argv){

    test1();
    test2();
    test3();
    test4();
    test5();
    test6();
}
