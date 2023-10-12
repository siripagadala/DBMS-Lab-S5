#include "BlockAccess.h"

#include <cstring>
#include <iostream>
using namespace std;

int compareAttrs(union Attribute attr1, union Attribute attr2, int attrType) {
    double diff;

    if (attrType == STRING) {
        diff = strcmp(attr1.sVal, attr2.sVal);
    } else {
        diff = attr1.nVal - attr2.nVal;
    }

    if (diff > 0)
        return 1;
     else 
    if (diff < 0) 
        return -1;
     
    else 
        return 0;
    
}

RecId BlockAccess::linearSearch(int relId, char attrName[ATTR_SIZE], union Attribute attrVal, int op) {
    RecId prevRecId;
    RelCacheTable::getSearchIndex(relId, &prevRecId);		
    int block,slot;
    if (prevRecId.block == -1 && prevRecId.slot == -1)
    {
        RelCatEntry relCatBuf;
        RelCacheTable::getRelCatEntry(relId, &relCatBuf);
        block = relCatBuf.firstBlk;
        slot = 0;
    }
    else
    {
	    block = prevRecId.block;
	    slot = prevRecId.slot+1;
    }
    
    //RelCatEntry relCatBuff;
    //RelCacheTable::getRelCatEntry(relId, &relCatBuff);
    while (block != -1)
    {
        /* create a RecBuffer object for block (use RecBuffer Constructor for
           existing block) */
        RecBuffer currBuffer(block);
        HeadInfo currHeader;
        currBuffer.getHeader(&currHeader);
        
        Attribute currRecord[currHeader.numAttrs];
        currBuffer.getRecord(currRecord, slot);

        unsigned char slotMap[currHeader.numSlots];
        currBuffer.getSlotMap(slotMap);

        if(slot>= currHeader.numSlots)
        {
            block = currHeader.rblock;
            slot=0;
            continue; 
        }
        if(slotMap[slot]=='0')
        {
            slot++;
            continue;
        }

        //printf("nya1\n");
        AttrCatEntry* attrCatBuf = (AttrCatEntry*)malloc(sizeof(AttrCatEntry));
        //printf("nya2\n");
        AttrCacheTable::getAttrCatEntry(relId,attrName,attrCatBuf);
        //printf("nya3\n");
        //printf("block: %d, slot: %d\n", block, slot);

        int cmpVal= compareAttrs(currRecord[attrCatBuf->offset],attrVal,attrCatBuf->attrType);
        //printf("nya7\n");
    //printf("cmpVal %d\n", cmpVal);
        if (
            (op == NE && cmpVal != 0) ||    // if op is "not equal to"
            (op == LT && cmpVal < 0) ||     // if op is "less than"
            (op == LE && cmpVal <= 0) ||    // if op is "less than or equal to"
            (op == EQ && cmpVal == 0) ||    // if op is "equal to"
            (op == GT && cmpVal > 0) ||     // if op is "greater than"
            (op == GE && cmpVal >= 0)       // if op is "greater than or equal to"
        ) {
            RecId ansRecId;
            ansRecId.block=block;
            ansRecId.slot=slot;
            RelCacheTable::setSearchIndex(relId, &ansRecId);  

            return RecId{block, slot};
        }

        slot++;
    }

    // no record in the relation with Id relid satisfies the given condition
    return RecId{-1, -1};
}

int BlockAccess::renameRelation (char oldName[ATTR_SIZE], char newName[ATTR_SIZE]){

    RelCacheTable::resetSearchIndex(RELCAT_RELID);

    Attribute newRelationName;
    strcpy(newRelationName.sVal, newName);
    char relationName[10];
    strcpy(relationName, "RelName");

    RecId searchNewRel = BlockAccess::linearSearch(RELCAT_RELID, relationName, newRelationName, EQ);
    if(searchNewRel.block != -1 && searchNewRel.slot != -1)
        return E_RELEXIST;

    RelCacheTable::resetSearchIndex(RELCAT_RELID);

    Attribute oldRelationName;
    strcpy(oldRelationName.sVal, oldName);

    RecId searchOldRel = BlockAccess::linearSearch(RELCAT_RELID, relationName, oldRelationName, EQ);

    if(searchOldRel.block == -1 && searchOldRel.slot == -1)
        return E_RELNOTEXIST;

    Attribute modifiedRecord[RELCAT_NO_ATTRS];
    RecBuffer recBuff(searchOldRel.block);
    recBuff.getRecord(modifiedRecord, searchOldRel.slot);

    strcpy(modifiedRecord[RELCAT_REL_NAME_INDEX].sVal, newName);

    recBuff.setRecord(modifiedRecord, searchOldRel.slot);
    
    // TO-DO Update all attribute catalog entries with newname

    RecId attributeCatRecord;
    Attribute modifiedAttributeRecord[ATTRCAT_NO_ATTRS];

    RelCacheTable::resetSearchIndex(ATTRCAT_RELID);

    while(true){

        attributeCatRecord = BlockAccess::linearSearch(ATTRCAT_RELID, relationName, oldRelationName, EQ);
        
        if(attributeCatRecord.block == -1 && attributeCatRecord.slot == -1)
            break;
        
        RecBuffer recBuff(attributeCatRecord.block);
        recBuff.getRecord(modifiedAttributeRecord, attributeCatRecord.slot);
        strcpy(modifiedAttributeRecord[ATTRCAT_REL_NAME_INDEX].sVal, newName);
        recBuff.setRecord(modifiedAttributeRecord, attributeCatRecord.slot);

    }

    return SUCCESS;

}

int BlockAccess::renameAttribute(char relName[ATTR_SIZE], char oldName[ATTR_SIZE], char newName[ATTR_SIZE]) {

    //We check if the relation with name relName exists and return E_RELNOTEXIST if not.
    //We check if the attribute with name oldName exists in the relation with name relName and return E_ATTRNOTEXIST if not.
    //We check if the attribute with name newName exists in the relation with name relName and return E_ATTREXIST if so.
    //We then change the attribute name in the attribute catalog entry for the attribute with name oldName to newName.

    RelCacheTable::resetSearchIndex(RELCAT_RELID);

    Attribute relNameAttr;
    strcpy(relNameAttr.sVal, relName);

    char relationName[10];
    strcpy(relationName, "RelName");

    RecId searchRel = BlockAccess::linearSearch(RELCAT_RELID, relationName, relNameAttr, EQ);

    if(searchRel.block == -1 && searchRel.slot == -1)
        return E_RELNOTEXIST;

    RelCacheTable::resetSearchIndex(ATTRCAT_RELID);

    RecId attrToRenameRecId{-1, -1};
    Attribute attrCatEntryRecord[ATTRCAT_NO_ATTRS];

    //This loop will find the attribute catalog entry for the attribute with name oldName and store its record id in attrToRenameRecId.

    while(true){

        RecId recId = BlockAccess::linearSearch(ATTRCAT_RELID, relationName, relNameAttr, EQ);
        
        if(recId.block == -1 && recId.slot == -1)
            break;
        
        RecBuffer recBuff(recId.block);
        recBuff.getRecord(attrCatEntryRecord, recId.slot);

        if(strcmp(attrCatEntryRecord[ATTRCAT_ATTR_NAME_INDEX].sVal, oldName) == 0)
            attrToRenameRecId = {recId.block, recId.slot};

        if(strcmp(attrCatEntryRecord[ATTRCAT_ATTR_NAME_INDEX].sVal, newName) == 0)
            return E_ATTREXIST;
    }

    if(attrToRenameRecId.block == -1 && attrToRenameRecId.slot == -1)
        return E_ATTRNOTEXIST;

    RecBuffer recBuff(attrToRenameRecId.block);
    recBuff.getRecord(attrCatEntryRecord, attrToRenameRecId.slot);
    strcpy(attrCatEntryRecord[ATTRCAT_ATTR_NAME_INDEX].sVal, newName);
    recBuff.setRecord(attrCatEntryRecord, attrToRenameRecId.slot);

    return SUCCESS;
}


int BlockAccess::insert(int relId, Attribute *record){

  RelCatEntry relCatEntry;
  RelCacheTable::getRelCatEntry(relId, &relCatEntry);

  int blockNum = relCatEntry.firstBlk;
  RecId rec_id = {-1, -1};

  int numAttrs = relCatEntry.numAttrs;
  int numSlots = relCatEntry.numSlotsPerBlk;

  int prevBlockNum = -1;
  int freeSlot = -1;
  int freeBlock = -1;

  while(blockNum != -1){

    RecBuffer recBuff(blockNum);
    HeadInfo head;
    recBuff.getHeader(&head);
    unsigned char* slotMap = (unsigned char*) malloc (head.numSlots*sizeof(unsigned char));
    recBuff.getSlotMap(slotMap);

    for(int slotIndex=0; slotIndex < numSlots; slotIndex++){
      if(slotMap[slotIndex] == SLOT_UNOCCUPIED){
        rec_id = {blockNum, slotIndex};
        break;
      }
    }
    if(rec_id.block != -1 && rec_id.slot != -1)
        break;

    prevBlockNum = blockNum;
    blockNum = head.rblock;

  }

  if(rec_id.block == -1 && rec_id.slot == -1){
    if(relId == RELCAT_RELID)
        return E_MAXRELATIONS;
    RecBuffer recBuff;
    blockNum = recBuff.getBlockNum();

    if(blockNum == E_DISKFULL)
        return E_DISKFULL;

    rec_id.block = blockNum;
    rec_id.slot = 0;

    HeadInfo head1;

    head1.blockType = REC;
    head1.pblock = -1;
    head1.numAttrs = numAttrs;
    head1.numSlots = numSlots;
    head1.numEntries = 0;
    head1.rblock = -1;

    if(blockNum != relCatEntry.firstBlk)
        head1.lblock = prevBlockNum;
    else
        head1.lblock = -1;
    
    recBuff.setHeader(&head1);
    unsigned char *slotMap = ((unsigned char*) malloc (numSlots*sizeof(unsigned char)));
    for(int i=0; i<numSlots; i++)
        slotMap[i] = SLOT_UNOCCUPIED;
    recBuff.RecBuffer::setSlotMap(slotMap);

    if(prevBlockNum != -1){
        RecBuffer prevRecBuff(prevBlockNum);
        HeadInfo head2;
        prevRecBuff.getHeader(&head2);
        head2.rblock = blockNum;
        prevRecBuff.setHeader(&head2);
    }
    else{
        relCatEntry.firstBlk = blockNum;
        RelCacheTable::setRelCatEntry(relId, &relCatEntry);
    }

  }

    RecBuffer recBuff2(rec_id.block);
    recBuff2.setRecord(record, rec_id.slot);
    unsigned char *slotMap = ((unsigned char*) malloc (numSlots*sizeof(unsigned char)));
    recBuff2.getSlotMap(slotMap);
    slotMap[rec_id.slot] = SLOT_OCCUPIED;
    recBuff2.setSlotMap(slotMap);
    
    HeadInfo blockHeader;
	recBuff2.getHeader(&blockHeader);
	blockHeader.numEntries++;
	recBuff2.setHeader(&blockHeader);

    relCatEntry.numRecs++;
    RelCacheTable::setRelCatEntry(relId, &relCatEntry);

  return SUCCESS;

}