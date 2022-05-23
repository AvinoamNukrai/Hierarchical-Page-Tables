#include "PhysicalMemory.h"
#include <vector>
#include <unordered_map>
#include <cassert>
#include <cstdio>

typedef std::vector<word_t> page_t;

std::vector<page_t> RAM;
std::unordered_map<uint64_t, page_t> swapFile;

void initialize() {
    RAM.resize(NUM_FRAMES, page_t(PAGE_SIZE));
}

void PMread(uint64_t physicalAddress, word_t* value) {
    if (RAM.empty()) {
        initialize();
    }

    assert(physicalAddress < RAM_SIZE);

    uint64_t frameIndex = physicalAddress / PAGE_SIZE;
    uint64_t frameOffset = physicalAddress % PAGE_SIZE;
    *value = RAM[frameIndex][frameOffset];
 }

void PMwrite(uint64_t physicalAddress, word_t value) {
    if (RAM.empty()) {
        initialize();
    }

    assert(physicalAddress < RAM_SIZE);

    uint64_t frameIndex = physicalAddress / PAGE_SIZE;
    uint64_t frameOffset = physicalAddress % PAGE_SIZE;
    RAM[frameIndex][frameOffset] = value;
}

void PMevict(uint64_t frameIndex, uint64_t evictedPageIndex) {
    if (RAM.empty()) {
        initialize();
    }

    assert(frameIndex < NUM_FRAMES);
    assert(evictedPageIndex < NUM_PAGES);
    assert(swapFile.find(evictedPageIndex) == swapFile.end());

    swapFile[evictedPageIndex] = RAM[frameIndex];
}

void PMrestore(uint64_t frameIndex, uint64_t restoredPageIndex) {
    if (RAM.empty()) {
        initialize();
    }

    assert(frameIndex < NUM_FRAMES);

    // page is not in swap file, so this is essentially
    // the first reference to this page. we can just return
    // as it doesn't matter if the page contains garbage
    if (swapFile.find(restoredPageIndex) == swapFile.end()) {
        return;
    }

    RAM[frameIndex] = std::move(swapFile[restoredPageIndex]);
    swapFile.erase(restoredPageIndex);
}


















//void up2(uint64_t base, uint64_t curr_path, uint64_t &curr_weight, uint64_t i, uint64_t &max_path,
//         int &max_weight, uint64_t &valPointer, word_t& maxWeightFrame, word_t value){
//  if (max_weight < curr_weight){
//
//    max_weight = max_weight < curr_weight ? curr_weight:max_weight;
//
//  }
//  max_path = curr_path;
//  maxWeightFrame = value;
//  valPointer = base*PAGE_SIZE + i;
//}
//
//
///**
// *
// * @param depth - depth of path
// * @param base - base address
// * @param curr_path - current path
// * @param emptyFrame - boolean indicating frame is empty
// * @param frameToKeep - a frame in the path that we do not want to evict
// * @param max_path - the path with the maximum weight
// * @param max_weight - the weight of the maximum path
// * @param toEvict - the item we need to remove
// * @param valPointer - a pointer to the item we need to remove
// * @return a frame to be evicted
// */
//int dfs(uint64_t depth, uint64_t base, uint64_t curr_path, uint64_t curr_weight,bool *emptyFrame, uint64_t frameToKeep, uint64_t &max_path,
//        int &max_weight, word_t& toEvict, uint64_t& valPointer, word_t & maxWeightFrame) {
//  toEvict = (toEvict > (int)base) ? toEvict:base;
//  word_t value;
//  int dfsRet; int foundFrame =0;
//  bool valueIsZero = true;
//  if(depth == TABLES_DEPTH) { // recursion base
//    return 0;
//  }
//  for (uint64_t i=0;i<PAGE_SIZE;i++){
//    PMread(i+base*PAGE_SIZE,&value); // read content of frame
//    if(value!=0){ // frame isn't empty
//      uint64_t newPath = (curr_path << OFFSET_WIDTH) +i; // path to the current frame
//      valueIsZero = false; // frame isn't empty boolean indicator
//      if(depth == TABLES_DEPTH -1){ // find path weights here
//        uint64_t absu = base - curr_path < 0  ? -(base - curr_path) : base - curr_path;
//        uint64_t mini = NUM_PAGES - absu < absu ? NUM_PAGES - absu: absu;
//
////        if (max_weight < mini){
////
////          max_weight = max_weight < mini ? mini:max_weight;
////
////        }
////        max_path = curr_path;
////        maxWeightFrame = value;
////        valPointer = base*PAGE_SIZE + i;
////        update_params(base, newPath, (curr_weight), i, max_path, max_weight, valPointer, maxWeightFrame, value);
//        up2(base, newPath, mini, i, max_path, max_weight, valPointer, maxWeightFrame, value);
//      }
//      dfsRet = dfs(depth+1, value, newPath, curr_weight, emptyFrame, frameToKeep,
//                   max_path, max_weight, toEvict, valPointer, maxWeightFrame); // recursive call for depth+1
//
//      if(dfsRet!=0 && !foundFrame){ // dfs has found some non empty frame AND didnt found a frame till now
//        if(*emptyFrame){ // the CHILD frame is currently empty - we can write to it
//          PMwrite(base*PAGE_SIZE+i,0); // writing to the parent 0 (unlink from parent)
//          *emptyFrame = false;
//        }
//        foundFrame = dfsRet; // updating that we have found a frame
//      }
//    }
//  }
//  if(base!= frameToKeep && base != 0 && valueIsZero){ //making sure to not remove a frame from the path
//    *emptyFrame = true;
//    return base;
//  }
//  return foundFrame;
//}