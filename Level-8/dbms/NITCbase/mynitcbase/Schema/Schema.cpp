#include "Schema.h"
#include <iostream>
#include <cmath>
#include <cstring>
int Schema::openRel(char relName[ATTR_SIZE]) {
  int ret = OpenRelTable::openRel(relName);

  // the OpenRelTable::openRel() function returns the rel-id if successful
  // a valid rel-id will be within the range 0 <= relId < MAX_OPEN and any
  // error codes will be negative
  if(ret >= 0){
    return SUCCESS;
  }

  //otherwise it returns an error message
  return ret;
}

int Schema::closeRel(char relName[ATTR_SIZE]) {
  if (strcmp(relName, RELCAT_RELNAME) == 0 || strcmp(relName, ATTRCAT_RELNAME) == 0) {
    return E_NOTPERMITTED;
  }

  // this function returns the rel-id of a relation if it is open or
  // E_RELNOTOPEN if it is not. we will implement this later.
  int relId = OpenRelTable::getRelId(relName);

  if (relId < 0) {
    return E_RELNOTOPEN;
  }
  //check later

  return OpenRelTable::closeRel(relId);
}

int Schema::renameRel(char oldRelName[ATTR_SIZE], char newRelName[ATTR_SIZE]) {

  // We check if the oldRelName is RELCAT_RELNAME or ATTRCAT_RELNAME and return E_NOTPERMITTED if true.
  // We check if the relation with name oldRelName is open and return E_RELNOTOPEN if not.
  // We call BlockAccess::renameRelation(oldRelName, newRelName) and return the return value of that function.

    if(strcmp(oldRelName,RELCAT_RELNAME) == 0 || strcmp(oldRelName,ATTRCAT_RELNAME) == 0)
        return E_NOTPERMITTED;
    
    if(strcmp(newRelName,RELCAT_RELNAME) == 0 || strcmp(newRelName,ATTRCAT_RELNAME) == 0)
        return E_NOTPERMITTED;

    if(OpenRelTable::getRelId(oldRelName) != E_RELNOTOPEN)
        return E_RELOPEN;
    
    int retVal = BlockAccess::renameRelation(oldRelName,newRelName);
    return retVal;
}

int Schema::renameAttr(char relName[ATTR_SIZE], char oldAttrName[ATTR_SIZE], char newAttrName[ATTR_SIZE]) {
  // We check if the relation with name relName is open and return E_RELNOTOPEN if not.
  // We call BlockAccess::renameAttribute(relName, oldAttrName, newAttrName) and return the return value of that function.
    if(strcmp(relName,RELCAT_RELNAME) == 0 || strcmp(relName,ATTRCAT_RELNAME) == 0)
        return E_NOTPERMITTED;

    if(OpenRelTable::getRelId(relName) != E_RELNOTOPEN)
        return E_RELOPEN;

    int retVal = BlockAccess::renameAttribute(relName,oldAttrName,newAttrName);
    return retVal;
}


int Schema::createRel(char relName[], int nAttrs, char attrs[][ATTR_SIZE], int attrtype[]){
  std::cout<<"schema 1\n";
  Attribute relNameAsAttribute;
  strcpy(relNameAsAttribute.sVal, relName);
  char relationName[10];
  strcpy(relationName, "RelName");

  RecId targetRelId;
  std::cout<<"schema 2\n";

  RelCacheTable::resetSearchIndex(RELCAT_RELID);
  std::cout<<"schema 3\n";
  targetRelId = BlockAccess::linearSearch(RELCAT_RELID, relationName, relNameAsAttribute , EQ);
  std::cout<<"schema 4 after linear search\n";
  

  if(targetRelId.block != -1 && targetRelId.slot != -1){
    return E_RELEXIST;
  }

  for(int i = 0; i < nAttrs; i++){
    std::cout<<"schema 5 for loop\n";
    for(int attrIndex = i+1; attrIndex < nAttrs; attrIndex++){
      if(strcmp(attrs[i], attrs[attrIndex]) == 0){
        return E_DUPLICATEATTR;
      }
    }
  }

  Attribute relCatRecord[RELCAT_NO_ATTRS];
  strcpy(relCatRecord[RELCAT_REL_NAME_INDEX].sVal, relName);
  relCatRecord[RELCAT_NO_ATTRIBUTES_INDEX].nVal = nAttrs;
  relCatRecord[RELCAT_NO_RECORDS_INDEX].nVal = 0;
  relCatRecord[RELCAT_FIRST_BLOCK_INDEX].nVal = -1;
  relCatRecord[RELCAT_LAST_BLOCK_INDEX].nVal = -1;

  int numberSlotsPerBlk = floor((2016 / (ATTR_SIZE * nAttrs + 1)));
  relCatRecord[RELCAT_NO_SLOTS_PER_BLOCK_INDEX].nVal = numberSlotsPerBlk;

  int retVal = BlockAccess::insert(RELCAT_RELID, relCatRecord);
  if(retVal != SUCCESS)
    return retVal;
  
  for(int attrIndex = 0; attrIndex < nAttrs; attrIndex++){
    Attribute attrCatRecord[ATTRCAT_NO_ATTRS];
    strcpy(attrCatRecord[ATTRCAT_REL_NAME_INDEX].sVal, relName);
    strcpy(attrCatRecord[ATTRCAT_ATTR_NAME_INDEX].sVal, attrs[attrIndex]);
    attrCatRecord[ATTRCAT_ATTR_TYPE_INDEX].nVal = attrtype[attrIndex];
    attrCatRecord[ATTRCAT_PRIMARY_FLAG_INDEX].nVal = -1;
    attrCatRecord[ATTRCAT_ROOT_BLOCK_INDEX].nVal = -1;
    attrCatRecord[ATTRCAT_OFFSET_INDEX].nVal = attrIndex;

    retVal = BlockAccess::insert(ATTRCAT_RELID, attrCatRecord);
    if(retVal != SUCCESS){
      Schema::deleteRel(relName);
      return E_DISKFULL;
    }
  }
  return SUCCESS;
}

int Schema::deleteRel(char *relName){

  if(strcmp(relName, RELCAT_RELNAME) == 0 || strcmp(relName, ATTRCAT_RELNAME) == 0){
    return E_NOTPERMITTED;
  }
  int relId = OpenRelTable::getRelId(relName);
  if(relId != E_RELNOTOPEN){
    return E_RELOPEN;
  }

  int retVal = BlockAccess::deleteRelation(relName);
  std::cout<<"error:"<<retVal<<'\n';
  return retVal;

}