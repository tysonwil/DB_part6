#include "catalog.h"
#include "query.h"


/*
 * Inserts a record into the specified relation.

 * Insert a tuple with the given attribute values (in attrList) in relation. 
 * The value of the attribute is supplied in the attrValue member of the 
 * attrInfo structure. Since the order of the attributes in attrList[] may not 
 * be the same as in the relation, you might have to rearrange them before insertion. 
 * If no value is specified for an attribute, you should reject the insertion as Minirel 
 * does not implement NULLs.
 *
 * Returns:
 * 	OK on success
 * 	an error code otherwise
 */

const Status QU_Insert(const string & relation, const int attrCnt, const attrInfo attrList[])
{
	Status status = OK;
	int recordLength = 0;
	int type;
	Record record;
	RID rid;

	int attrRelCnt;

	// Need to instantiate InsertFileScan
	InsertFileScan ifs(relation, status);
	if (status != OK) return status;

	status = relCat->getInfo(relation, attrRelCnt);
	if (status != OK) return status;
	AttrDesc attrRel[attrRelCnt];

	// Get attribute information
	status = attrCat->getRelInfo(relation, attrRelCnt, attrRel);
	if (status != OK) return status;

	if(attrRelCnt != attrCnt){
		// Reject This, some attributes would be null
		return ATTRTYPEMISMATCH;
	}

	// Determine what attributes will be inserted, and then
	// sum total size of these attributes in order to determine
	// total space of this new record
	for (int i=0; i < attrCnt; i++) {
		recordLength += attrDescArray[i].attrLen;
	}

	char insertData[recordLength];
    record.data = (void *) insertData;
    record.length = recordLength;

	// Determine attribute value and insert into record
	// Since attrValue is a string, it needs to be converted
	for (int i=0; i < attrCnt; i++) {
		for (int j=0; j < attrRelCnt; j++) {
			if (strcmp(attrList[i].attrName, attrRel[j].attrName) == 0) {
				type = attrList[i].attrType;
				switch(type){
					case INTEGER:
						int valuei = atoi((char*)attrList[i].attrValue);
						memcpy(insertData + attrRel[j].attrOffset, &valuei, attrList[i].attrLen);
						break;
					case FLOAT:
						float valuef = atof((char*)attrList[i].attrValue);
						memcpy(insertData + attrRel[j].attrOffset, &valuef, attrList[i].attrLen);
						break;
					case STRING:
						memcpy(insertData + attrRel[j].attrOffset, attrList[i].attrValue, attrList[i].attrLen);
						break;
				}
				break;
			}
		}
	}

	// Finally insert the record into the relation
	return ifs.insertRecord(record, rid);

	return status;
}

