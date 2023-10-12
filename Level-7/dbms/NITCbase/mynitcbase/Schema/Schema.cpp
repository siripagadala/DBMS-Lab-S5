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
