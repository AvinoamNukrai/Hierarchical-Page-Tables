# Hierarchical-Page-Tables
Virtual Memory management, Implement of Hierarchical-Page-Tables paradigm

Virtual memory is a memory-management model that allows processes to use more memory than is actually available on the host.
This is achieved by mapping virtual (a.k.a. logical) addresses (what the process sees) into physical addresses (actual locations in the RAM). Since the physical address space is significantly smaller than the virtual address space, not all memory space of all processes (or even of one process!) can fit in the RAM at the same time. Therefore, parts of the virtual memory must be stored in the hard drive until the next time they are used by their process.
Paging is an approach where the virtual address space is partitioned into fixed-sized contiguous blocks, called pages. The physical address space is similarly partitioned into blocks the same size as pages, called frames. At any given time, each page is either mapped to a frame in the physical memory or is stored in the hard drive. When a process tries to access a virtual memory address in a page that is not in the physical memory, that page must be brought into the physical memory (swapped in). If there are no unused frames, another page must be evicted from the physical memory (swapped out).
The mapping between pages and frames is done using page tables.
The naïve implementation will have a big table where the number in the p row is the index of the frame to which the p page is mapped. 
Consider the next diagram of the naïve implementation: 

![Screenshot 2022-05-23 174955](https://user-images.githubusercontent.com/64755588/169846535-2b99aaa9-3fd6-43e0-9122-e803234d05a5.png)



In this implementation each address is split into two parts: the offset and the page number. The offset (d in the above figure) is the position within the page and hence doesn’t need any translation. Its width in bits is log2(page_size). The page number is the rest of the address, and its width is the remaining bits, or more precisely: log2(virtual_memory_size) - log2(page_size).In this implementation each address is split into two parts: the offset and the page number. The offset (d in the above figure) is the position within the page and hence doesn’t need any translation. Its width in bits is log2(page_size). The page number is the rest of the address, and its width is the remaining bits, or more precisely: log2(virtual_memory_size) - log2(page_size).
The problem with a single page table is that it can be very wasteful in terms of physical memory consumption, usually there are many unused pages in the middle of the virtual address space which needlessly bloat the table, even to the point that it may not fit in the physical memory at all.
Hierarchical page tables use a tree of smaller tables to save memory. This effectively separates the translation into multiple steps. These tables reside in frames of the RAM just like any other page. Each row of the table points to a frame that contains either the next layer (the next table), or the relevant page itself (if it’s the last, i.e., the lowest, layer).
Each table can only contain a small number of rows, so there are only so many bits it can translate on its own, log2(number_of_rows) bits to be precise. In total the page tables are supposed to translate log2(virtual_memory_size) - log2(page_size) bits, so the depth of the page tables tree must be: ceil((log2(virtual_memory_size) - log2(page_size)) / log2(number_of_rows)). Below is an example of address translation using a tree with 2 layers of tables.

![somi](https://user-images.githubusercontent.com/64755588/169846845-d56fe7bf-9e2f-4d0d-b794-05bf6c8c1c32.png)


In this project I've Implemented the Hierarchical-Page-Tables paradigm, the Implementation include traversing the 
tables tree in a recursive way for search each time the next availeble frame to work with. 
The code was tested well, enjoy :)



