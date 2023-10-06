#include "OpenRelTable.h"

#include <cstring>
#include <iostream>

using namespace std;

OpenRelTableMetaInfo OpenRelTable::tableMetaInfo[MAX_OPEN];


OpenRelTable::OpenRelTable() {

  // initialize relCache and attrCache with nullptr
  for (int i = 0; i < MAX_OPEN; ++i) {
    RelCacheTable::relCache[i] = nullptr;
    AttrCacheTable::attrCache[i] = nullptr;
    OpenRelTable::tableMetaInfo[i].free = true;
  }

  /************ Setting up Relation Cache entries ************/
  // (we need to populate relation cache with entries for the relation catalog
  //  and attribute catalog.)

  /**** setting up Relation Catalog relation in the Relation Cache Table****/
  RecBuffer relCatBlock(RELCAT_BLOCK);

  Attribute relCatRecord[RELCAT_NO_ATTRS];
  relCatBlock.getRecord(relCatRecord, RELCAT_SLOTNUM_FOR_RELCAT);

  struct RelCacheEntry relCacheEntry;
  RelCacheTable::recordToRelCatEntry(relCatRecord, &relCacheEntry.relCatEntry);
  relCacheEntry.recId.block = RELCAT_BLOCK;
  relCacheEntry.recId.slot = RELCAT_SLOTNUM_FOR_RELCAT;

  // allocate this on the heap because we want it to persist outside this function
  RelCacheTable::relCache[RELCAT_RELID] = (struct RelCacheEntry*)malloc(sizeof(RelCacheEntry));
  *(RelCacheTable::relCache[RELCAT_RELID]) = relCacheEntry;

  /**** setting up Attribute Catalog relation in the Relation Cache Table ****/
  RecBuffer relCatBlock1(RELCAT_BLOCK);

  // set up the relation cache entry for the attribute catalog similarly
  // from the record at RELCAT_SLOTNUM_FOR_ATTRCAT
  Attribute relCatRecord1[RELCAT_NO_ATTRS];
  relCatBlock.getRecord(relCatRecord1, RELCAT_SLOTNUM_FOR_ATTRCAT);

  struct RelCacheEntry attrRelCacheEntry;
  RelCacheTable::recordToRelCatEntry(relCatRecord1, &attrRelCacheEntry.relCatEntry);
  attrRelCacheEntry.recId.block = RELCAT_BLOCK;
  attrRelCacheEntry.recId.slot = RELCAT_SLOTNUM_FOR_ATTRCAT;

  // set the value at RelCacheTable::relCache[ATTRCAT_RELID]

   RelCacheTable::relCache[ATTRCAT_RELID] = (struct RelCacheEntry*)malloc(sizeof(RelCacheEntry));
  *(RelCacheTable::relCache[ATTRCAT_RELID]) = attrRelCacheEntry;



  //for loading student relation into student cache
  
    // RecBuffer studentRelCatBlock(RELCAT_BLOCK);
    // Attribute studentRelCatRecord[RELCAT_NO_ATTRS];
    // studentRelCatBlock.getRecord(studentRelCatRecord, 2);

    // struct RelCacheEntry studentRelCacheEntry;
    // RelCacheTable::recordToRelCatEntry(studentRelCatRecord, &studentRelCacheEntry.relCatEntry);
    // studentRelCacheEntry.recId.block = RELCAT_BLOCK;
    // studentRelCacheEntry.recId.slot = 2;

    // // Allocate memory on the heap for the cache entry
    // RelCacheTable::relCache[2] = (struct RelCacheEntry*)malloc(sizeof(RelCacheEntry));
    // *(RelCacheTable::relCache[2]) = studentRelCacheEntry;


  /************ Setting up Attribute cache entries ************/
  // (we need to populate attribute cache with entries for the relation catalog
  //  and attribute catalog.)




  

  /**** setting up Relation Catalog relation in the Attribute Cache Table ****/
  RecBuffer attrCatBlock(ATTRCAT_BLOCK);
  Attribute attrCatRecord[ATTRCAT_NO_ATTRS];
  struct AttrCacheEntry* headrel=NULL;
  for(int i=RELCAT_NO_ATTRS-1; i>=0; i--){
  	struct  AttrCacheEntry* relAttrCacheEntry=(struct AttrCacheEntry*) malloc (sizeof(struct AttrCacheEntry));
  	attrCatBlock.getRecord(attrCatRecord, i);
  	AttrCacheTable::recordToAttrCatEntry(attrCatRecord,&relAttrCacheEntry->attrCatEntry);
  	relAttrCacheEntry->recId.block = ATTRCAT_BLOCK;
  	relAttrCacheEntry->recId.slot = i;
  	relAttrCacheEntry->next=headrel;
  	headrel=relAttrCacheEntry;
  }
  AttrCacheTable::attrCache[RELCAT_RELID] = headrel;


  RecBuffer attrCatBlock1(ATTRCAT_BLOCK);

  Attribute attrCatRecord1[ATTRCAT_NO_ATTRS];
  struct AttrCacheEntry* headattr=NULL;
  for(int i=11; i>=6; i--){
  	struct  AttrCacheEntry* AttrCacheEntry=(struct AttrCacheEntry*) malloc (sizeof(struct AttrCacheEntry));
  	attrCatBlock.getRecord(attrCatRecord1, i);
  	AttrCacheTable::recordToAttrCatEntry(attrCatRecord1,&AttrCacheEntry->attrCatEntry);
  	AttrCacheEntry->recId.block = ATTRCAT_BLOCK;
  	AttrCacheEntry->recId.slot = i;
  	AttrCacheEntry->next=headattr;
  	headattr=AttrCacheEntry;
  }
  
  // set up the attributes of the attribute cache similarly.
  // read slots 6-11 from attrCatBlock and initialise recId appropriately
  AttrCacheTable::attrCache[ATTRCAT_RELID] = headattr;
  // set the value at AttrCacheTable::attrCache[ATTRCAT_RELID]


  
  // AttrCacheEntry *attrCacheHead = nullptr;
  // AttrCacheEntry *attrCachePrev = nullptr;
  
  // for(int i=0; i<=5; ++i){
  //   AttrCacheEntry *attrCacheEntry = (AttrCacheEntry*)malloc(sizeof(AttrCacheEntry));
  //   attrCatBlock1.getRecord(attrCatRecord1, i);
  //   AttrCacheTable::recordToAttrCatEntry(attrCatRecord1, &attrCacheEntry->attrCatEntry);
  //   attrCacheEntry->recId.block = ATTRCAT_BLOCK;
  //   attrCacheEntry->recId.slot = i;
  //   attrCacheEntry->next = nullptr;

  //   if(attrCachePrev){
  //       attrCachePrev->next = attrCacheEntry;
  //   }
  //   else{
  //      attrCacheHead = attrCacheEntry;
  //   }
  //   attrCachePrev = attrCacheEntry;
  // }

  
 //AttrCacheTable::attrCache[RELCAT_RELID] = attrCacheHead;




  // AttrCacheEntry *attrCacheHead2 = nullptr;
  // AttrCacheEntry *attrCachePrev2 = nullptr;

  
  // RecBuffer attrCatBlock2(ATTRCAT_BLOCK);
  // Attribute attrCatRecord2[ATTRCAT_NO_ATTRS];

  // //attrCatBlock2.getRecord(attrCatRecord2, RELCAT_SLOTNUM_FOR_ATTRCAT);
  
  // for(int i=6; i <= 11; ++i){
  //   AttrCacheEntry *attrCacheEntry2 = (AttrCacheEntry*)malloc(sizeof(AttrCacheEntry));
  //   attrCatBlock2.getRecord(attrCatRecord2, i);
  //   AttrCacheTable::recordToAttrCatEntry(attrCatRecord2, &attrCacheEntry2->attrCatEntry);
  //   attrCacheEntry2->recId.block = ATTRCAT_BLOCK;
  //   attrCacheEntry2->recId.slot = i;
  //   attrCacheEntry2->next = nullptr;

  //   if(attrCachePrev2){
  //       attrCachePrev2->next = attrCacheEntry2;
  //   }
  //   else{
  //       attrCacheHead2 = attrCacheEntry2;
  //   }
  //   attrCachePrev2 = attrCacheEntry2;
  // }
  // AttrCacheTable::attrCache[ATTRCAT_RELID] = attrCacheHead2;



  //student in attribute cache

    // RecBuffer studentAttrCatBlock(ATTRCAT_BLOCK);
    // Attribute studentAttrCatRecord[ATTRCAT_NO_ATTRS];

    // AttrCacheEntry *studentAttrCacheHead = nullptr;
    // AttrCacheEntry *studentAttrCachePrev = nullptr;

    // for (int i = 12; i <= 17; ++i) {
    //     AttrCacheEntry *attrCacheEntry3 = (AttrCacheEntry*)malloc(sizeof(AttrCacheEntry));
    //     studentAttrCatBlock.getRecord(studentAttrCatRecord, i);
    //     AttrCacheTable::recordToAttrCatEntry(studentAttrCatRecord, &attrCacheEntry3->attrCatEntry);
    //     attrCacheEntry3->recId.block = ATTRCAT_BLOCK;
    //     attrCacheEntry3->recId.slot = i;
    //     attrCacheEntry3->next = nullptr;

    //     if (studentAttrCachePrev) {
    //         studentAttrCachePrev->next = attrCacheEntry3;
    //     } else {
    //         studentAttrCacheHead = attrCacheEntry3;
    //     }
    //     studentAttrCachePrev = attrCacheEntry3;
    // }

    // AttrCacheTable::attrCache[2] = studentAttrCacheHead;


    /************ Setting up tableMetaInfo entries ************/

  //   in the tableMetaInfo array
  //   set free = false for RELCAT_RELID and ATTRCAT_RELID
  tableMetaInfo[RELCAT_RELID].free = false;
  
  //   set relname for RELCAT_RELID and ATTRCAT_RELID
  strcpy(tableMetaInfo[RELCAT_RELID].relName, RELCAT_RELNAME);
  tableMetaInfo[ATTRCAT_RELID].free = false;
  strcpy(tableMetaInfo[ATTRCAT_RELID].relName, ATTRCAT_RELNAME);

}

int OpenRelTable::getFreeOpenRelTableEntry() {

  /* traverse through the tableMetaInfo array,
    find a free entry in the Open Relation Table.*/

  // if found return the relation id, else return E_CACHEFULL.

  for (int i = 2; i < MAX_OPEN; i++) {
        if (tableMetaInfo[i].free == true) {
            return i; // Return the relation id if a free entry is found
        }
    }
    printf("\n");
    return E_CACHEFULL; // Return E_CACHEFULL if no free entry is found
}

int OpenRelTable::getRelId(char relName[ATTR_SIZE]) {
  for (int i = 0; i < MAX_OPEN; i++) {
        if (!strcmp(tableMetaInfo[i].relName, (char*)relName)) {
            return i; // Return the relation id if an entry corresponding to relName is found
        }
    }
    return E_RELNOTOPEN; // Return E_RELNOTOPEN if no such entry is found

}


int OpenRelTable::openRel(char relName[ATTR_SIZE]) {
  printf("hello\n");
  int relIdCheck = getRelId(relName);
  printf("h1\n");
  if(relIdCheck >= 0 && relIdCheck < MAX_OPEN){
    return relIdCheck;
  }
  printf("h2\n");

  /* find a free slot in the Open Relation Table
     using OpenRelTable::getFreeOpenRelTableEntry(). */
  printf("h3\n");

int freeSlot = getFreeOpenRelTableEntry();
printf("h4\n");
  
  if (freeSlot == E_CACHEFULL){
    return E_CACHEFULL;
  }
  printf("h5\n");

  // let relId be used to store the free slot.
  int relId = freeSlot;

  // let relId be used to store the free slot.
  //int relId;

  /****** Setting up Relation Cache entry for the relation ******/

  /* search for the entry with relation name, relName, in the Relation Catalog using
      BlockAccess::linearSearch().
      Care should be taken to reset the searchIndex of the relation RELCAT_RELID
      before calling linearSearch().*/
printf("h6\n");
    Attribute relationName;
    strcpy(relationName.sVal, relName);
    RelCacheTable::resetSearchIndex(RELCAT_RELID);
printf("h7\n");
    RecId relcatRecId;
    char attributeRelName[10];
    strcpy(attributeRelName, "RelName");
printf("h8\n");

    relcatRecId = BlockAccess::linearSearch(RELCAT_RELID, attributeRelName, relationName, EQ);
    printf("block :%d Slot: %d\n",relcatRecId.block , relcatRecId.slot);
    //printf("block: %d Slot: %d\n", relcatRecId.block, relcatRecId.slot);

  // relcatRecId stores the rec-id of the relation `relName` in the Relation Catalog.

  if (relcatRecId.block == -1 && relcatRecId.slot == -1) {
    // (the relation is not found in the Relation Catalog.)
    return E_RELNOTEXIST;
}

  /* read the record entry corresponding to relcatRecId and create a relCacheEntry
      on it using RecBuffer::getRecord() and RelCacheTable::recordToRelCatEntry().
      update the recId field of this Relation Cache entry to relcatRecId.
      use the Relation Cache entry to set the relId-th entry of the RelCacheTable.
    NOTE: make sure to allocate memory for the RelCacheEntry using malloc()
  */
     RecBuffer recBuff(relcatRecId.block);
     Attribute relCatRecord[RELCAT_NO_ATTRS];
     recBuff.getRecord(relCatRecord, relcatRecId.slot);
     struct RelCacheEntry* relCacheEntry = (struct RelCacheEntry*)malloc(sizeof(RelCacheEntry));
     RelCacheTable::recordToRelCatEntry(relCatRecord, &relCacheEntry->relCatEntry);
     relCacheEntry->recId = relcatRecId;
     RelCacheTable::relCache[relId] = relCacheEntry;

  /****** Setting up Attribute Cache entry for the relation ******/

  // let listHead be used to hold the head of the linked list of attrCache entries.
  AttrCacheEntry* listHead = nullptr, *prev = NULL;

  // /*iterate over all the entries in the Attribute Catalog corresponding to each
  // attribute of the relation relName by multiple calls of BlockAccess::linearSearch()
  // care should be taken to reset the searchIndex of the relation, ATTRCAT_RELID,
  // corresponding to Attribute Catalog before the first call to linearSearch().*/
  // {
  //     /* let attrcatRecId store a valid record id an entry of the relation, relName,
  //     in the Attribute Catalog.*/
  //     RecId attrcatRecId;

  //     /* read the record entry corresponding to attrcatRecId and create an
  //     Attribute Cache entry on it using RecBuffer::getRecord() and
  //     AttrCacheTable::recordToAttrCatEntry().
  //     update the recId field of this Attribute Cache entry to attrcatRecId.
  //     add the Attribute Cache entry to the linked list of listHead .*/
  //     // NOTE: make sure to allocate memory for the AttrCacheEntry using malloc()
  // }
 RelCacheTable::resetSearchIndex(ATTRCAT_RELID);
 Attribute currentRelName;
 strcpy(currentRelName.sVal , relName);
  
  while(true) {
    // printf("hello every nya\n");
    // printf("goodmorning\n");
    
    RecId attrcatRecId = BlockAccess::linearSearch(ATTRCAT_RELID, attributeRelName, currentRelName, EQ);
        // printf("cock and ball torture\n");
        // printf("block: %d Slot: %d\n", attrcatRecId.block, attrcatRecId.slot);
        // Let attrcatRecId store a valid record id for an entry of the relation, relName, in the Attribute Catalog
    if(attrcatRecId.block == -1 && attrcatRecId.slot == -1){\
      break;
    }
      RecBuffer currentRecBlock(attrcatRecId.block);
      Attribute currentRec[ATTRCAT_NO_ATTRS];
      currentRecBlock.getRecord(currentRec, attrcatRecId.slot);

      struct AttrCacheEntry* AttrCacheEntry = (struct AttrCacheEntry*)malloc(sizeof(struct AttrCacheEntry));
      AttrCacheTable::recordToAttrCatEntry(currentRec, &AttrCacheEntry->attrCatEntry);
      AttrCacheEntry->recId = attrcatRecId;
      AttrCacheEntry->next = NULL;

      if(prev == NULL){
        listHead = AttrCacheEntry;
      }
      else{
        prev->next = AttrCacheEntry;
      }
      prev = AttrCacheEntry;
    }

    AttrCacheTable::attrCache[relId] = listHead;
    prev = listHead;
  
  while(prev != NULL){
    printf("check attr:");
    std::cout<<prev->attrCatEntry.attrName<<'\n';
    prev = prev->next;
  }

  tableMetaInfo[relId].free = false;
  strcpy(tableMetaInfo[relId].relName, relName);

  return relId;
}

int OpenRelTable::closeRel(int relId) {
    // Check if rel-id corresponds to relation catalog or attribute catalog
    if (relId == RELCAT_RELID || relId == ATTRCAT_RELID) {
        return E_NOTPERMITTED;
    }

    // Check if relId is within valid range
    if (relId < 0 || relId >= MAX_OPEN) {
        return E_OUTOFBOUND;
    }

    // Check if rel-id corresponds to a free slot
    if (tableMetaInfo[relId].free == true) {
        return E_RELNOTOPEN;
    }

    // Free the memory allocated in the relation and attribute caches which was
    // allocated in the OpenRelTable::openRel() function
    // You will need to implement the logic to free this memory.
    
    free(RelCacheTable::relCache[relId]);

    // Update tableMetaInfo to set relId as a free slot
    tableMetaInfo[relId].free = true;
    RelCacheTable::relCache[relId] = nullptr;

    return SUCCESS;
}



OpenRelTable::~OpenRelTable() {
  for (int i = 0; i < MAX_OPEN; ++i) {
    free(RelCacheTable::relCache[i]);
    free(AttrCacheTable::attrCache[i]);
  }

  for(int i=2; i<MAX_OPEN; ++i){
    if(!tableMetaInfo[i].free){
      OpenRelTable::closeRel(i);
    }
  }
}





