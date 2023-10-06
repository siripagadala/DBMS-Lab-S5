#include "BlockBuffer.h"

#include <cstdlib>
#include <cstring>
#include <iostream>


using namespace std ;


// the declarations for these functions can be found in "BlockBuffer.h"

BlockBuffer::BlockBuffer(int blockNum) {
  // initialise this.blockNum with the argument
  this->blockNum = blockNum;
}

// calls the parent class constructor
RecBuffer::RecBuffer(int blockNum) : BlockBuffer::BlockBuffer(blockNum) {}

// load the block header into the argument pointer
int BlockBuffer::getHeader(struct HeadInfo *head) {
  unsigned char *bufferPtr;
  
  // read the block at this.blockNum into the buffer
  int ret = loadBlockAndGetBufferPtr(&bufferPtr);
  if(ret != SUCCESS){
    return ret;
  }
  //char message2[BLOCK_SIZE];
  //memcpy(message2, buffer + 20, BLOCK_SIZE);
  //cout << message2 << endl ;

  // populate the numEntries, numAttrs and numSlots fields in *head
  memcpy(&head->numSlots, bufferPtr + 24, 4);
  memcpy(&head->numEntries, bufferPtr + 16, 4);
  memcpy(&head->numAttrs, bufferPtr + 20, 4);
  memcpy(&head->rblock, bufferPtr + 12, 4);
  memcpy(&head->lblock, bufferPtr + 8 , 4);

  return SUCCESS;
}

// load the record at slotNum into the argument pointer
int RecBuffer::getRecord(union Attribute *rec, int slotNum) {
  struct HeadInfo head;
    
  // get the header using this.getHeader() function
  this->getHeader(&head) ;
  
  int attrCount = head.numAttrs;
  int slotCount = head.numSlots;

  // read the block at this.blockNum into a buffer
  unsigned char *buffer;
  int ret = loadBlockAndGetBufferPtr(&buffer);
  if(ret != SUCCESS){
    return ret;
  }

  // record at slotNum will be at offset HEADER_SIZE + slotMapSize + (recordSize * slotNum)
     //- each record will have size attrCount * ATTR_SIZE
     //- slotMap will be of size slotCount
  
  int recordSize = attrCount * ATTR_SIZE;
  // calculate buffer + offset 
  unsigned char *slotPointer =  buffer + recordSize*slotNum + slotCount + HEADER_SIZE;
  //
  
  // load the record into the rec data structure
  memcpy(rec, slotPointer, recordSize);

  return SUCCESS;
}


int BlockBuffer::loadBlockAndGetBufferPtr(unsigned char **buffPtr) {
  // check whether the block is already present in the buffer using StaticBuffer.getBufferNum()
  int bufferNum = StaticBuffer::getBufferNum(this->blockNum);

  if (bufferNum == E_BLOCKNOTINBUFFER) {
    bufferNum = StaticBuffer::getFreeBuffer(this->blockNum);
    Disk::readBlock(StaticBuffer::blocks[bufferNum], this->blockNum);
  }
  else if(bufferNum == E_OUTOFBOUND) {
      return E_OUTOFBOUND;
  }

  else {
    // update the timestamp of the buffer in the meta info
    StaticBuffer::metainfo[bufferNum].timeStamp = 0;

    for(int bufferIndex = 0;bufferIndex < BUFFER_CAPACITY; bufferIndex++){
      if(bufferIndex != bufferNum && StaticBuffer::metainfo[bufferIndex].free == false){
        StaticBuffer::metainfo[bufferIndex].timeStamp++;
      }
    }
  }

  // store the pointer to this buffer (blocks[bufferNum]) in *buffPtr
   *buffPtr = StaticBuffer::blocks[bufferNum];

  return SUCCESS;
}

/* used to get the slotmap from a record block
NOTE: this function expects the caller to allocate memory for `*slotMap`
*/
int RecBuffer::getSlotMap(unsigned char *slotMap) {
  unsigned char *bufferPtr;

  // get the starting address of the buffer containing the block using loadBlockAndGetBufferPtr().
  int ret = loadBlockAndGetBufferPtr(&bufferPtr);
  if (ret != SUCCESS) {
    return ret;
  }

  struct HeadInfo head;
  // get the header of the block using getHeader() function
  this->getHeader(&head);

  int slotCount = head.numSlots;  /* number of slots in block from header */

  // get a pointer to the beginning of the slotmap in memory by offsetting HEADER_SIZE
  unsigned char *slotMapInBuffer = bufferPtr + HEADER_SIZE;

  // copy the values from `slotMapInBuffer` to `slotMap` (size is `slotCount`)
  memcpy(slotMap,slotMapInBuffer,slotCount);

  return SUCCESS;
}

int RecBuffer::setRecord(union Attribute *rec, int slotNum){

  unsigned char *bufferPtr;
  int ret= loadBlockAndGetBufferPtr(&bufferPtr);

  if(ret!=SUCCESS)
  	return ret;

  struct HeadInfo head;
  this->getHeader(&head);
  int attrCount = head.numAttrs;
  int slotCount = head.numSlots;

  if(slotNum < 0 || slotNum >= slotCount)
  	return E_OUTOFBOUND;

  int recordSize = attrCount * ATTR_SIZE;
  unsigned char *slotPointer = bufferPtr + HEADER_SIZE + slotCount + recordSize * slotNum;

  memcpy(slotPointer, rec, recordSize);

  StaticBuffer::setDirtyBit(this->blockNum);

  return SUCCESS;

}
