////////////////////////////////////////////////////////////////////////////////
// Main File:        mem.c
// This File:        mem.c
// Other Files:      N/A
// Semester:         CS 354 Fall 2018
//
// Author:           YUXUAN LIU
// Email:            liu686@wisc.edu
// CS Login:         yuxuan
//
/////////////////////////// OTHER SOURCES OF HELP //////////////////////////////
//                   fully acknowledge and credit all sources of help,
//                   other than Instructors and TAs.
//
// Persons:          N/A
//                   
//
// Online sources:   N/A
//                   
//                   
//////////////////////////// 80 columns wide ///////////////////////////////////

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <string.h>
#include "mem.h"

/*
 * This structure serves as the header for each allocated and free block
 * It also serves as the footer for each free block
 * The blocks are ordered in the increasing order of addresses 
 */
typedef struct blk_hdr {                         
    int size_status;
  
    /*
    * Size of the block is always a multiple of 8
    * => last two bits are always zero - can be used to store other information
    *
    * LSB -> Least Significant Bit (Last Bit)
    * SLB -> Second Last Bit 
    * LSB = 0 => free block
    * LSB = 1 => allocated/busy block
    * SLB = 0 => previous block is free
    * SLB = 1 => previous block is allocated/busy
    * 
    * When used as the footer the last two bits should be zero
    */

    /*
    * Examples:
    * 
    * For a busy block with a payload of 20 bytes (i.e. 20 bytes data + an additional 4 bytes for header)
    * Header:
    * If the previous block is allocated, size_status should be set to 27
    * If the previous block is free, size_status should be set to 25
    * 
    * For a free block of size 24 bytes (including 4 bytes for header + 4 bytes for footer)
    * Header:
    * If the previous block is allocated, size_status should be set to 26
    * If the previous block is free, size_status should be set to 24
    * Footer:
    * size_status should be 24
    * 
    */
} blk_hdr;

/* Global variable - This will always point to the first block
 * i.e. the block with the lowest address */
blk_hdr *first_blk = NULL;

/*
 * Note: 
 *  The end of the available memory can be determined using end_mark
 *  The size_status of end_mark has a value of 1
 *
 */

/* 
 * Function for allocating 'size' bytes
 * Returns address of allocated block on success 
 * Returns NULL on failure 
 * Here is what this function should accomplish 
 * - Check for sanity of size - Return NULL when appropriate 
 * - Round up size to a multiple of 8 
 * - Traverse the list of blocks and allocate the best free block which can accommodate the requested size 
 * - Also, when allocating a block - split it into two blocks
 * Tips: Be careful with pointer arithmetic 
 */                    
void* Alloc_Mem(int size) 
{                 
    if (size < 0)
    {
        return NULL;
    }   
    // check for sanity of size
    
    size += 4;
    if (size < 8)
    {
        size = 8;
    }
    if (size % 8 != 0)
    {
        size += (8 - (size % 8));
    }
    // process the size to include the header and padding
    
    blk_hdr *temp = (void*)first_blk; 
    blk_hdr *minPtr = NULL;  
    // pointer to the minimum block we have already found
    int tempSize = temp -> size_status;
    int minBlockSize = 0;
    // the current size of the minimum block, used in minimum block comparison
    
    while (tempSize != 1)  // while we have not reached the end of the list
    {   
        if ((tempSize) % 2 != 0) 
        {
            temp = (blk_hdr*)((void*)temp + ((tempSize) - (tempSize) % 8)); 
            // if the current block is allocated, continue searching
        } else {
            if ((tempSize - 2) < size) 
            {
                temp = (blk_hdr*)((void*)temp + tempSize - 2);
                // if the current block do not have enough space, 
                // continue searching
            } else {
                int actualSize = tempSize - 2;
                if (minPtr == NULL)
                {
                    minBlockSize = actualSize;
                    minPtr = temp;
                    // first iteration initialization
                } else if (actualSize < minBlockSize) {
                    minBlockSize = actualSize;
                    minPtr = temp;
                    // if we found a better block, 
                    // update the minumum size and pointer
                }
                temp = (blk_hdr*)((void*)temp + actualSize);
            }
        }
        tempSize = temp -> size_status;
    }
    
    if (minPtr != NULL) 
    {
        // if we found a block that can fit the requested size,
        // perform the allocation
        int actualSize = minPtr -> size_status - 2;
        (minPtr -> size_status) = size + 3;
        // we make the free block busy
        blk_hdr *result = (blk_hdr*)((void*)minPtr + 4);
        // notice we need to return the address of the block's payload
        if (actualSize - size >= 8) 
        {
            minPtr = (blk_hdr*)((void*)minPtr + size);
            minPtr -> size_status = actualSize - size + 2;
            minPtr = (blk_hdr*)((void*)minPtr + actualSize - size - 4);
            minPtr -> size_status = actualSize - size;
            return result;
            // if we have leftover space no less than 8 bytes, 
            // we need to do splitting
        } else {
            minPtr = (blk_hdr*)((void*)minPtr + actualSize);
            minPtr -> size_status += 2;
            return result;
            // else simply update the size_status of the next block
            // Notice we assume all blocks are of size mutiple of 8,
            // in this situation there's no leftover space in the 
            // free block after the allocation
        }
    }
    return NULL;  
    // Notice that when the requested size is greater than 
    // the initialized memory, this function correctly retruns NULL
}

/* 
 * Function for freeing up a previously allocated block 
 * Argument - ptr: Address of the block to be freed up 
 * Returns 0 on success 
 * Returns -1 on failure 
 * Here is what this function should accomplish 
 * - Return -1 if ptr is NULL
 * - Return -1 if ptr is not 8 byte aligned or if the block is already freed
 * - Mark the block as free 
 * - Coalesce if one or both of the immediate neighbours are free 
 */                    
int Free_Mem(void *ptr) 
{
    blk_hdr* first = first_blk;
    blk_hdr* current = ptr;
    blk_hdr *end = NULL;
    blk_hdr *temp = (void*)first_blk; 
    // initialize pointers used to check the range of ptr
    int currSize = temp -> size_status;
    while (currSize != 1)   // while we have not reached the end mark
    {   
        temp = (blk_hdr*)((void*)temp + ((currSize) - (currSize) % 8)); 
        currSize = temp -> size_status;
    }
    // iterate through the list of blocks
    end = temp;
    if (current >= end || current < first){
        return -1;
    }
    // if ptr is not within valid range, return -1
    
    ptr = (blk_hdr*)((void*)ptr - 4);
    // set the address of pointer to teh address of the header
    
    if (ptr == NULL || ((int)((void*)ptr + 4)) % 8 != 0 
                    || ((blk_hdr*)ptr) -> size_status % 2 == 0)
    {
        return -1;
    } 
    // check the input and return -1 if it is illeagal
    
    int prevSize = 0;
    int sucSize = 0;
    // size of previous block and successor block
    ((blk_hdr*)ptr) -> size_status -= 1;
    // make the current block free
    int tempSize = ((blk_hdr*)ptr) -> size_status;
    
    int actualSize = ((blk_hdr*)ptr) -> size_status;
    if (actualSize % 8 != 0)
    {
        actualSize -= 2;
    }
    // if the previous block is free, current block's
    // size_status must be a multiple of 8. in this sense,
    // if previous block is allocate, actualSize needs to subtract 2 bytes
    
    if (ptr == first_blk)  // if we are freeing the first block
    {
        if (((blk_hdr*)((void*)ptr + actualSize)) -> size_status % 2 == 0)
        {   
            // if the next block is free, do coalescing
            int sizeCount = actualSize;
            sucSize = ((blk_hdr*)((void*)ptr + actualSize)) -> size_status;
            sucSize -= 2;
            sizeCount = sizeCount + sucSize;
            ((blk_hdr*)ptr) -> size_status = sizeCount + 2;
            // update footer
            ptr = (blk_hdr*)((void*)ptr + actualSize + sucSize - 4);
            ((blk_hdr*)ptr) -> size_status = sizeCount;
        } else {
            // else set the footer and update the size_status of next block
            ptr = (blk_hdr*)((void*)ptr + actualSize - 4);
            ((blk_hdr*)ptr) -> size_status = actualSize;
            ptr = (blk_hdr*)((void*)ptr + 4);
            ((blk_hdr*)ptr) -> size_status -= 2;
        }
        return 0;
    } else if (((blk_hdr*)((void*)ptr + actualSize)) -> size_status == 1) {
        // if we are freeing the last block
        if (tempSize % 8 == 0)
        {
            // if the next previous is free, do coalescing
            prevSize = ((blk_hdr*)((void*)ptr - 4)) -> size_status;
            blk_hdr *prev = ((blk_hdr*)((void*)ptr - prevSize));
            int sizeCount = actualSize;
            // successor coalescing
            sizeCount = sizeCount + prevSize;
            prev -> size_status = sizeCount + 2;
            ptr = (blk_hdr*)((void*)ptr + actualSize - 4);
            ((blk_hdr*)ptr) -> size_status = sizeCount;
        } else {
            // else simply set the footer
            ptr = (blk_hdr*)((void*)ptr + actualSize - 4);
            ((blk_hdr*)ptr) -> size_status = actualSize;
        }
        return 0;
    } else if ((tempSize % 8 == 0) 
        && ((blk_hdr*)((void*)ptr + actualSize)) -> size_status % 2 == 0) {
        // if both the previous block and successor block is free
        prevSize = ((blk_hdr*)((void*)ptr - 4)) -> size_status;
        int sizeCount = actualSize;
        blk_hdr *prev = ((blk_hdr*)((void*)ptr - prevSize));
        sucSize = ((blk_hdr*)((void*)ptr + actualSize)) -> size_status - 2;
        sizeCount = sizeCount + sucSize + prevSize;
        // do coalescing on both sides
        prev -> size_status = sizeCount + 2;
        // update footer
        ptr = (blk_hdr*)((void*)ptr + actualSize + sucSize - 4);
        ((blk_hdr*)ptr) -> size_status = sizeCount;
        return 0;
    } else if (tempSize % 8 == 0) {
        // if only the previous block is free, do front coalescing
        prevSize = ((blk_hdr*)((void*)ptr - 4)) -> size_status;
        int sizeCount = actualSize;
        blk_hdr *prev = ((blk_hdr*)((void*)ptr - prevSize));    
        sizeCount = sizeCount + prevSize;
        prev -> size_status = sizeCount + 2;
        // update successor block
        ptr = (blk_hdr*)((void*)ptr + actualSize - 4);
        ((blk_hdr*)ptr) -> size_status = sizeCount;
        ptr = (blk_hdr*)((void*)ptr + 4);
        ((blk_hdr*)ptr) -> size_status -= 2;
        return 0;
    } else if (((blk_hdr*)((void*)ptr + actualSize)) -> size_status % 2 == 0) {
        // if only the succesor block is free, do end coalescing
        int sizeCount = actualSize;
        sucSize = ((blk_hdr*)((void*)ptr + actualSize)) -> size_status - 2;
        sizeCount = sizeCount + sucSize;
        ((blk_hdr*)ptr) -> size_status = sizeCount + 2;
        // update footer
        ptr = (blk_hdr*)((void*)ptr + actualSize + sucSize - 4);
        ((blk_hdr*)ptr) -> size_status = sizeCount;
        return 0;
    } else if ((tempSize % 8 != 0) 
        && ((blk_hdr*)((void*)ptr + actualSize)) -> size_status % 2 != 0) {
        // if there's no space both in the front and end, 
        // simply free the block
        ptr = (blk_hdr*)((void*)ptr + actualSize - 4);
        ((blk_hdr*)ptr) -> size_status = actualSize;
        ptr = (blk_hdr*)((void*)ptr + 4);
        ((blk_hdr*)ptr) -> size_status -= 2;
        return 0;
    }
    return -1;   // if there's a problem, return -1
}

/*
 * Function used to initialize the memory allocator
 * Not intended to be called more than once by a program
 * Argument - sizeOfRegion: Specifies the size of the chunk which needs to be allocated
 * Returns 0 on success and -1 on failure 
 */                    
int Init_Mem(int sizeOfRegion)
{                         
    int pagesize;
    int padsize;
    int fd;
    int alloc_size;
    void* space_ptr;
    blk_hdr* end_mark;
    static int allocated_once = 0;
  
    if (0 != allocated_once) {
        fprintf(stderr, 
        "Error:mem.c: Init_Mem has allocated space during a previous call\n");
        return -1;
    }
    if (sizeOfRegion <= 0) {
        fprintf(stderr, "Error:mem.c: Requested block size is not positive\n");
        return -1;
    }

    // Get the pagesize
    pagesize = getpagesize();

    // Calculate padsize as the padding required to round up sizeOfRegion 
    // to a multiple of pagesize
    padsize = sizeOfRegion % pagesize;
    padsize = (pagesize - padsize) % pagesize;

    alloc_size = sizeOfRegion + padsize;

    // Using mmap to allocate memory
    fd = open("/dev/zero", O_RDWR);
    if (-1 == fd) {
        fprintf(stderr, "Error:mem.c: Cannot open /dev/zero\n");
        return -1;
    }
    space_ptr = mmap(NULL, alloc_size, PROT_READ | PROT_WRITE, MAP_PRIVATE, 
                    fd, 0);
    if (MAP_FAILED == space_ptr) {
        fprintf(stderr, "Error:mem.c: mmap cannot allocate space\n");
        allocated_once = 0;
        return -1;
    }
  
     allocated_once = 1;

    // for double word alignement and end mark
    alloc_size -= 8;

    // To begin with there is only one big free block
    // initialize heap so that first block meets 
    // double word alignement requirement
    first_blk = (blk_hdr*) space_ptr + 1;
    end_mark = (blk_hdr*)((void*)first_blk + alloc_size);
  
    // Setting up the header
    first_blk->size_status = alloc_size;

    // Marking the previous block as busy
    first_blk->size_status += 2;

    // Setting up the end mark and marking it as busy
    end_mark->size_status = 1;

    // Setting up the footer
    blk_hdr *footer = (blk_hdr*) ((char*)first_blk + alloc_size - 4);
    footer->size_status = alloc_size;
  
    return 0;
}

/* 
 * Function to be used for debugging 
 * Prints out a list of all the blocks along with the following information i
 * for each block 
 * No.      : serial number of the block 
 * Status   : free/busy 
 * Prev     : status of previous block free/busy
 * t_Begin  : address of the first byte in the block (this is where the header starts) 
 * t_End    : address of the last byte in the block 
 * t_Size   : size of the block (as stored in the block header) (including the header/footer)
 */                     
void Dump_Mem() {                        
    int counter;
    char status[5];
    char p_status[5];
    char *t_begin = NULL;
    char *t_end = NULL;
    int t_size;

    blk_hdr *current = first_blk;
    counter = 1;

    int busy_size = 0;
    int free_size = 0;
    int is_busy = -1;

    fprintf(stdout, "************************************Block list***\
                    ********************************\n");
    fprintf(stdout, "No.\tStatus\tPrev\tt_Begin\t\tt_End\t\tt_Size\n");
    fprintf(stdout, "-------------------------------------------------\
                    --------------------------------\n");
  
    while (current->size_status != 1) {
        t_begin = (char*)current;
        t_size = current->size_status;
    
        if (t_size & 1) {
            // LSB = 1 => busy block
            strcpy(status, "Busy");
            is_busy = 1;
            t_size = t_size - 1;
        } else {
            strcpy(status, "Free");
            is_busy = 0;
        }

        if (t_size & 2) {
            strcpy(p_status, "Busy");
            t_size = t_size - 2;
        } else {
            strcpy(p_status, "Free");
        }

        if (is_busy) 
            busy_size += t_size;
        else 
            free_size += t_size;

        t_end = t_begin + t_size - 1;
    
        fprintf(stdout, "%d\t%s\t%s\t0x%08lx\t0x%08lx\t%d\n", counter, status, 
        p_status, (unsigned long int)t_begin, (unsigned long int)t_end, t_size);
    
        current = (blk_hdr*)((char*)current + t_size);
        counter = counter + 1;
    }

    fprintf(stdout, "---------------------------------------------------\
                    ------------------------------\n");
    fprintf(stdout, "***************************************************\
                    ******************************\n");
    fprintf(stdout, "Total busy size = %d\n", busy_size);
    fprintf(stdout, "Total free size = %d\n", free_size);
    fprintf(stdout, "Total size = %d\n", busy_size + free_size);
    fprintf(stdout, "***************************************************\
                    ******************************\n");
    fflush(stdout);

    return;
}
