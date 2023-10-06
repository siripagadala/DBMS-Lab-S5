#include "Algebra.h"

#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <iostream>

using namespace std;


bool isNumber(char *str) {
  int len;
  float ignore;
  int ret = sscanf(str, "%f %n", &ignore, &len);
  return ret == 1 && len == strlen(str);
}


int Algebra::select(char srcRel[ATTR_SIZE], char targetRel[ATTR_SIZE], char attr[ATTR_SIZE], int op, char strVal[ATTR_SIZE]) {
  int srcRelId = OpenRelTable::getRelId(srcRel);
  printf("srcRelId = %d", srcRelId);
  if (srcRelId == E_RELNOTOPEN) {
    return E_RELNOTOPEN;
  }

  AttrCatEntry attrCatEntry;
  int ret = AttrCacheTable::getAttrCatEntry(srcRelId, attr, &attrCatEntry);
  printf("ret = %d", ret);
  if (ret != SUCCESS) {
    return E_ATTRNOTEXIST;
  }

  int type = attrCatEntry.attrType;
  Attribute attrVal;
  if (type == NUMBER) {
    if (isNumber(strVal)) {
      attrVal.nVal = atof(strVal);
    } else {
      return E_ATTRTYPEMISMATCH;
    }
  } else if (type == STRING) {
    strcpy(attrVal.sVal, strVal);
  }

  // Before calling the search function, reset the search to start from the first hit
  RelCacheTable::resetSearchIndex(srcRelId);

  RelCatEntry relCatEntry;
  RelCacheTable::getRelCatEntry(srcRelId, &relCatEntry);

  printf("|");
  for (int i = 0; i < relCatEntry.numAttrs; ++i) {
    AttrCatEntry attrCatEntry;
    AttrCacheTable::getAttrCatEntry(srcRelId, i, &attrCatEntry);

    printf(" %s |", attrCatEntry.attrName); 
  }
  //printf("hello everynya \n"); //no error till here

  //printf("Entering the loop...\n");
  printf("\n");  
  while (true) {
    //printf("Inside the loop iteration...\n");
    RecId searchRes = BlockAccess::linearSearch(srcRelId, attr, attrVal, op);
    //printf("searchRes.block: %d, searchRes.slot: %d\n", searchRes.block, searchRes.slot);

    if (searchRes.block != -1 && searchRes.slot != -1) {
      RecBuffer yajat(searchRes.block);

      struct HeadInfo head;
      yajat.getHeader(&head);
      Attribute record[head.numAttrs];
      yajat.getRecord(record, searchRes.slot);  //CHECK THIS NIGA 

      printf("|");
      for (int i = 0; i < head.numAttrs; ++i) {
        AttrCatEntry attrbu;
        AttrCacheTable::getAttrCatEntry(srcRelId,i,&attrbu);
        if (attrbu.attrType == NUMBER) {
          printf("%0.f |", record[i].nVal);
        } else {
          printf("%s |", record[i].sVal);
        }
      }
      printf("\n");

    } else {
      break;
    }
  }

  return SUCCESS;
}
