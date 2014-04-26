#include "catalog.h"
#include "query.h"
#include "stdio.h"
#include "stdlib.h"


// forward declaration
const Status ScanSelect(const string & result, 
			const int projCnt, 
			const AttrDesc projNames[],
			const AttrDesc *attrDesc, 
			const Operator op, 
			const char *filter,
			const int reclen);



/*
 * Selects records from the specified relation.
 *
 * Returns:
 * 	OK on success
 * 	an error code otherwise
 */

const Status QU_Select(const string & result, 
		       const int projCnt, 
		       const attrInfo projNames[],
		       const attrInfo *attr, 
		       const Operator op, 
		       const char *attrValue)
{
   // Qu_Select sets up things and then calls ScanSelect to do the actual work
    cout << "Doing QU_Select " << endl;

typedef struct {
	  char relName[MAXNAME];                // relation name
	  char attrName[MAXNAME];               // attribute name
	  int attrOffset;                       // attribute offset
	  int attrType;                         // attribute type
	  int attrLen;                          // attribute length
	} AttrDesc;

	int filValue, type;
	char *filter;
	attrDesc *attrDesc;
    AttrDesc attrDescArray[projCnt];
    Status status;

    if((status = attrCat->getInfo(attr.relName, attr.attrName, attrDesc)) != OK){
    	return status;
    }

    for (int i = 0; i < projCnt; i++)
    {
        status = attrCat->getInfo(projNames[i].relName,
                                         projNames[i].attrName,
                                         attrDescArray[i]);
        if (status != OK)
        {
            return status;
        }
    }

    // Get the length of a record
    reclen = 0;
    for (int i = 0; i < projCnt; i++)
    {
        reclen += attrDescArray[i].attrLen;
    }

    // Create filter based on the attribute type
    type = attr.attrType;
    switch(type){
    	case INTEGER:
    		filValue = atoi(attrValue);
    		memcpy(&filter, (char*)&filValue, sizeof(int));
    		break;
    	case FLOAT:
    		filValue = atof(attrValue);
    		memcpy(&filter, (char*)&filValue, sizeof(float));
    		break;
    	case STRING:
    		memcpy(&filter, &attrValue, attr.attrLen);
    		break;
    }

    return ScanSelect(result, projCnt, attrDescArray, attrDesc, op, filter, reclen);



}


const Status ScanSelect(const string & result, 
			const int projCnt, 
			const AttrDesc projNames[],
			const AttrDesc *attrDesc, 
			const Operator op, 
			const char *filter,
			const int reclen)
{

    cout << "Doing HeapFileScan Selection using ScanSelect()" << endl;

    // Start the Query with HeapFileScan
    Status status;
    RID rid;

    char outputData[reclen];
    Record outputRec;
    outputRec.data = (void *) outputData;
    outputRec.length = reclen;

    HeapFileScan *hfs = new HeapFileScan(attrDesc.relName, status);
    InsertFileScan *ifs = new InsertFileScan(result, status);

    if((status = hfs->startScan(attrDesc.attrOffset, attrDesc.attrLen, attrDesc.attrType, filter, op)) != OK){
    	return status;
    }

    while(hfs->scanNext(rid) == OK){
    	Record scanRec;
    	if((status = hfs->getRecord(scanRec)) != OK){
    		return status;
    	}

    	int outputOffset = 0;
    	for(int i = 0; i < projCnt; i++){
    		memcpy(outputRec + outputOffset,
                           (char *)scanRec.data + projNames[i].attrOffset,
                           projNames[i].attrLen);
    		outputOffset += projNames[i].attrLen;
    	}
    	RID outRID;
        if((status = ifs->insertRecord(outputRec, outRID)) != OK){
        	return status;
        }
        // Debug
        // resultTupCnt++;
    }

    return OK;
}
