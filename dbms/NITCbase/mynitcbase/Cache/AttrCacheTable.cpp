#include "AttrCacheTable.h"

#include <cstring>
#include <iostream>

using namespace std;

AttrCacheEntry* AttrCacheTable::attrCache[MAX_OPEN];

/* returns the attrOffset-th attribute for the relation corresponding to relId
NOTE: this function expects the caller to allocate memory for `*attrCatBuf`
*/
int AttrCacheTable::getAttrCatEntry(int relId, char attrName[ATTR_SIZE], AttrCatEntry* attrCatBuf) {
  // check if 0 <= relId < MAX_OPEN and return E_OUTOFBOUND otherwise
  if(relId<0 || relId>=MAX_OPEN)
  	return E_OUTOFBOUND;
  // check if attrCache[relId] == nullptr and return E_RELNOTOPEN if true
  if(attrCache[relId] == nullptr)
  	return E_RELNOTOPEN;
  // traverse the linked list of attribute cache entries
  AttrCacheEntry* entry;
  for (entry = attrCache[relId]; entry != nullptr; entry = entry->next) {
    //if (entry->attrCatEntry.offset == attrOffset)
    if(!strcmp(attrName,entry->attrCatEntry.attrName)) {
	    *attrCatBuf= entry->attrCatEntry;
      break;
	//return SUCCESS;
      // copy entry->attrCatEntry to *attrCatBuf and return SUCCESS;
    }
  }
  if(entry!= nullptr){
    //printf("found\n");
  	return SUCCESS;
  }
  // there is no attribute at this offset
  //printf("not found\n");
  return E_ATTRNOTEXIST;
}

int AttrCacheTable::getAttrCatEntry(int relId, int attrOffset, AttrCatEntry* attrCatBuf) {
  // check if 0 <= relId < MAX_OPEN and return E_OUTOFBOUND otherwise
  if(relId<0 || relId>=MAX_OPEN)
  	return E_OUTOFBOUND;
  // check if attrCache[relId] == nullptr and return E_RELNOTOPEN if true
  if(attrCache[relId] == nullptr)
  	return E_RELNOTOPEN;
  // traverse the linked list of attribute cache entries
  AttrCacheEntry* entry;
  for (entry = attrCache[relId]; entry != nullptr; entry = entry->next) {
    if (entry->attrCatEntry.offset == attrOffset) {
	    *attrCatBuf= entry->attrCatEntry;
	return SUCCESS;
      // copy entry->attrCatEntry to *attrCatBuf and return SUCCESS;
    }
  }
  if(entry!= nullptr)
  	return SUCCESS;
  // there is no attribute at this offset
  return E_ATTRNOTEXIST;
}

/* Converts a attribute catalog record to AttrCatEntry struct
    We get the record as Attribute[] from the BlockBuffer.getRecord() function.
    This function will convert that to a struct AttrCatEntry type.
*/
void AttrCacheTable::recordToAttrCatEntry(union Attribute record[ATTRCAT_NO_ATTRS],
                                          AttrCatEntry* attrCatEntry) {
  strcpy(attrCatEntry->relName, record[ATTRCAT_REL_NAME_INDEX].sVal);
  strcpy(attrCatEntry->attrName, record[ATTRCAT_ATTR_NAME_INDEX].sVal);
  attrCatEntry->attrType= (int)record[ATTRCAT_ATTR_TYPE_INDEX].nVal;
  //std::cout<<attrCatEntry->attrType<<'\n'
  //attribute has usually only number or string val, bool conversion???
  attrCatEntry->primaryFlag= (bool)record[ATTRCAT_PRIMARY_FLAG_INDEX].nVal;
  attrCatEntry->rootBlock= (int)record[ATTRCAT_ROOT_BLOCK_INDEX].nVal;
  attrCatEntry->offset= (int)record[ATTRCAT_OFFSET_INDEX].nVal;
  // copy the rest of the fields in the record to the attrCacheEntry struct
}