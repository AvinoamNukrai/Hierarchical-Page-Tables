#pragma once

#include <climits>
#include <stdint.h>

typedef int word_t;

#define WORD_WIDTH (sizeof(word_t) * CHAR_BIT)

// number of bits in the offset
#define OFFSET_WIDTH 4
// page/frame size in words
// in this implementation this is also the number of entries in a table
#define PAGE_SIZE (1LL << OFFSET_WIDTH)

// number of bits in a physical address
#define PHYSICAL_ADDRESS_WIDTH 10
// RAM size in words
#define RAM_SIZE (1LL << PHYSICAL_ADDRESS_WIDTH)

// number of bits in a virtual address
#define VIRTUAL_ADDRESS_WIDTH 20
// virtual memory size in words
#define VIRTUAL_MEMORY_SIZE (1LL << VIRTUAL_ADDRESS_WIDTH)

// number of frames in the RAM
#define NUM_FRAMES (RAM_SIZE / PAGE_SIZE)

// number of pages in the virtual memory
#define NUM_PAGES (VIRTUAL_MEMORY_SIZE / PAGE_SIZE)

#define CEIL(VARIABLE) ( (VARIABLE - (int)VARIABLE)==0 ? (int)VARIABLE : (int)VARIABLE+1 )
#define TABLES_DEPTH CEIL((((VIRTUAL_ADDRESS_WIDTH - OFFSET_WIDTH) / (double)OFFSET_WIDTH)))






// TODO: TO DELETE!!!!

#define WEIGHT_EVEN 4

#define WEIGHT_ODD 2