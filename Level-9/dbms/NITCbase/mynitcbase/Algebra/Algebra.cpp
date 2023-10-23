#include "Algebra.h"
#include<iostream>
#include <cstring>
using namespace std;

bool isNumber(char *str) {
  int len;
  float ignore;
  int ret = sscanf(str, "%f %n", &ignore, &len);
  return ret == 1 && len == strlen(str);
}

int Algebra::insert(char relName[ATTR_SIZE], int nAttrs, char record[][ATTR_SIZE]){
  
  if(strcmp(relName, RELCAT_RELNAME) == 0 || strcmp(relName, ATTRCAT_RELNAME) == 0)
    return E_NOTPERMITTED;

  int relId = OpenRelTable::getRelId(relName);
  if(relId == E_RELNOTOPEN)
    return E_RELNOTOPEN;

  RelCatEntry relCatEntry;
  RelCacheTable::getRelCatEntry(relId, &relCatEntry);
  if(relCatEntry.numAttrs != nAttrs)
    return E_NATTRMISMATCH;

  Attribute recordValues[nAttrs];

  for(int attrIndex=0; attrIndex < nAttrs; attrIndex++){
    AttrCatEntry attrCatEntry;
    AttrCacheTable::getAttrCatEntry(relId, attrIndex, &attrCatEntry);
    int attrType = attrCatEntry.attrType;
    if(attrType == NUMBER){
      if(isNumber(record[attrIndex])){
        recordValues[attrIndex].nVal = atof(record[attrIndex]);
      }
      else{
        return E_ATTRTYPEMISMATCH;
      }
    }
    else{
      strcpy((char *) &(recordValues[attrIndex].sVal), record[attrIndex]);
      //strcpy(recordValues[attrIndex].sVal, record[attrIndex]);
    }
  }

  int retVal = BlockAccess::insert(relId, recordValues);
  return retVal;

}

int Algebra::select(char srcRel[ATTR_SIZE], char targetRel[ATTR_SIZE], char attr[ATTR_SIZE], int op, char strVal[ATTR_SIZE]) {
  
  int srcRelId = OpenRelTable::getRelId(srcRel);      
  if (srcRelId == E_RELNOTOPEN) {
    return E_RELNOTOPEN;
  }

  AttrCatEntry attrCatEntry;
  int ret = AttrCacheTable::getAttrCatEntry(srcRelId, attr,&attrCatEntry);
  
  if(ret == E_ATTRNOTEXIST)
  	return E_ATTRNOTEXIST;

  int type = attrCatEntry.attrType;
  Attribute attrVal;
  if (type == NUMBER) {
    if (isNumber(strVal)) {       // the isNumber() function is implemented below
      attrVal.nVal = atof(strVal);
    } else {
      return E_ATTRTYPEMISMATCH;
    }
  } else if (type == STRING) {
    strcpy(attrVal.sVal, strVal);
  }

    RelCatEntry relCatEntry;
    RelCacheTable::getRelCatEntry(srcRelId,&relCatEntry);

    int src_nAttrs = relCatEntry.numAttrs;

    char attr_names[src_nAttrs][ATTR_SIZE];
    int attr_types[src_nAttrs];
    
    for (int attrIndex = 0; attrIndex < src_nAttrs; attrIndex++) {
        AttrCatEntry attrCatEntry;
        AttrCacheTable::getAttrCatEntry(srcRelId, attrIndex, &attrCatEntry);
        strcpy(attr_names[attrIndex], attrCatEntry.attrName);
        attr_types[attrIndex] = attrCatEntry.attrType;
    }

    int retVal = Schema::createRel(targetRel, src_nAttrs, attr_names, attr_types);
    if(retVal != SUCCESS)
    	return retVal;

    int openedRelId = OpenRelTable::openRel(targetRel);
    
    if(openedRelId < 0 || openedRelId >= MAX_OPEN){
    	Schema::deleteRel(targetRel);
    	return openedRelId;
    }
  
    Attribute record[src_nAttrs];

    RelCacheTable::resetSearchIndex(srcRelId);
    //AttrCacheTable::resetSearchIndex(srcRelId, attr);

    while (true) {
        RecId recId = BlockAccess::linearSearch(srcRelId, attr, attrVal, op);

        if(recId.block == -1 && recId.slot == -1){
          break;
        }
        RecBuffer recBuff(recId.block);
        recBuff.getRecord(record, recId.slot);
        ret = BlockAccess::insert(openedRelId, record);
        if(ret != SUCCESS){
          Schema::closeRel(targetRel);
          Schema::deleteRel(targetRel);
          return ret;
        }
    }

    Schema::closeRel(targetRel);
    return SUCCESS;
}

int Algebra::project(char srcRel[ATTR_SIZE], char targetRel[ATTR_SIZE]) {

    int srcRelId = OpenRelTable::getRelId(srcRel);

   if (srcRelId < 0 || srcRelId >= MAX_OPEN) 
    return E_RELNOTOPEN;


    RelCatEntry relCatEntry;
    RelCacheTable::getRelCatEntry(srcRelId, &relCatEntry);

    int numAttrs = relCatEntry.numAttrs;

    char attrNames[numAttrs][ATTR_SIZE];
    int attrTypes[numAttrs];

    for(int attrIndex = 0; attrIndex < numAttrs; attrIndex++){

      AttrCatEntry attrCatEntry;
      AttrCacheTable::getAttrCatEntry(srcRelId, attrIndex, &attrCatEntry);
      strcpy(attrNames[attrIndex], attrCatEntry.attrName);
      attrTypes[attrIndex] = attrCatEntry.attrType;

    } 

    int ret = Schema::createRel(targetRel, numAttrs, attrNames, attrTypes);

    if(ret != SUCCESS)
      return ret;
    
    int targetRelId = OpenRelTable::openRel(targetRel);

    if(targetRelId < 0 || targetRelId >= MAX_OPEN){
      Schema::deleteRel(targetRel);
      return targetRelId;
    }
    
    RelCacheTable::resetSearchIndex(srcRelId);

    Attribute record[numAttrs];

    while (true)
    {

      // check this part POTENTIAL ISSUE
        int retVal = BlockAccess::project(srcRelId, record);

        if(retVal != SUCCESS)
          break;

        ret = BlockAccess::insert(targetRelId, record);

        if (ret != SUCCESS) {
            Schema::closeRel(targetRel);
            Schema::deleteRel(targetRel);
            return ret;
        }
    }

    Schema::closeRel(targetRel);
    return SUCCESS;
}

int Algebra::project(char srcRel[ATTR_SIZE], char targetRel[ATTR_SIZE], int tar_nAttrs, char tar_Attrs[][ATTR_SIZE]) {

    int srcRelId = OpenRelTable::getRelId(srcRel);

    if (srcRelId < 0 || srcRelId >= MAX_OPEN) 
      return E_RELNOTOPEN;

    RelCatEntry relCatEntry;
    RelCacheTable::getRelCatEntry(srcRelId, &relCatEntry);

    // get the no. of attributes present in relation from the fetched RelCatEntry.
    int numAttrs = relCatEntry.numAttrs;
    int attr_offset[tar_nAttrs];

    int attr_types[tar_nAttrs];

    for(int tar_AttrIndex = 0; tar_AttrIndex < tar_nAttrs; tar_AttrIndex++){
      AttrCatEntry attrCatEntry;
      int ret = AttrCacheTable::getAttrCatEntry(srcRelId, tar_Attrs[tar_AttrIndex], &attrCatEntry);
      if(ret == E_ATTRNOTEXIST)
        return E_ATTRNOTEXIST;
      attr_offset[tar_AttrIndex] = attrCatEntry.offset;
      attr_types[tar_AttrIndex] = attrCatEntry.attrType;
    }

    int retVal = Schema::createRel(targetRel, tar_nAttrs, tar_Attrs, attr_types);

    if(retVal != SUCCESS)
      return retVal;

    int targetRelId = OpenRelTable::openRel(targetRel);

    if(targetRelId < 0 || targetRelId >= MAX_OPEN){
      Schema::deleteRel(targetRel);
      return targetRelId;
    }

    RelCacheTable::resetSearchIndex(srcRelId);

    Attribute record[numAttrs];
    Attribute proj_record[tar_nAttrs];

    while (true) {

        int ret = BlockAccess::project(srcRelId, record);

        if(ret != SUCCESS)
          break;
        Attribute proj_record[tar_nAttrs];

        for(int tar_AttrIndex = 0; tar_AttrIndex < tar_nAttrs; tar_AttrIndex++){
          proj_record[tar_AttrIndex] = record[attr_offset[tar_AttrIndex]];
        }

        ret = BlockAccess::insert(targetRelId, proj_record);

        if (ret != SUCCESS) {
            Schema::closeRel(targetRel);
            Schema::deleteRel(targetRel);
            return ret;
        }
    }

    Schema::closeRel(targetRel);
    return SUCCESS;
}