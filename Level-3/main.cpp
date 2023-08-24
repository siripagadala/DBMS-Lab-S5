#include <iostream>
#include "Buffer/StaticBuffer.h"
#include "Cache/OpenRelTable.h"
#include "Disk_Class/Disk.h"
#include <cstring>
#include "FrontendInterface/FrontendInterface.h"
using namespace std;

int main(int argc, char *argv[]) {
  Disk disk_run;
  StaticBuffer buffer;
  OpenRelTable cache;

  /*
  for i = 0 and i = 1 (i.e RELCAT_RELID and ATTRCAT_RELID)

      get the relation catalog entry using RelCacheTable::getRelCatEntry()
      printf("Relation: %s\n", relname);

      for j = 0 to numAttrs of the relation - 1
          get the attribute catalog entry for (rel-id i, attribute offset j)
           in attrCatEntry using AttrCacheTable::getAttrCatEntry()

          printf("  %s: %s\n", attrName, attrType);
  */

  for (int i = 0; i <= 2; i++) {

      RelCatEntry relCatEntry;
      int getRelResult = RelCacheTable::getRelCatEntry(i, &relCatEntry);
      if (getRelResult == SUCCESS) {
        printf("Relation: %s\n", relCatEntry.relName);
        for (int j = 0; j < relCatEntry.numAttrs; j++) {
          AttrCatEntry attrCatEntry;
          int getAttrResult = AttrCacheTable::getAttrCatEntry(i, j, &attrCatEntry);
          if(getAttrResult == SUCCESS){
            const char* attrType = attrCatEntry.attrType== NUMBER ? "NUM" : "STR";
            printf(" %s: %s\n", attrCatEntry.attrName, attrType);
          }
        }
      }
    printf("\n");
  }
  return 0;
}