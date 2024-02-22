#include <stdio.h>
#include <stdlib.h>
#include "mymalloc.h"

#define MEMLENGTH 512
#define HEADERSIZE 8
#define OBJECTS 64
#define OBJSIZE (MEMSIZE / OBJECTS - HEADERSIZE)

static double memory[MEMLENGTH]; // Just an array of bytes

typedef struct Metadata {
    size_t size;
    int is_free;
} Metadata;

size_t align8(size_t size) {
    return (size + 7) & ~7;  // Rounds up to nearest multiple of 8
}

void memory_initialize() {
    Metadata* initial_meta = (Metadata*) memory;
    initial_meta->is_free = 1;
    initial_meta->size = MEMLENGTH * sizeof(double) - sizeof(Metadata); // This initializes the entire memory minus the space taken by the initial metadata
}


void* mymalloc(size_t size, const char *file, int line) {
    if (size == 0) {
        printf("ERROR: you're trying to allocate 0 bytes! File: %s, Line: %d\n", file, line);
        return NULL;
    }

    Metadata* initial_meta = (Metadata*) memory;
#ifdef DEBUG
    printf("Request to allocate %zu bytes\n", size);
#endif
    // Check if the memory is uninitialized
     if (initial_meta->size == 0) {
        //printf("Intializing memory for first time\n");
        memory_initialize();
    }

    size = align8(size);  // Aligned to 8 bytes
#ifdef DEBUG
    printf("Adjusted size (8-byte alignment): %zu bytes\n", size);
#endif

    char* current = (char*)memory;

    while (current < (char*)memory + MEMLENGTH * sizeof(double)) {
        Metadata* meta = (Metadata*)current;
        //printf("Current metadata at %p: size = %zu, is_free = %d\n", meta, meta->size, meta->is_free);

        // If the block is free and has enough size
        if (meta->is_free && meta->size >= size) {
#ifdef DEBUG
            printf("Found suitable free block at %p\n", current);
#endif
            // Update metadata to flag it as used
            meta->is_free = 0;
            size_t originalSize = meta->size;
            meta->size = size;

            // Calc space needed for data and new metadata
            size_t totalAllocationSize = sizeof(Metadata) + size;

            // Determines where the next metadata block should start
            char* nextBlockStart = current + align8(totalAllocationSize);

            // Determines the end position for the next block if it were to have the minimum size allocation
            char* nextBlockEnd = nextBlockStart + sizeof(Metadata) + align8(1); 

            // Check if space to place the next metadata block and at least 1 byte of data
            if (nextBlockEnd <= (char*)memory + MEMLENGTH * sizeof(double)) {
                Metadata* nextMeta = (Metadata*)nextBlockStart;
                nextMeta->is_free = 1;
                nextMeta->size = originalSize - totalAllocationSize;
#ifdef DEBUG
                printf("Created new metadata block at %p with size %zu\n", nextMeta, nextMeta->size);
#endif
            } else {
#ifdef DEBUG
                printf("Not enough space for another block, using remaining space\n");
#endif
                meta->size = originalSize;  // Take the remaining space
            }

#ifdef DEBUG
            printf("Allocated block at %p, returning %p\n", current, current + sizeof(Metadata));
#endif
            // Return a pointer to the usable memory
            return current + sizeof(Metadata);
        }

        // Move to the next block
        current += sizeof(Metadata) + meta->size;
    }

    printf("ERROR: can't allocate memory. No room left! File: %s, Line: %d\n", file, line);
    return NULL;
}

#define FREE 1
#define USED 0

// This loop will basically loop through the entire memory array in check for 2 adjacent free metadata blocks
//// When coalescing takes place we reiterate through the entire array again till we coalesce or reach the end of memory
void coalesce() {

    int is_coalesced;

    do {
        is_coalesced = 0;
        char* current = (char*)memory;

        while (current < (char*)memory + MEMLENGTH * sizeof(double)) {
            Metadata* meta = (Metadata*)current;

            // Check if this block is free
            if (meta->is_free == FREE) {
                // Check the adjacent block
                Metadata* adjacentMeta = (Metadata*)((char*)meta + sizeof(Metadata) + meta->size);

                // If adjacentMeta is within bounds and is also free
                if ((char*)adjacentMeta < (char*)memory + MEMLENGTH * sizeof(double) && adjacentMeta->is_free == FREE) {
                    // Coalesce the blocks
                    meta->size += sizeof(Metadata) + adjacentMeta->size;
                    is_coalesced = 1;  // set the flag
                } else {
                    // move to next metadata block if coalescing did not happen
                    current += sizeof(Metadata) + meta->size;
                }
            } else {
                // move to next metadata block if current block is not free
                current += sizeof(Metadata) + meta->size;
            }
        }
    } while (is_coalesced);  // repeat the loop if coalescing happened
}

void myfree(void* ptr, const char *file, int line) {
    if (ptr == NULL) {
        printf("ERROR: pointer is null! File: %s, Line: %d\n", file, line);
        return;
    }

    char* current = (char*)memory;
    int found = 0;
    
    while (current < (char*)memory + MEMLENGTH * sizeof(double)) {
        Metadata* meta = (Metadata*)current;

        // Pointer matches the start of a user memory block
        if (current + sizeof(Metadata) == (char*)ptr) {
            found = 1;
            break;
        }
        
        // Pointer is within an allocated block but not at the start
        if (current + sizeof(Metadata) < (char*)ptr && current + sizeof(Metadata) + meta->size > (char*)ptr) {
            printf("ERROR: Attempted to free a pointer not at the start of a data block. File: %s, Line: %d\n", file, line);
            return;
        }

        // Move to the next block
        current += sizeof(Metadata) + meta->size;
    }

    if (found == 0) {
        printf("ERROR: Attempted to free a pointer not allocated by malloc. File: %s, Line: %d\n", file, line);
        return;
    }

    Metadata* meta = (Metadata*)((char*)ptr - sizeof(Metadata));
    if (meta->is_free == FREE) {
        printf("ERROR: double free! File: %s, Line: %d\n", file, line);
        return;
    }
    
    meta->is_free = FREE;  // Flags the metadata as free

    coalesce();
}

