#include <stdio.h>
#include <stdlib.h>
#include "mymalloc.h"
#include <stdint.h>		/* used for casting clock_t to uintmax_t for printf */
#include <time.h>

#define ITERATIONS 50
#define ALLOCATIONS 120

#if defined(_WIN32) || defined(_WIN64) 
// Windows
#include <windows.h>

long long current_time_in_microseconds() {
    LARGE_INTEGER frequency, counter;
    QueryPerformanceFrequency(&frequency);
    QueryPerformanceCounter(&counter);
    return (counter.QuadPart * 1000000) / frequency.QuadPart;
}

#elif defined(__unix__) || defined(__APPLE__)
// Unix-based system (Linux, macOS)
#include <sys/time.h>

long long current_time_in_microseconds() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec * (long long)1000000 + tv.tv_usec;
}

#else
// Other platforms
// This is a simple fallback using C's standard library. It has only second resolution.
long long current_time_in_microseconds() {
    return (long long)time(NULL) * 1000000;
}

#endif

////////////////////////////////////////
// Allocates and then frees 120 times //
////////////////////////////////////////
void test1() {
    for (int i = 0; i < ALLOCATIONS; i++) {
        char *ptr = (char *)malloc(1);
        free(ptr);
    }
}

//////////////////////////////////////////////////////////////////////
// Puts pointers into an array and allocates them, then frees them. //
//////////////////////////////////////////////////////////////////////
void test2() {
    char *ptrs[ALLOCATIONS];
    for (int i = 0; i < ALLOCATIONS; i++) {
        ptrs[i] = (char *)malloc(1);
        //printf("Malloc: iteration: %d\n", i);
    }
    for (int i = 0; i < ALLOCATIONS; i++) {
        //printf("Entering free\n");
        //printf("i is: %d\n", i);
        //printf("ptr: %d", i, " is equal to: %d\n", *ptrs[i]);
        free(ptrs[i]);
        //printf("Free: iteration: %d\n", i);
    }
}

void test3() {
    char *ptrs[ALLOCATIONS];
    int count = 0;
    int ptrs_count = 0;

    while (count < ALLOCATIONS) {
        if (rand() % 2 == 0 || ptrs_count == 0) {
            ptrs[ptrs_count] = (char *)malloc(1);
            ptrs_count++;
            count++;
        } else if (ptrs_count > 0) {
            ptrs_count--;
            free(ptrs[ptrs_count]);
        }

    }

    for (int i = 0; i < ptrs_count; i++) {
        free(ptrs[i]);
    }
}

// CHECK IF MALLOC CAN HANDLE DIFFERENT SIZES BEING ALLOCATED
void test4() {
	
	// Because we use a random size, using ALLOCATIONS produces "not big enough chunk" 
	// error, so we use less allocations for this
	int safealloc = 85;
	char *ptrs[safealloc];
    for (int i = 0; i < safealloc; i++) {
        size_t size = (rand() % 32) + 1; // random size between 1 and 32
        ptrs[i] = (char*)malloc(size);
	//printf("Pointer %d = %d\n", i, *ptrs[i]);
    }

    for (int i = 0; i < safealloc; i++) {
	free(ptrs[i]);
    }
}

// CHECK IF FREEING EVERY OTHER POINTER THEN FREE THE REST OF THE POINTERS WORKS
void test5() {
    char * ptrs[ALLOCATIONS];

    for (int i = 0; i < ALLOCATIONS; i++) {
        if(i % 2 == 0){
            ptrs[i] = (char*)malloc(10);
        }
        else {
             ptrs[i] = (char*)malloc(5);
        }
    }

    // Free every odd ptr
    for (int i = 1; i < ALLOCATIONS; i+=2) {
        free(ptrs[i]);
    }

    for (int i = 0; i < ALLOCATIONS; i+=2) {
        free(ptrs[i]);
    }
}

int main(int argc, char** argv) {

    long long start_time, end_time;
    long long times[5] = {0, 0, 0, 0, 0};
    for (int i = 0; i < ITERATIONS; i++) {
        start_time = current_time_in_microseconds();
        test1();
        end_time = current_time_in_microseconds();
        times[0] += (end_time - start_time);

        start_time = current_time_in_microseconds();
        test2();
        end_time = current_time_in_microseconds();
        times[1] += (end_time - start_time);

        start_time = current_time_in_microseconds();
        test3();
        end_time = current_time_in_microseconds();
        times[2] += (end_time - start_time);

        start_time = current_time_in_microseconds();
        test4();
        end_time = current_time_in_microseconds();
        times[3] += (end_time - start_time);

        start_time = current_time_in_microseconds();
        test5();
        end_time = current_time_in_microseconds();
        times[4] += (end_time - start_time);
        //printf("ITERATION: %d\n", i);
    }

    // Gonna need to change this to i < 5
    for (int i = 0; i < 5; i++) {
        printf("Average time for test %d: %.2f microseconds\n", i+1, (double)times[i] / ITERATIONS);
    }

    return 1;
}
