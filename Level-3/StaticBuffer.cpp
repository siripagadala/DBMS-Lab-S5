#include "StaticBuffer.h"// the declarations for this class can be found at "StaticBuffer.h"
#include <iostream>

using namespace std;

// Initialize static members of the class
unsigned char StaticBuffer::blocks[BUFFER_CAPACITY][BLOCK_SIZE];
struct BufferMetaInfo StaticBuffer::metainfo[BUFFER_CAPACITY];

StaticBuffer::StaticBuffer() {
  // Initialize all blocks as free
  for (int bufferIndex = 0; bufferIndex < BUFFER_CAPACITY; bufferIndex++) {
    metainfo[bufferIndex].free = true;
  }
}

StaticBuffer::~StaticBuffer() {
  // Empty destructor for now
  // In subsequent stages, implement the write-back functionality here
}

int StaticBuffer::getFreeBuffer(int blockNum) {
  if (blockNum < 0 || blockNum > DISK_BLOCKS) {
    return E_OUTOFBOUND;
  }

  int allocatedBuffer = -1;

  // Iterate through all the blocks in the StaticBuffer
  for (int bufferIndex = 0; bufferIndex < BUFFER_CAPACITY; bufferIndex++) {
    if (metainfo[bufferIndex].free) {
      allocatedBuffer = bufferIndex;
      break;
    }
  }

  if (allocatedBuffer == -1) {
    return E_OUTOFBOUND; // No free buffer available
  }

  metainfo[allocatedBuffer].free = false;
  metainfo[allocatedBuffer].blockNum = blockNum;

  return allocatedBuffer;
}

int StaticBuffer::getBufferNum(int blockNum) {
  // Check if blockNum is valid (between zero and DISK_BLOCKS)
  if (blockNum < 0 || blockNum >= DISK_BLOCKS) {
    return E_OUTOFBOUND;
  }

  // Find and return the bufferIndex which corresponds to blockNum (check metainfo)
  for (int bufferIndex = 0; bufferIndex < BUFFER_CAPACITY; bufferIndex++) {
    if (!metainfo[bufferIndex].free && metainfo[bufferIndex].blockNum == blockNum) {
      return bufferIndex;
    }
  }

  // If block is not in the buffer
  return E_BLOCKNOTINBUFFER;
}
