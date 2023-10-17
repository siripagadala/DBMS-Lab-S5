#include "BlockAccess.h"
#include <stdlib.h>
#include <stdio.h>
#include <cstring>
#include <iostream>

using namespace std;

int compareAttrs(union Attribute attr1, union Attribute attr2, int attrType) {

    double diff;
    if(attrType == STRING){
    	//printf("attr1 %s attr2 %s\n", attr1.sVal, attr2.sVal);
        diff = strcmp(attr1.sVal, attr2.sVal);
    }
    // if attrType == STRING
    //     diff = strcmp(attr1.sval, attr2.sval)
    else
    	diff = attr1.nVal - attr2.nVal;
    // else
    //     diff = attr1.nval - attr2.nval

    if(diff > 0)
    	return 1;
    else
    if(diff < 0)
    	return -1;
    else
    	return 0;
    /*
    if diff > 0 then return 1
    if diff < 0 then return -1
    if diff = 0 then return 0
    */
}

RecId BlockAccess::linearSearch(int relId, char attrName[ATTR_SIZE], union Attribute attrVal, int op) {
    // get the previous search index of the relation relId from the relation cache
    // (use RelCacheTable::getSearchIndex() function)
    RecId prevRecId ;
    
    RelCacheTable::getSearchIndex(relId, &prevRecId) ;
    
    //cout << prevRecId.block ;
    //cout <<  prevRecId.slot ;
    
    // let block and slot denote the record id of the record being currently checked
    int block , slot ;
    // if the current search index record is invalid(i.e. both block and slot = -1)
    if (prevRecId.block== -1 && prevRecId.slot == -1)
    {
        // (no hits from previous search; search should start from the
        // first record itself)

        // get the first record block of the relation from the relation cache
        // (use RelCacheTable::getRelCatEntry() function of Cache Layer)
        RelCatEntry relCatBuf ;
        RelCacheTable::getRelCatEntry(relId, &relCatBuf);
        block = relCatBuf.firstBlk ;
        //cout << relCatBuf.relName << endl ;
        //cout << relCatBuf.firstBlk << endl ;
        //cout << relCatBuf.lastBlk << endl ;
        //cout << relCatBuf.numRecs << endl ;
        //cout << block << endl ;
        slot = 0 ;
        
    }
    else
    {
        // (there is a hit from previous search; search should start from
        // the record next to the search index record)
        
        block = prevRecId.block ;
        //cout << block << endl ;
        slot = prevRecId.slot + 1 ;
    }

    
    while (block != -1)
    {
        
          RecBuffer currBuffer(block) ;
        // get the record with id (block, slot) using RecBuffer::getRecord()
          
        // get header of the block using RecBuffer::getHeader() function
          HeadInfo currHeader;
          currBuffer.getHeader(&currHeader);
          
          Attribute currRecord[currHeader.numAttrs] ;
          currBuffer.getRecord(currRecord, slot);
          
        // get slot map of the block using RecBuffer::getSlotMap() function
          unsigned char slotMap[currHeader.numSlots];
          
          currBuffer.getSlotMap(slotMap) ;
          
        // If slot >= the number of slots per block(i.e. no more slots in this block)
        //cout << slot ;
        //cout << currHeader.numSlots ;
        if(slot >= currHeader.numSlots){
            // update block = right block of block
            // update slot = 0
            block = currHeader.rblock ;
            slot = 0 ;
            
            continue;  // continue to the beginning of this while loop
        }
        //SLOT_UNOCCUPIED '0'
        // if slot is free skip the loop
        // (i.e. check if slot'th entry in slot map of block contains SLOT_UNOCCUPIED)
        if(slotMap[slot] == '0'){
            // increment slot and continue to the next record slot
            slot++ ;
            continue ;
        }

        // compare record's attribute value to the the given attrVal as below:
             /*
            firstly get the attribute offset for the attrName attribute
            from the attribute cache entry of the relation using
            AttrCacheTable::getAttrCatEntry()
        */
        /* use the attribute offset to get the value of the attribute from
           current record */   
           //cout << "Hi6\n" ;
        AttrCatEntry* attrCatBuf = (AttrCatEntry*)malloc(sizeof(AttrCatEntry));    
        AttrCacheTable::getAttrCatEntry(relId , attrName , attrCatBuf) ;
        //cout << attrCatBuf->attrName << endl ;
        int cmpVal = compareAttrs( currRecord[attrCatBuf->offset], attrVal , attrCatBuf->attrType);  
        // will store the difference between the attributes
        // set cmpVal using compareAttrs()

        
        if (
            (op == NE && cmpVal != 0) ||    // if op is "not equal to"
            (op == LT && cmpVal < 0) ||     // if op is "less than"
            (op == LE && cmpVal <= 0) ||    // if op is "less than or equal to"
            (op == EQ && cmpVal == 0) ||    // if op is "equal to"
            (op == GT && cmpVal > 0) ||     // if op is "greater than"
            (op == GE && cmpVal >= 0)       // if op is "greater than or equal to"
        ) {
            
            /*
            set the search index in the relation cache as
            the record id of the record that satisfies the given condition
            (use RelCacheTable::setSearchIndex function)
            */
           RecId ansRecId ;
           ansRecId.block = block ;
           ansRecId.slot = slot ;
           RelCacheTable::setSearchIndex(relId ,&ansRecId) ;
           //cout << block << endl ;
           //cout << slot << endl ;
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

int BlockAccess::search(int relId, Attribute *record, char attrName[ATTR_SIZE], Attribute attrVal, int op){

    RecId recId;
    recId = BlockAccess::linearSearch(relId, attrName, attrVal, op);

    if(recId.block == -1 && recId.slot == -1)
        return E_NOTFOUND;

    RecBuffer recBuff(recId.block);
    recBuff.getRecord(record, recId.slot);

    return SUCCESS;
    
}

int BlockAccess::deleteRelation(char relName[ATTR_SIZE]){

    if(strcmp(relName, RELCAT_RELNAME) == 0 || strcmp(relName, ATTRCAT_RELNAME) == 0)
        return E_NOTPERMITTED;
    
    RelCacheTable::resetSearchIndex(RELCAT_RELID);
    Attribute relNameAttr;
    strcpy(relNameAttr.sVal, relName);
    char relationName[10];
    strcpy(relationName, "RelName");

    RecId recId = BlockAccess::linearSearch(RELCAT_RELID, relationName, relNameAttr, EQ);

    if(recId.block == -1 && recId.slot == -1)
        return E_RELNOTEXIST;

    Attribute relCatRecord[RELCAT_NO_ATTRS];

    RecBuffer recBuff(recId.block);
    recBuff.getRecord(relCatRecord, recId.slot);

    int firstBlk = relCatRecord[RELCAT_FIRST_BLOCK_INDEX].nVal;
    int numAttrs = relCatRecord[RELCAT_NO_ATTRIBUTES_INDEX].nVal;

    int currentBlk, nextBlk;
    currentBlk = firstBlk;
    
    while(currentBlk != -1){

        RecBuffer recBuff(currentBlk);
        HeadInfo head;
        recBuff.getHeader(&head);
        nextBlk = head.rblock;
        recBuff.releaseBlock();
        currentBlk = nextBlk;

    }

    RelCacheTable::resetSearchIndex(ATTRCAT_RELID);

    int numberOfAttributesDeleted = 0;

    while(true){

        RecId attrCatRecId;
        attrCatRecId = BlockAccess::linearSearch(ATTRCAT_RELID, relationName, relNameAttr, EQ);

        if(attrCatRecId.block == -1 && attrCatRecId.slot == -1)
            break;

        numberOfAttributesDeleted++;

        RecBuffer recBuff(attrCatRecId.block);
        HeadInfo head;
        recBuff.getHeader(&head);

        Attribute attrCatRecord[ATTRCAT_NO_ATTRS];
        recBuff.getRecord(attrCatRecord, attrCatRecId.slot);

        int rootBlock = attrCatRecord[ATTRCAT_ROOT_BLOCK_INDEX].nVal;

        unsigned char *slotMap = (unsigned char*) malloc (head.numSlots*sizeof(unsigned char));
        recBuff.getSlotMap(slotMap);
        slotMap[attrCatRecId.slot] = SLOT_UNOCCUPIED;
        recBuff.setSlotMap(slotMap);

        if(head.numEntries == 1){
            head.numEntries--;
            recBuff.setHeader(&head);
            recBuff.releaseBlock();
        }
        else{
            head.numEntries--;
            recBuff.setHeader(&head);
        }

        if(head.numEntries == 0){
            
            RecBuffer prevRecBuff(head.lblock);
            HeadInfo prevHead;
            prevRecBuff.getHeader(&prevHead);
            prevHead.rblock = head.rblock;
            prevRecBuff.setHeader(&prevHead);
           
            if(head.rblock != -1){
                RecBuffer nextRecBuff(head.rblock);
                HeadInfo nextHead;
                nextRecBuff.getHeader(&nextHead);
                nextHead.lblock = head.lblock;
                nextRecBuff.setHeader(&nextHead);
            }
            else{
                
                //DOUBT
                RecBuffer relCatRecBuff(RELCAT_BLOCK);
                int lastBlk = head.lblock;
                relCatRecBuff.getRecord(relCatRecord, RELCAT_SLOTNUM_FOR_ATTRCAT);
                relCatRecord[RELCAT_LAST_BLOCK_INDEX].nVal = lastBlk;
                relCatRecBuff.setRecord(relCatRecord, RELCAT_SLOTNUM_FOR_ATTRCAT);

                RelCatEntry relCatEntry;
                RelCacheTable::getRelCatEntry(ATTRCAT_RELID, &relCatEntry);
                relCatEntry.lastBlk = lastBlk;

            }
            
            recBuff.releaseBlock();
        }
    }

    RecBuffer relCatRecBuff(RELCAT_BLOCK);

    HeadInfo relCatHead;
    relCatRecBuff.getHeader(&relCatHead);
    relCatHead.numEntries--;
    relCatRecBuff.setHeader(&relCatHead);

    unsigned char *slotMap = (unsigned char*) malloc (relCatHead.numSlots*sizeof(unsigned char));
    relCatRecBuff.getSlotMap(slotMap);
    slotMap[recId.slot] = SLOT_UNOCCUPIED;
    relCatRecBuff.setSlotMap(slotMap);

    RelCatEntry relCatEntry;

    RelCacheTable::getRelCatEntry(RELCAT_RELID, &relCatEntry);
    relCatEntry.numRecs--;
    RelCacheTable::setRelCatEntry(RELCAT_RELID, &relCatEntry);

    RelCacheTable::getRelCatEntry(ATTRCAT_RELID, &relCatEntry);
    relCatEntry.numRecs -= numberOfAttributesDeleted;
    RelCacheTable::setRelCatEntry(ATTRCAT_RELID, &relCatEntry);

    return SUCCESS;

}
