#include "StaticBuffer.h"

// the declarations for this class can be found at "StaticBuffer.h"

unsigned char StaticBuffer::blocks[BUFFER_CAPACITY][BLOCK_SIZE];
struct BufferMetaInfo StaticBuffer::metainfo[BUFFER_CAPACITY];

StaticBuffer::StaticBuffer() {
  for (int bufferIndex = 0; bufferIndex < BUFFER_CAPACITY; bufferIndex++) {
    // Initialize the metainfo for each buffer block
    metainfo[bufferIndex].free = true;
    metainfo[bufferIndex].dirty = false;
    metainfo[bufferIndex].timeStamp = -1;
    metainfo[bufferIndex].blockNum = -1;
  }
}

/*
At this stage, we are not writing back from the buffer to the disk since we are
not modifying the buffer. So, we will define an empty destructor for now. In
subsequent stages, we will implement the write-back functionality here.
*/
StaticBuffer::~StaticBuffer() {
// stage 6
  for(int bufferIndex = 0;bufferIndex < BUFFER_CAPACITY; bufferIndex++){
		if(metainfo[bufferIndex].dirty == true){
			Disk::writeBlock(blocks[bufferIndex],metainfo[bufferIndex].blockNum);
		}
	}
}


int StaticBuffer::getBufferNum(int blockNum) {
  // Check if blockNum is valid (between zero and DISK_BLOCKS)
  // and return E_OUTOFBOUND if not valid.
  if (blockNum < 0 || blockNum > DISK_BLOCKS) {
    return E_OUTOFBOUND;
  }
  // find and return the bufferIndex which corresponds to blockNum (check metainfo)
 for (int i=0; i<BUFFER_CAPACITY-1;i++)
  {
    if ( metainfo[i].blockNum==blockNum )
    {
      return i;
    }
  }

  // if block is not in the buffer
  return E_BLOCKNOTINBUFFER;
}

int StaticBuffer::getFreeBuffer(int blockNum) {
  if (blockNum < 0 || blockNum > DISK_BLOCKS) {
    return E_OUTOFBOUND;
  }
  //stage 6 LRU
  //buffernum used to store the freed buffer 
  int bufferNum = -1;
  //we check if there is a free buffer and return its number if so
	//we also increment the timeStamp of all the buffers that are not free

  for(int bufferIndex = 0;bufferIndex < BUFFER_CAPACITY; bufferIndex++){
		
		if(metainfo[bufferIndex].free == false){
			metainfo[bufferIndex].timeStamp++;
		}

		if(metainfo[bufferIndex].free == true){
			bufferNum = bufferIndex;
			break;
		}
	}

  //if there is no free buffer, we find the least recently used buffer and return its number
	//we also write the block in the buffer to disk if it is dirty

	if(bufferNum == -1){
			int leastRecentTimeStamp = metainfo[0].timeStamp;
			bufferNum = 0;

			for(int bufferIndex = 0;bufferIndex < BUFFER_CAPACITY; bufferIndex++){
				if(metainfo[bufferIndex].timeStamp > leastRecentTimeStamp){
					leastRecentTimeStamp = metainfo[bufferIndex].timeStamp;
					bufferNum = bufferIndex;
				}
			}
	}

  if(metainfo[bufferNum].dirty == true){
		Disk::writeBlock(blocks[bufferNum],metainfo[bufferNum].blockNum);
	}

	metainfo[bufferNum].free = false;
	metainfo[bufferNum].blockNum = blockNum;
	metainfo[bufferNum].timeStamp = 0;
	metainfo[bufferNum].dirty = false;

	return bufferNum;

}

/* Get the buffer index where a particular block is stored
   or E_BLOCKNOTINBUFFER otherwise
*/

int StaticBuffer::setDirtyBit(int blockNum){
    // find the buffer index corresponding to the block using getBufferNum().

    if(blockNum < 0 || blockNum > DISK_BLOCKS){
		return E_OUTOFBOUND;
	}


	int bufferNum = getBufferNum(blockNum);

	if(bufferNum == E_BLOCKNOTINBUFFER){
		return E_BLOCKNOTINBUFFER;
	}

	metainfo[bufferNum].dirty = true;

	return SUCCESS;
}