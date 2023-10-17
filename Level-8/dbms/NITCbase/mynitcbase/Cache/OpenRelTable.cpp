#include "OpenRelTable.h"
#include <stdlib.h>
#include <cstring>
#include <iostream>


OpenRelTableMetaInfo OpenRelTable::tableMetaInfo[MAX_OPEN];

OpenRelTable::OpenRelTable() {

  // initialize relCache and attrCache with nullptr
  for (int i = 0; i < MAX_OPEN; ++i) {
    RelCacheTable::relCache[i] = nullptr;
    AttrCacheTable::attrCache[i] = nullptr;
    OpenRelTable::tableMetaInfo[i].free= true;
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

  // set up the relation cache entry for the attribute catalog similarly
  // from the record at RELCAT_SLOTNUM_FOR_ATTRCAT
  RecBuffer relCatBlock1(RELCAT_BLOCK);
  Attribute relCatRecord1[RELCAT_NO_ATTRS];
  relCatBlock.getRecord(relCatRecord1, RELCAT_SLOTNUM_FOR_ATTRCAT);
  
  struct RelCacheEntry attrRelCacheEntry;
  RelCacheTable::recordToRelCatEntry(relCatRecord1,&attrRelCacheEntry.relCatEntry);
  attrRelCacheEntry.recId.block= RELCAT_BLOCK;
  attrRelCacheEntry.recId.slot= RELCAT_SLOTNUM_FOR_ATTRCAT;

  // set the value at RelCacheTable::relCache[ATTRCAT_RELID]
  RelCacheTable::relCache[ATTRCAT_RELID] = (struct RelCacheEntry*)malloc(sizeof(RelCacheEntry));
  *(RelCacheTable::relCache[ATTRCAT_RELID]) = attrRelCacheEntry;


// Setting up Students in Relation Cache

  // relCatBlock.getRecord(relCatRecord, 2);
  // RelCacheTable::recordToRelCatEntry(relCatRecord,&attrRelCacheEntry.relCatEntry);
  // attrRelCacheEntry.recId.block = RELCAT_BLOCK;
  // attrRelCacheEntry.recId.slot = 2;
  
  // RelCacheTable::relCache[2]= (struct RelCacheEntry*) malloc (sizeof(RelCacheEntry));
  // *(RelCacheTable::relCache[2]) = attrRelCacheEntry;
  
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
  // iterate through all the attributes of the relation catalog and create a linked
  // list of AttrCacheEntry (slots 0 to 5)
  // for each of the entries, set
  //    attrCacheEntry.recId.block = ATTRCAT_BLOCK;
  //    attrCacheEntry.recId.slot = i   (0 to 5)
  //    and attrCacheEntry.next appropriately
  // NOTE: allocate each entry dynamically using malloc
   
  // set the next field in the last entry to nullptr
  
  AttrCacheTable::attrCache[RELCAT_RELID] = headrel;
  
  /**** setting up Attribute Catalog relation in the Attribute Cache Table ****/
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


//// Setting up Students in ATTRIBUTE CACHE
  // RecBuffer attrCatBlockStu(ATTRCAT_BLOCK);
  // Attribute attrCatRecordStu[ATTRCAT_NO_ATTRS];
  // struct AttrCacheEntry* headStu=NULL;
  // for(int i=15; i>=12; i--){
  // 	struct AttrCacheEntry* AttrCacheEntry = (struct AttrCacheEntry*) malloc (sizeof (struct AttrCacheEntry));
  // 	attrCatBlockStu.getRecord(attrCatRecordStu, i);
  // 	AttrCacheTable::recordToAttrCatEntry(attrCatRecordStu, &AttrCacheEntry->attrCatEntry);
  // 	AttrCacheEntry->recId.block = ATTRCAT_BLOCK;
  // 	AttrCacheEntry->recId.slot = i;
  // 	AttrCacheEntry->next = headStu;
  // 	headStu = AttrCacheEntry; 
  // }

  // AttrCacheTable::attrCache[2] = headStu;



  //////////////////////////////// Stage 5 ///////////////////////////////////////
  // set up table meta info
  // in the tableMetaInfo array
  //   set free = false for RELCAT_RELID and ATTRCAT_RELID
  //   set relname for RELCAT_RELID and ATTRCAT_RELID

  tableMetaInfo[RELCAT_RELID].free = false;
  strcpy(tableMetaInfo[RELCAT_RELID].relName, RELCAT_RELNAME);
  tableMetaInfo[ATTRCAT_RELID].free = false;
  strcpy(tableMetaInfo[ATTRCAT_RELID].relName, ATTRCAT_RELNAME);

}
/* This function will open a relation having name `relName`.
Since we are currently only working with the relation and attribute catalog, we
will just hardcode it. In subsequent stages, we will loop through all the relations
and open the appropriate one.
*/
// int OpenRelTable::getRelId(char relName[ATTR_SIZE]) {
//   //std::cout<<"RELNAME CHECK:"<<relName<<" ";
//   if(!strcmp(relName, RELCAT_RELNAME))
//   	return RELCAT_RELID;
//   else if(!strcmp(relName, ATTRCAT_RELNAME))
//   	return ATTRCAT_RELID;
//   else if(!strcmp(relName, "Students"))
//   	return 2;
//   // if relname is RELCAT_RELNAME, return RELCAT_RELID
//   // if relname is ATTRCAT_RELNAME, return ATTRCAT_RELID

//   return E_RELNOTOPEN;
// }
int OpenRelTable::getFreeOpenRelTableEntry() {

  /* traverse through the tableMetaInfo array,
    find a free entry in the Open Relation Table.*/

  // if found return the relation id, else return E_CACHEFULL.
  int i;
  for(i=2; i<MAX_OPEN; i++){
  	if(tableMetaInfo[i].free == true)
  		return i;
  }
  std::cout<<i<<'\n';
  return E_CACHEFULL;
}
int OpenRelTable::getRelId(char relName[ATTR_SIZE]) {

  /* traverse through the tableMetaInfo array,
    find the entry in the Open Relation Table corresponding to relName.*/

  // if found return the relation id, else indicate that the relation do not
  // have an entry in the Open Relation Table.
  for(int i=0; i<MAX_OPEN; i++){
  	if(!strcmp(tableMetaInfo[i].relName, (char*)relName) && tableMetaInfo[i].free != true)
  		return i;
  }
  return E_RELNOTOPEN;
  // else return not in cache??
}

int OpenRelTable::openRel(char relName[ATTR_SIZE]) {

  int RelIdCheck;
  RelIdCheck = getRelId(relName);
  if(RelIdCheck >= 0 && RelIdCheck < MAX_OPEN){
    return RelIdCheck;
  }

  /* find a free slot in the Open Relation Table*/

  int freeSlot = getFreeOpenRelTableEntry();
  
  if (freeSlot == E_CACHEFULL){
    return E_CACHEFULL;
  }

  // let relId be used to store the free slot.
  int relId = freeSlot;

  /****** Setting up Relation Cache entry for the relation ******/

  /* search for the entry with relation name, relName, in the Relation Catalog using
      BlockAccess::linearSearch().
      Care should be taken to reset the searchIndex of the relation RELCAT_RELID
      before calling linearSearch().*/
  
  Attribute relationName;
  strcpy(relationName.sVal, relName);
  RelCacheTable::resetSearchIndex(RELCAT_RELID);
  
  // relcatRecId stores the rec-id of the relation `relName` in the Relation Catalog.
  RecId relcatRecId;
  char attributeRelName[10];
  strcpy(attributeRelName, "RelName");
  
  relcatRecId = BlockAccess::linearSearch(RELCAT_RELID, attributeRelName, relationName, EQ);
  
  if ( relcatRecId.block == -1 && relcatRecId.slot == -1) {
    // (the relation is not found in the Relation Catalog.)
    return E_RELNOTEXIST;
  }

  /* read the record entry corresponding to relcatRecId and create a relCacheEntry
      on it using RecBuffer::getRecord() and RelCacheTable::recordToRelCatEntry().
      update the recId field of this Relation Cache entry to relcatRecId.
      use the Relation Cache entry to set the relId-th entry of the RelCacheTable.
    NOTE: make sure to allocate memory for the RelCacheEntry using malloc()
  */

 RecBuffer recBuff (relcatRecId.block);
 Attribute relCatRecord[RELCAT_NO_ATTRS];
 recBuff.getRecord(relCatRecord, relcatRecId.slot);
  struct RelCacheEntry* relCacheEntry = (struct RelCacheEntry*) malloc (sizeof(RelCacheEntry));
  RelCacheTable::recordToRelCatEntry(relCatRecord, &relCacheEntry->relCatEntry);
  relCacheEntry->recId = relcatRecId;
  RelCacheTable::relCache[relId] = relCacheEntry;	
  /****** Setting up Attribute Cache entry for the relation ******/

  // let listHead be used to hold the head of the linked list of attrCache entries.
  AttrCacheEntry* listHead = NULL, *prev = NULL;

  /*iterate over all the entries in the Attribute Catalog corresponding to each
  attribute of the relation relName by multiple calls of BlockAccess::linearSearch()
  care should be taken to reset the searchIndex of the relation, ATTRCAT_RELID,
  corresponding to Attribute Catalog before the first call to linearSearch().*/
  RelCacheTable::resetSearchIndex(ATTRCAT_RELID);
  Attribute currentRelName;
  strcpy(currentRelName.sVal, relName);
  
  while(true)
  {
      /* let attrcatRecId store a valid record id an entry of the relation, relName,
      in the Attribute Catalog.*/
    RecId attrcatRecId = BlockAccess::linearSearch(ATTRCAT_RELID, attributeRelName, currentRelName, EQ);
    
    if(attrcatRecId.block == -1 && attrcatRecId.slot == -1)
      break;
    RecBuffer currentRecBlock (attrcatRecId.block);
    Attribute currentRec[ATTRCAT_NO_ATTRS];
    
    currentRecBlock.getRecord(currentRec, attrcatRecId.slot);
   
    
   
    struct AttrCacheEntry* AttrCacheEntry = (struct AttrCacheEntry*) malloc (sizeof (struct AttrCacheEntry));
  	
    AttrCacheTable::recordToAttrCatEntry(currentRec, &AttrCacheEntry->attrCatEntry);
    AttrCacheEntry->recId = attrcatRecId;
    AttrCacheEntry->next = NULL;
     //std::cout<<"check:"<<freeSlot<<"\n";
    //std::cout<<AttrCacheEntry->attrCatEntry.attrName<<'\n';
    if(prev == NULL)
      listHead = AttrCacheEntry;
    else
      prev->next = AttrCacheEntry;
    prev = AttrCacheEntry;
    
      /* read the record entry corresponding to attrcatRecId and create an
      Attribute Cache entry on it using RecBuffer::getRecord() and
      AttrCacheTable::recordToAttrCatEntry().
      update the recId field of this Attribute Cache entry to attrcatRecId.
      add the Attribute Cache entry to the linked list of listHead .*/
      // NOTE: make sure to allocate memory for the AttrCacheEntry using malloc()
  }
  AttrCacheTable::attrCache[relId] = listHead;
  prev = listHead;
  // while(prev != NULL){
  //   // std::cout<<"check attr:";
  //   // std::cout<<prev->attrCatEntry.attrName<<'\n';
  //   prev = prev->next;
  // }
  // set the relIdth entry of the AttrCacheTable to listHead.

  /****** Setting up metadata in the Open Relation Table for the relation******/
  tableMetaInfo[relId].free = false;
  strcpy(tableMetaInfo[relId].relName, relName);
  // update the relIdth entry of the tableMetaInfo with free as false and
  // relName as the input.

  return relId;
}


int OpenRelTable::closeRel(int relId) {
  if (relId == RELCAT_RELID || relId == ATTRCAT_RELID) {
    return E_NOTPERMITTED;
  }

  if (relId < 0 || relId >= MAX_OPEN) {
    return E_OUTOFBOUND;
  }

  if (tableMetaInfo[relId].free == true) {
    return E_RELNOTOPEN;
  }

  /* Releasing the Relation Cache Entry of the Relation */

  if(RelCacheTable::relCache[relId]->dirty == true){
    // if the relation cache entry is dirty, write it back to the relation catalog
    Attribute relCatRecord[RELCAT_NO_ATTRS];
    RelCacheTable::relCatEntryToRecord(&RelCacheTable::relCache[relId]->relCatEntry, relCatRecord);
    RecBuffer relCatBlock(RelCacheTable::relCache[relId]->recId.block);
    relCatBlock.setRecord(relCatRecord, RelCacheTable::relCache[relId]->recId.slot);

  }

  /* Releasing the Attribute Cache Entry of the relation */

  // free the memory allocated in the attribute caches which was allocated in openRel
  AttrCacheEntry* current = AttrCacheTable::attrCache[relId];
  while(current != NULL){
    AttrCacheEntry* temp = current;
    current = current->next;
    free(temp);
  }

  // free the memory allocated in the relation and attribute caches which was
  // allocated in the OpenRelTable::openRel() function

  free(RelCacheTable::relCache[relId]);

  // update `tableMetaInfo` to set `relId` as a free slot
  tableMetaInfo[relId].free = true;
  // update `relCache` and `attrCache` to set the entry at `relId` to nullptr
  RelCacheTable::relCache[relId] = nullptr;
  AttrCacheTable::attrCache[relId] = nullptr;
  
  return SUCCESS;
}


// OpenRelTable::~OpenRelTable() {
  
//   for (int i = 2; i < MAX_OPEN; ++i) {
//     if (!tableMetaInfo[i].free) {
//       OpenRelTable::closeRel(i); // we will implement this function later
//     }
//   }
//   // free all the memory that you allocated in the constructor
// }

// STAGE 8
OpenRelTable::~OpenRelTable() {
  
  for (int i = 2; i < MAX_OPEN; ++i) {
    if (!tableMetaInfo[i].free) {
      OpenRelTable::closeRel(i); // we will implement this function later
    }
  }

  if(RelCacheTable::relCache[ATTRCAT_RELID]->dirty == true){
    // if the relation cache entry is dirty, write it back to the relation catalog
    Attribute relCatRecord[RELCAT_NO_ATTRS];
    RelCacheTable::relCatEntryToRecord(&RelCacheTable::relCache[ATTRCAT_RELID]->relCatEntry, relCatRecord);
    RecBuffer relCatBlock(RelCacheTable::relCache[ATTRCAT_RELID]->recId.block);
    relCatBlock.setRecord(relCatRecord, RelCacheTable::relCache[ATTRCAT_RELID]->recId.slot);

  }

  if(RelCacheTable::relCache[RELCAT_RELID]->dirty == true){
    // if the relation cache entry is dirty, write it back to the relation catalog
    Attribute relCatRecord[RELCAT_NO_ATTRS];
    RelCacheTable::relCatEntryToRecord(&RelCacheTable::relCache[RELCAT_RELID]->relCatEntry, relCatRecord);
    RecBuffer relCatBlock(RelCacheTable::relCache[RELCAT_RELID]->recId.block);
    relCatBlock.setRecord(relCatRecord, RelCacheTable::relCache[RELCAT_RELID]->recId.slot);

  }

  free(RelCacheTable::relCache[RELCAT_RELID]);
  free(RelCacheTable::relCache[ATTRCAT_RELID]);

  AttrCacheEntry* current = AttrCacheTable::attrCache[RELCAT_RELID];
  while(current != NULL){
    AttrCacheEntry* temp = current;
    current = current->next;
    free(temp);
  }

  current = AttrCacheTable::attrCache[ATTRCAT_RELID];
  while(current != NULL){
    AttrCacheEntry* temp = current;
    current = current->next;
    free(temp);
  }
}