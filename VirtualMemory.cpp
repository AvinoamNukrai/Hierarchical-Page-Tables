// Written Be Avinoam Nukrai, Hebrew U 2022 OS Course

#include "VirtualMemory.h"
#include "PhysicalMemory.h"



/**
 * Initialize virtual memory.
 * Meaning - fill the first table of the virtual memory with zero's
 */
void VMinitialize(){
  for (int offset = 0; offset < PAGE_SIZE; ++offset) {
    PMwrite(0 + offset, 0);
  }
}

/**
 * This func returns the actual place in memory for some given frame number
 * and frame offset
 * @param frameNumber
 * @param frameOffset
 * @return physical address of the given frame number
 */
uint64_t getFinalMemoryAddress(uint64_t frameNumber, uint64_t frameOffset){
  return (frameNumber * PAGE_SIZE) + frameOffset;
}


/**
 * This function returns the offset of a frame that related to the given virtualAddress (someVA)
 * @param level - the level in the tree
 * @param someVA - some virtual address to work from
 * @return the wanted offset
 */
uint64_t getFrameOffset(int level, uint64_t someVA){
  return (someVA >> (TABLES_DEPTH - level) * OFFSET_WIDTH) % PAGE_SIZE;
}

/**
 * This func init some given frame, meaning - fill it with zero's
 * @param frame - some frame to init
 */
void initMemoryChunk(uint64_t frame) {
  for (int i = 0; i < PAGE_SIZE; ++i) {
    uint64_t address = getFinalMemoryAddress(frame, i);
    PMwrite(address, 0);
  }
}



/**
 * This func just update the value of the cyc dis parameter according to the
 * new cyc dis, also updates the path to the right frame.
 * @param baseAddress
 * @param currPath
 * @param currWeight
 * @param offset
 * @param maxPath
 * @param maxWeight
 * @param frameToEvict
 * @param maxWeightFrame
 * @param value
 */
void updateCycArgs(uint64_t baseAddress, uint64_t currPath,
                   uint64_t &currWeight, uint64_t offset,
                   uint64_t &maxPath, int &maxWeight,
                   uint64_t &frameToEvict, word_t& maxWeightFrame, word_t value){
  maxWeight = maxWeight < int(currWeight) ? int(currWeight): maxWeight;
  maxPath = currPath;
  maxWeightFrame = value;
  frameToEvict = getFinalMemoryAddress(baseAddress, offset);
}

/**
 * This function calc the cyclical distance between the current page we wish to
 * push to the RAM, to other page p (we got his path from the dfs running)
 * @param pageToSwapIn
 * @param currPath
 * @return returns the cyc dis according to the formula from the PFD file
 */
uint64_t calcCyclicalDistance(uint64_t pageToSwapIn, uint64_t currPath){
  uint64_t abs = pageToSwapIn < currPath ?
      currPath - pageToSwapIn : pageToSwapIn - currPath;
  uint64_t min = NUM_PAGES - abs < abs ? NUM_PAGES - abs : abs;
  return min;
}


/**
 * This func runs on the tree in recursive way - searching for some free frame
 * and calculate the wat to the evicted one (if we will need in the future)
 * @param level
 * @param baseAddress
 * @param currPath
 * @param ifEmptyFrame
 * @param forbiddenFrame
 * @param maxPath
 * @param max_weight
 * @param frameToEvict
 * @param valPointer
 * @param maxWeightFrame
 * @return
 */
word_t dfs(uint64_t level, uint64_t baseAddress, uint64_t currPath,
           bool *ifEmptyFrame, uint64_t forbiddenFrame,
           uint64_t &maxPath, int &maxWeight, word_t& frameToEvict,
           uint64_t& valPointer, word_t& maxWeightFrame) {
  word_t nextFrame;
  if (frameToEvict <= (int)baseAddress){
    frameToEvict = baseAddress;
  }
  bool ifFrameIsZero = true;
  int retVal;
  int foundFrame =0;
  if(level == TABLES_DEPTH) return 0; // base case - we got into a leaf
  for (uint64_t offset = 0; offset < PAGE_SIZE; offset++){
    PMread(getFinalMemoryAddress(baseAddress, offset), &nextFrame);
    if(nextFrame != 0){ // meaning that the frame is not empty! there is some pointer (non-zero value) inside him
      uint64_t newPath = (currPath << OFFSET_WIDTH) + offset;  // the path to the current frame (for evicting)
      ifFrameIsZero = false; // frame isn't empty boolean indicator
      if (level == TABLES_DEPTH - 1){ // find path weights here
        uint64_t curCycDis = calcCyclicalDistance(baseAddress, currPath);
        updateCycArgs(baseAddress, newPath, curCycDis, offset,
                      maxPath, maxWeight, valPointer,
                      maxWeightFrame, nextFrame);
      }
      retVal = dfs(level+1, nextFrame, newPath, ifEmptyFrame,
                   forbiddenFrame, maxPath, maxWeight,
                   frameToEvict, valPointer, maxWeightFrame);

      if (!foundFrame && retVal != 0){ // meaning that we did not found a frame YET and we got non-empty frame
        if (*ifEmptyFrame){
          PMwrite(getFinalMemoryAddress(baseAddress, offset), 0); // update the parent to contains zero
          *ifEmptyFrame = false;
        }
        foundFrame = retVal; // indicates that we've found an empty frame with the address baseAddress
      }
    }
  }
  // validate that we've not got the root of the tree and we're not takes some allocated frame
  if (baseAddress != 0 && (baseAddress != forbiddenFrame) && ifFrameIsZero) {
    *ifEmptyFrame = true;
    return baseAddress;
  }
  return foundFrame;
}


/**
 * This function responsible for finding an empty frame (with the dfs algorithm)
 * and also calc the path of the frame we would want to evict in case there is
 * not an empty one
 * @param ifSucceeded - boolean that indicates if the current func succeeded
 * @param forbiddenFrame - the frame we dont want to allow for any allocation (since he in use already)
 * @param maxPath - the max path to the right frame (for evicting)
 * @param maxWeight - the maximum value of the maxPath
 * @param evict_init - running index for check if + 1 < NUM_FRAMES
 * @param valPointer - the pointer in which the frame we would want to evict
 * @return return the frame we found in the dfs running (if)
 */
uint64_t findEmptyFrame(bool* ifSucceeded ,uint64_t& forbiddenFrame,
                        uint64_t& maxPath, int maxWeight, word_t& evict_init,
                        uint64_t& valPointer, word_t& maxWeightFrame){
  bool theFrameIsEmpty = false;
  uint64_t frame =  dfs(0,0,0,&theFrameIsEmpty,
                        forbiddenFrame, maxPath, maxWeight,
                        evict_init, valPointer, maxWeightFrame);
  if (frame) return frame;
  if (evict_init + 1 < NUM_FRAMES){
    frame = evict_init + 1;
    *ifSucceeded = true; // we found an empty frame, no need to evict any
  }
  return frame;
}



/**
 * This func evict some given frame with some given path + fill the right
 * frame with zero's
 * @param maxFrameForEvict - address of frame we want to avict
 * @param maxPath - pagIndex to evict to
 * @param frameToWriteTo - the actual place in memory in which we will init
 */
void eivctFrame(word_t& maxFrameForEvict, uint64_t& maxPath,
                uint64_t& frameToWriteTo){
  PMevict(maxFrameForEvict, maxPath);
  PMwrite(frameToWriteTo,0);
}


/**
 * This function search for valid frame, either by search for an empty one
 * either for evict one
 * @param forbiddenFrame - the frame that we can't get from the dfs = the
 * one that already was visited
 * @return returns the address of the new frame we found
 */
uint64_t getEmptyOrEvictedFrame(uint64_t forbiddenFrame){
  bool ifSucceeded = false;
  uint64_t maxPath = 0; int maxWeight = 0; word_t evict_init = 0;
  uint64_t frameToWriteTo = 0; word_t maxFrameForEvict = 0;
  uint64_t frame = findEmptyFrame(&ifSucceeded, forbiddenFrame,
                                  maxPath, maxWeight, evict_init,
                                  frameToWriteTo, maxFrameForEvict);
  if (frame != 0) return frame;
  // check if we found empty frame, if not - we must evict
  if (!ifSucceeded){
    eivctFrame(maxFrameForEvict, maxPath, frameToWriteTo);
    frame = maxFrameForEvict;
  }
  // found a frame containing an empty table:
  return frame;
}


/**
 * This function search for some valid frame in the RAM
 * @param level - the current level in the tree
 * @param someVA - the virtual address to work from
 * @return the valid frame we've found, either an empty one or an evicted one
 */
uint64_t findValidFrame(int level, uint64_t someVA, uint64_t forbiddenFrame){
  // need to find some empty frame, if not such one - need to find someone for evicting
  uint64_t validFrame = getEmptyOrEvictedFrame(forbiddenFrame);
  if (level < TABLES_DEPTH - 1) initMemoryChunk(validFrame);
    // else we got to a leaf of the tree
  else PMrestore(validFrame, someVA >> OFFSET_WIDTH);
  return validFrame;
}

/**
 * This function returns the physical address of the given virtual address, doing this by search the right frame and
 * such one that is valid
 * @param someVirtualAddress - virtual address to translate for
 * @return the physical address of the given virtual address
 */
uint64_t translateToPhysicalAddress(uint64_t someVirtualAddress){
  // first, iterating the depth of the tree
  uint64_t forbiddenFrame = 0; // the frame of which we use in, we dont want to allow to use it since we init this
  uint64_t finalOffset = someVirtualAddress % PAGE_SIZE;
  uint64_t frameNumber = 0;
  word_t nextFrame = 0; // the number of frame that stored in our calc physical address
  for (int level = 0; level < TABLES_DEPTH; ++level) {
    uint64_t frameOffset = getFrameOffset(level, someVirtualAddress);
    PMread(frameNumber + frameOffset, &nextFrame);
    // if the nextFrame == 0, we want to find some empty frame or to evict one (cause he is non-valid)
    if (nextFrame == 0){
      nextFrame = findValidFrame(level, someVirtualAddress, forbiddenFrame);
      PMwrite(frameNumber + frameOffset, (word_t)nextFrame);
    }
    forbiddenFrame = nextFrame;
    frameNumber = nextFrame * PAGE_SIZE;
  }
  uint64_t finalAddress = getFinalMemoryAddress(nextFrame, finalOffset);
  return finalAddress;
}


/** Reads a word from the given virtual address
 * and puts its content in *value.
 *
 * returns 1 on success.
 * returns 0 on failure (if the address cannot be mapped to a physical
 * address for any reason)
 */
int VMread(uint64_t virtualAddress, word_t* value){
  // input validation
  if (virtualAddress >= VIRTUAL_MEMORY_SIZE){
    return 0;
  }
  uint64_t physicalAddress = translateToPhysicalAddress(virtualAddress);
  PMread(physicalAddress, value);
  return 1;
}


/** Writes a word to the given virtual address.
 *
 * returns 1 on success.
 * returns 0 on failure (if the address cannot be mapped to a physical
 * address for any reason)
 */
int VMwrite(uint64_t virtualAddress, word_t value){
  // input validation
  if (virtualAddress >= VIRTUAL_MEMORY_SIZE){
    return 0;
  }
  uint64_t physicalAddress = translateToPhysicalAddress(virtualAddress);
  PMwrite(physicalAddress, value);
  return 1;
}