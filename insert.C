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
	int attrLen = 0;
	int recordLength = 0;
	int type;
	Record record;

	// Need to instantiate InsertFileScan
	InsertFileScan ifs(relation, status);
	if (status != OK) return status;

	// Get relation information
	status = relCat->getInfo(relation, relinfo);
	if (status != OK) return status;

	// Get attribute information
	status = attrCat->getRelinfo(relation, attrLen, attrInfo);
	if (status != OK) return status;

	// Determine what attributes will be inserted, and then
	// sum total size of these attributes in order to determine
	// total space of this new record
	for (int i=0; i < attrCnt, i++) {
		for (int j=0; j < attrLen; j++) {
			if (strcmp(attrList[i].attrName, attrInfo[j].attrName) == 0)
				recordLength += attrList[i].attrLen;
		}
	}

	// Set record length
	record.length = recordLength;

	// Determine attribute value and insert into record
	// Since attrValue is a string, it needs to be converted
	for (int i=0; i < attrCnt, i++) {
		for (int j=0; j < attrLen; j++) {
			if (strcmp(attrList[i].attrName, attrInfo[j].attrName) == 0) {
				type = attrInfo[j].attrType;
				if (type == INTEGER) {
					int value = atoi(attrList[i].attrValue);
					// TODO: copy into memory
				}
				else if (type == FLOAT) {
					float value = atof(attrList[i].attrValue);
					// TODO: copy into memory
				}
				else {
					// TODO: already a string, copy into memory
				}
			}
		}
	}

	// Finally insert the record into the relation
	return ifs.insertRecord(record, rid);

	return status;
}

