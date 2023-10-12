#include "StaticBuffer.h"
#define BMAPSIZE 4
unsigned char StaticBuffer::blocks[BUFFER_CAPACITY][BLOCK_SIZE];
struct BufferMetaInfo StaticBuffer::metainfo[BUFFER_CAPACITY];
unsigned char StaticBuffer::blockAllocMap[DISK_BLOCKS];

StaticBuffer::StaticBuffer(){

	//initializing the block allocation map
	//reading from the first 4 blocks of the disk

	unsigned char block[BLOCK_SIZE];
	for(int i = 0; i < BMAPSIZE; i++){
		Disk::readBlock(block,i);
		for(int j = 0; j < BLOCK_SIZE; j++){
			blockAllocMap[(i * BLOCK_SIZE)+ j] = block[j];
		}
	}

	for(int bufferIndex = 0;bufferIndex < BUFFER_CAPACITY; bufferIndex++){
		
		metainfo[bufferIndex].free = true;
		metainfo[bufferIndex].dirty = false;
		metainfo[bufferIndex].blockNum = -1;
		metainfo[bufferIndex].timeStamp = -1;

	}
}


StaticBuffer::~StaticBuffer(){

	unsigned char block[BLOCK_SIZE];
	for(int i= 0; i < BMAPSIZE; i++){
		for(int j = 0; j < BLOCK_SIZE; j++){
			block[j] = blockAllocMap[i * BLOCK_SIZE + j];
		}
		Disk::writeBlock(block,i);
	}

	for(int bufferIndex = 0;bufferIndex < BUFFER_CAPACITY; bufferIndex++){
		if(metainfo[bufferIndex].dirty == true){
			Disk::writeBlock(blocks[bufferIndex],metainfo[bufferIndex].blockNum);
		}
	}
}  
  
int StaticBuffer::getBufferNum(int blockNum){
  	if(blockNum < 0 || blockNum > DISK_BLOCKS){
		return E_OUTOFBOUND;
	}
	for(int i=0;i<BUFFER_CAPACITY-1;i++){
		if(metainfo[i].blockNum==blockNum){
			return i;
			}
	}
	return E_BLOCKNOTINBUFFER;
}
  
int StaticBuffer::getFreeBuffer(int blockNum){

	//check if blockNum is within the range of disk blocks
	//return E_OUTOFBOUND if not


	if(blockNum < 0 || blockNum > DISK_BLOCKS){
		return E_OUTOFBOUND;
	} 	

	int bufferNum = -1;

	//we check if there is a free buffer and return its number if so
	//we also increment the timeStamp of all the buffers that are not free

	for(int bufferIndex = 0;bufferIndex < BUFFER_CAPACITY; bufferIndex++){
		
		if(metainfo[bufferIndex].free == false){
			metainfo[bufferIndex].timeStamp++;
		}

		if(metainfo[bufferIndex].free == true){
			bufferNum = bufferIndex;
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

int StaticBuffer::setDirtyBit(int blockNum){
	
	//check if blockNum is within the range of disk blocks
	//return E_OUTOFBOUND if not

	if(blockNum < 0 || blockNum > DISK_BLOCKS){
		return E_OUTOFBOUND;
	}

	//we find the buffer number corresponding to the blockNum
	//we set the dirty bit of the buffer to true

	int bufferNum = getBufferNum(blockNum);

	if(bufferNum == E_BLOCKNOTINBUFFER){
		return E_BLOCKNOTINBUFFER;
	}

	metainfo[bufferNum].dirty = true;

	return SUCCESS;
}
