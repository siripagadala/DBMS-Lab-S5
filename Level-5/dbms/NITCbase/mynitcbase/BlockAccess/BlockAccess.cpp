#include "BlockAccess.h"

#include <cstring>
#include <iostream>
using namespace std;



// RecId BlockAccess::linearSearch(int relId, char attrName[ATTR_SIZE], union Attribute attrVal, int op) {
//     // Get the previous search index of the relation relId from the relation cache
//     //RecId *prevRecId = (RecId*)malloc(sizeof(RecId));
//     RecId prevRecId;
//     RelCacheTable::getSearchIndex(relId, &prevRecId);
//     //printf("hello nya1\n");                             //working
//     // if (ret != SUCCESS) {
//     //     return RecId{-1, -1};
//     // }

//     int block;
//     int slot;

//     // If the current search index record is invalid (i.e., both block and slot = -1)
//     if (prevRecId.block == -1 && prevRecId.slot == -1) {
//         struct RelCatEntry relcatbu;
//         //printf("hello nya2 enterinf if.....\n");
//         // (no hits from previous search; search should start from the first record itself)

//         // Get the first record block of the relation from the relation cache
//         // You need to have a way to retrieve the root block of the relation here.
//         // For example, assuming you have a function getRootBlock(relId) to get the root block:
//         RelCacheTable::getRelCatEntry(relId,&relcatbu);
//         //printf("hello nya3\n");
//         // if (ret != SUCCESS) {
//         //     return RecId{-1, -1};
//         // }
//         block = relcatbu.firstBlk;
//         slot = 0;
//     } else {
//         // (there is a hit from the previous search; search should start from the record next to the search index record)
//         block = prevRecId.block;
//         slot = prevRecId.slot + 1;
//     }

//     /* The following code searches for the next record in the relation
//        that satisfies the given condition
//        We start from the record id (block, slot) and iterate over the remaining
//        records of the relation
//     */
//     while (block != -1) {
//         //printf("Block: %d, Slot: %d\n", prevRecId.block, prevRecId.slot); //working till here
//         // Create a RecBuffer object for block
//         RecBuffer recBuffer(block); //block instead of prevrecId->block

//         Attribute recAttrs[ATTRCAT_NO_ATTRS];
//         HeadInfo head;

//         // Get the record with id (block, slot) using RecBuffer::getRecord()
//         recBuffer.getRecord(recAttrs, slot);  //slot instead of prevrecId->slot
//         // if (ret != SUCCESS) {
//         //     return RecId{-1, -1};
//         // }

//         // Get header of the block using RecBuffer::getHeader() function
//         recBuffer.getHeader(&head);
//         // if (ret != SUCCESS) {
//         //     return RecId{-1, -1};
//         // }

//         // Get slot map of the block using RecBuffer::getSlotMap() function
//         unsigned char slotMap[head.numSlots];
//         recBuffer.getSlotMap(slotMap);
//         // if (ret != SUCCESS) {
//         //     return RecId{-1, -1};
//         // }

//         if (slot >= head.numSlots) {
//             // Update block = right block of block
//             // Update slot = 0
//             // Continue to the beginning of this while loop
//             block = head.rblock;
//             slot = 0;
//             continue;
//         }

//         if (slotMap[slot] == SLOT_UNOCCUPIED) {
//             // Increment slot and continue to the next record slot
//             slot++;
//             continue;
//         }

//         // Get the attribute offset for the attrName attribute
//         struct AttrCatEntry attrCatEntry;
//         AttrCacheTable::getAttrCatEntry(relId, attrName, &attrCatEntry);
//         // if (ret != SUCCESS) {
//         //     return RecId{-1, -1};
//         // }

//         // Use the attribute offset to get the value of the attribute from the current record
//         Attribute recordAttr = recAttrs[attrCatEntry.offset];

//         // Set cmpVal using compareAttrs()
//         int cmpVal = compareAttrs(recordAttr, attrVal, attrCatEntry.attrType);

//         // Check whether this record satisfies the given condition
        

//         if (
//             (op == NE && cmpVal != 0) ||    // if op is "not equal to"
//             (op == LT && cmpVal < 0) ||     // if op is "less than"
//             (op == LE && cmpVal <= 0) ||    // if op is "less than or equal to"
//             (op == EQ && cmpVal == 0) ||    // if op is "equal to"
//             (op == GT && cmpVal > 0) ||     // if op is "greater than"
//             (op == GE && cmpVal >= 0)       // if op is "greater than or equal to"
//         ) {
//             //printf("Record found: Block %d, Slot %d\n", prevRecId->block, prevRecId->slot);

//             RecId si;
//             si.block = block;
//             si.slot = slot;

//             RelCacheTable::setSearchIndex(relId, &si);
//             // if (ret != SUCCESS) {
//             //     return RecId{-1, -1};
//             // }

//             return si;

            
//         }

//         slot++;
//     }

//     // No record in the relation with Id relid satisfies the given condition
//     return RecId{-1, -1};
// }


int compareAttrs(union Attribute attr1, union Attribute attr2, int attrType) {
    double diff;

    if (attrType == STRING) {
        diff = strcmp(attr1.sVal, attr2.sVal);
    } else {
        diff = attr1.nVal - attr2.nVal;
    }

    if (diff > 0) {
        return 1;
    } else if (diff < 0) {
        return -1;
    } else {
        return 0;
    }
}
RecId BlockAccess::linearSearch(int relId, char attrName[ATTR_SIZE], union Attribute attrVal, int op) {
    RecId prevRecId;
    RelCacheTable::getSearchIndex(relId, &prevRecId);		
    int block,slot;
    if (prevRecId.block == -1 && prevRecId.slot == -1)
    {
        RelCatEntry relCatBuf;
        //CHECK THIS PART DOUBT
        RelCacheTable::getRelCatEntry(relId, &relCatBuf);
        //DOUBT HERE AGAIN
        block = relCatBuf.firstBlk;
        slot = 0;
    }
    else
    {
	    block = prevRecId.block;
	    slot = prevRecId.slot+1;
    }
    
    RelCatEntry relCatBuff;
    RelCacheTable::getRelCatEntry(relId, &relCatBuff);
    while (block != -1)
    {
        /* create a RecBuffer object for block (use RecBuffer Constructor for
           existing block) */
      
        //CHECK DOUBT
    RecBuffer recBuff(block);
	HeadInfo head;
	
	recBuff.getHeader(&head);
	unsigned char* slotMap= (unsigned char*) malloc (head.numSlots*sizeof(unsigned char));
	recBuff.getSlotMap(slotMap);
        if(slot>= relCatBuff.numSlotsPerBlk)
        {
            block = head.rblock;
            slot=0;
            continue; 
        }
        if(slotMap[slot]==SLOT_UNOCCUPIED)
        {
            slot++;
            continue;
        }
	
	Attribute record[head.numAttrs];
	recBuff.getRecord(record, slot);
	AttrCatEntry attrCatBuf;
	AttrCacheTable::getAttrCatEntry(relId,attrName,&attrCatBuf);
	int offset= attrCatBuf.offset;
	Attribute attrAtOffset= record[offset];

        int cmpVal;  // will store the difference between the attributes
        // set cmpVal using compareAttrs()
        //printf("attrVal %0.2f attrvaaal %s\n", attrVal.nVal, attrVal.sVal);
        //printf("attrType %d\n", attrType);
	//cmpVal= compareAttrs(attrAtOffset,attrVal, attrType);
    cmpVal= compareAttrs(attrAtOffset,attrVal,attrCatBuf.attrType);
    //printf("cmpVal %d\n", cmpVal);
        if (
            (op == NE && cmpVal != 0) ||    // if op is "not equal to"
            (op == LT && cmpVal < 0) ||     // if op is "less than"
            (op == LE && cmpVal <= 0) ||    // if op is "less than or equal to"
            (op == EQ && cmpVal == 0) ||    // if op is "equal to"
            (op == GT && cmpVal > 0) ||     // if op is "greater than"
            (op == GE && cmpVal >= 0)       // if op is "greater than or equal to"
        ) {
            RecId currentRec;
            currentRec.block=block;
            currentRec.slot=slot;
            RelCacheTable::setSearchIndex(relId, &currentRec);  

            return RecId{block, slot};
        }

        slot++;
    }

    // no record in the relation with Id relid satisfies the given condition
    return RecId{-1, -1};
}