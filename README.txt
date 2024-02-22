Project 1: My Little malloc()

Andrew Bishara- ab2761
Blake Rears- bar181



Design Notes:

    -> We chose to return null after printing out error messages 
        -> This makes it so thee user can choose how to deal with the error in their program, instead
         of us choosing to kill their program for them. 

    -> We chose to format errors in the following manor: "Error: <Desciption of Errorr>! <File>, <Line>"
        -> This is for clarity and to make it easy to figure out your problem if one does occur. 
        
    -> Memory (Malloc):
        -> Memory Block Structure: Every block of memory managed by this custom malloc function is prefixed
         with 'metadata'. This metadata provides details like the size of the allocated block and its 
         status (Used or Free). We decided to go with a struct design because we thought that it would be 
         easier to use arithmetic to transverse the memory array.
        -> Malloc Strategy: Our strategy for allocating memory was to iterate through the entire array and
         search for a spot where the data could fit and was also free.

    -> Coalescence:
        -> Iterative Coalescence: Instead of incorporating a doubly linked list structure to get the 
         previous metadata, we decided to take an iterative approach. When freeing a block of memory, we 
         iterate through the entire memory to merge any adjacent free blocks.
        -> We decided to do it this way because of the difficulty in identifying the start of the previous
         metadata block without a back-pointer.

    -> Edge Case Handling:
        -> Storing data at the edge of memory: One challenge faced during the implementation was deciding
         how to handle allocations near the end of the memory space. At first, we had the metadata for the
          next block being written at the very end, even if there was no actual space left for another block.
        -> The solution, a design modification ensures that if an allocation reaches the end of the memory,
         the system does not attempt to append metadata for the next block. This is because it's meaningless
         to provide metadata for a block that can't exist due to space constraints.


Test Plan:

    The first thing we did while testing was make the following list:

        Required functionalities of mymalloc():
            -> Calling free() with an address not obtained from malloc() returns error
                Can be checked by writing a program that calls free() with an address of 
                a variable
            -> Calling free() with an address not at the start of a chunk returns error
                Can be checked by writing a program that calls free() with a pointer
                returned by malloc() + 1 byte
            -> Double freeing a chunk returns error
                Can be checked by writing a program that calls free() twice with the 
                same pointer
            -> free() marks chunks free
                Can be checked by writing/using DEBUG data in malloc.c to see what is 
                getting written to memory and where it's getting written
            -> free() coallesces adjacent free chunks
                Can be checked by writing/using DEBUG data in malloc.c in malloc.c to see what is 
                getting written to memory and where it's getting written
            -> Writing to one payload does not affect another
                Can be checked by writing program that fills memory with distinct reconizable bytes 
                then checks if any bytes are wrong
            -> Returns NULL if there is not a large enough chunk to allocate requested size
                Can be checked by writing a program that fills memory then tries to allocate more
                data

    Using this list, we came up with a few different tests to check and make sure all of these
    functionalities were working properly with our malloc(). A couple requirements were easiest to 
    check by putting debug data in mymalloc.c, then compiling it while defining debug. These ones 
    include checking that calling free marks chunks free and calling free coallesces free chunks.
    THe other functionalities were checked by writing programs/functions to call malloc() and free() either
    correctly or with a specific problem to check for. These are described below under Testing Strategies. 


Testing strategies:

    customtest:
        This is a custom program designed to print errors for a variety of misuses of mymalloc() and
        myfree(). Each on tests a specific component of the mymalloc() that we were required to
        check for. Each test's purpose is as follows:

            -> test1: check error handling for calling free() with address not obtained from  malloc()
            -> test2: check error handing for calling free() with a pointer that doesn't correspond 
                to a payload
            -> test3: check error handling for calling free() twice on the same pointer
            -> test4: check error handling for calling malloc() while memory is already full
            -> test5: check both to make sure there are no overlaps in memory and to make sure 
                that allocating data then freeing all of it results in a clear array
            -> test6: check to see if writing to one pointer returned by malloc() affects any 
                other data in the memory

    memgrind:
        This program performs 5 stress tests on our malloc() and free() functions. The first three
        were outlined in the project write up with the purpose of making sure malloc() can be 
        functional and efficient. These three tests track how long it takes malloc() and free()
        to do a given amount of iterations of both, then give the average time it takes. For our
        two stress tests we were asked to design

            -> the first one checks to see how our malloc() handles allocating different sizes of 
                data. We called malloc a variable amount of times, each with a different random
                number between 1 and 32. Then we called free() on all data we just put in. Most
                of the time, this would result in the program trying to free more memory than 
                available, so it also tests error handling for calling malloc() while the memory
                is already full, then tests error handling for calling a free pointer.
            -> our second tests checks to make sure that freeing data around a pointer does not
                affect that pointer. In other words, if you have a payload pointer then free the
                data immediately before and immediately after that pointer, the pointer's data 
                should not be affected. We checked this by filling up the memory array by calling
                malloc() a variable number of times, each time rotating between requesting 5 and 
                requesting 10 bytes of allocated memory. Once the memory was full, we called free()
                on every other pointer, then went back and called it on the remaining pointers. 

        design note:
            We used different timing methods depending on what OS your machine is using because 
            we are both using different operating sytems. On macOS and Unix machines, we used the 
            gettimeofday() function, but for windows we used a LARGE INTEGER struct as defined in 
            windows.h. We also included a general case if your machine does not have any of these
            three operaing systems.

    memtest:
        memtest is a simple program given by you that tests basic functionality of malloc(). It fills the
        memory with a given number of objects based on a given memory size, header size, and how
        many objects you want. It then fills each object with a distinct integer. Then it checks 
        to make sure that all of the objects have the correct bytes in them. This program ensures 
        that filling up the memory array is functional and does not result in any unwanted data 
        getting written over when allocating memory. 
