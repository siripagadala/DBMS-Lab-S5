#include "OpenRelTable.h"

#include <cstring>
#include <iostream>

using namespace std;

OpenRelTable::OpenRelTable() {

  // initialize relCache and attrCache with nullptr
  for (int i = 0; i < MAX_OPEN; ++i) {
    RelCacheTable::relCache[i] = nullptr;
    AttrCacheTable::attrCache[i] = nullptr;
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
  RecBuffer attrCatBlock(RELCAT_BLOCK);

  // set up the relation cache entry for the attribute catalog similarly
  // from the record at RELCAT_SLOTNUM_FOR_ATTRCAT
  Attribute attrCatRecord[RELCAT_NO_ATTRS];
  attrCatBlock.getRecord(attrCatRecord, RELCAT_SLOTNUM_FOR_ATTRCAT);

  struct RelCacheEntry attrRelCacheEntry;
  //AttrCacheTable::recordToRelCatEntry(attrCatRecord, &AttrCacheEntry.relCatEntry);
  RelCacheTable::recordToRelCatEntry(attrCatRecord, &attrRelCacheEntry.relCatEntry);
  attrRelCacheEntry.recId.block = RELCAT_BLOCK;
  attrRelCacheEntry.recId.slot = RELCAT_SLOTNUM_FOR_ATTRCAT;

  // set the value at RelCacheTable::relCache[ATTRCAT_RELID]

   RelCacheTable::relCache[ATTRCAT_RELID] = (struct RelCacheEntry*)malloc(sizeof(RelCacheEntry));
  *(RelCacheTable::relCache[ATTRCAT_RELID]) = attrRelCacheEntry;



  //for loading student relation into student cache
  
    RecBuffer studentRelCatBlock(RELCAT_BLOCK);
    Attribute studentRelCatRecord[RELCAT_NO_ATTRS];
    studentRelCatBlock.getRecord(studentRelCatRecord, 2);

    struct RelCacheEntry studentRelCacheEntry;
    RelCacheTable::recordToRelCatEntry(studentRelCatRecord, &studentRelCacheEntry.relCatEntry);
    studentRelCacheEntry.recId.block = RELCAT_BLOCK;
    studentRelCacheEntry.recId.slot = 2;

    // Allocate memory on the heap for the cache entry
    RelCacheTable::relCache[2] = (struct RelCacheEntry*)malloc(sizeof(RelCacheEntry));
    *(RelCacheTable::relCache[2]) = studentRelCacheEntry;


  /************ Setting up Attribute cache entries ************/
  // (we need to populate attribute cache with entries for the relation catalog
  //  and attribute catalog.)




  

  /**** setting up Relation Catalog relation in the Attribute Cache Table ****/
  RecBuffer attrCatBlock1(ATTRCAT_BLOCK);
  Attribute attrCatRecord1[ATTRCAT_NO_ATTRS];

  //attrCatBlock1.getRecord(attrCatRecord1, RELCAT_SLOTNUM_FOR_ATTRCAT);


  
  AttrCacheEntry *attrCacheHead = nullptr;
  AttrCacheEntry *attrCachePrev = nullptr;
  
  for(int i=0; i<=5; ++i){
    AttrCacheEntry *attrCacheEntry = (AttrCacheEntry*)malloc(sizeof(AttrCacheEntry));
    attrCatBlock1.getRecord(attrCatRecord1, i);
    AttrCacheTable::recordToAttrCatEntry(attrCatRecord1, &attrCacheEntry->attrCatEntry);
    attrCacheEntry->recId.block = ATTRCAT_BLOCK;
    attrCacheEntry->recId.slot = i;
    attrCacheEntry->next = nullptr;

    if(attrCachePrev){
        attrCachePrev->next = attrCacheEntry;
    }
    else{
       attrCacheHead = attrCacheEntry;
    }
    attrCachePrev = attrCacheEntry;
  }

  
 AttrCacheTable::attrCache[RELCAT_RELID] = attrCacheHead;




  AttrCacheEntry *attrCacheHead2 = nullptr;
  AttrCacheEntry *attrCachePrev2 = nullptr;

  
  RecBuffer attrCatBlock2(ATTRCAT_BLOCK);
  Attribute attrCatRecord2[ATTRCAT_NO_ATTRS];\

  //attrCatBlock2.getRecord(attrCatRecord2, RELCAT_SLOTNUM_FOR_ATTRCAT);
  
  for(int i=6; i <= 11; ++i){
    AttrCacheEntry *attrCacheEntry2 = (AttrCacheEntry*)malloc(sizeof(AttrCacheEntry));
    attrCatBlock2.getRecord(attrCatRecord2, i);
    AttrCacheTable::recordToAttrCatEntry(attrCatRecord2, &attrCacheEntry2->attrCatEntry);
    attrCacheEntry2->recId.block = ATTRCAT_BLOCK;
    attrCacheEntry2->recId.slot = i;
    attrCacheEntry2->next = nullptr;

    if(attrCachePrev2){
        attrCachePrev2->next = attrCacheEntry2;
    }
    else{
        attrCacheHead2 = attrCacheEntry2;
    }
    attrCachePrev2 = attrCacheEntry2;
  }
  AttrCacheTable::attrCache[ATTRCAT_RELID] = attrCacheHead2;



  //student in attribute cache

    RecBuffer studentAttrCatBlock(ATTRCAT_BLOCK);
    Attribute studentAttrCatRecord[ATTRCAT_NO_ATTRS];

    AttrCacheEntry *studentAttrCacheHead = nullptr;
    AttrCacheEntry *studentAttrCachePrev = nullptr;

    for (int i = 12; i <= 17; ++i) {
        AttrCacheEntry *attrCacheEntry3 = (AttrCacheEntry*)malloc(sizeof(AttrCacheEntry));
        studentAttrCatBlock.getRecord(studentAttrCatRecord, i);
        AttrCacheTable::recordToAttrCatEntry(studentAttrCatRecord, &attrCacheEntry3->attrCatEntry);
        attrCacheEntry3->recId.block = ATTRCAT_BLOCK;
        attrCacheEntry3->recId.slot = i;
        attrCacheEntry3->next = nullptr;

        if (studentAttrCachePrev) {
            studentAttrCachePrev->next = attrCacheEntry3;
        } else {
            studentAttrCacheHead = attrCacheEntry3;
        }
        studentAttrCachePrev = attrCacheEntry3;
    }

    AttrCacheTable::attrCache[2] = studentAttrCacheHead;

}




OpenRelTable::~OpenRelTable() {
  // free all the memory that you allocated in the constructor
  for(int i=0; i< MAX_OPEN; ++i){
    if(RelCacheTable::relCache[i]){
        free(RelCacheTable::relCache[i]);
        free(AttrCacheTable::attrCache[i]);
    }
  }
}