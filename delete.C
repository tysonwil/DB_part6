#include "catalog.h"
#include "query.h"


/*
 * This function will delete all tuples in relation satisfying the predicate specified by attrName, 
 * op, and the constant attrValue. type denotes the type of the attribute. You can locate all the 
 * qualifying tuples using a filtered HeapFileScan.
 */

/*
 * Deletes records from a specified relation.
 *
 * Returns:
 * 	OK on success
 * 	an error code otherwise
 */

const Status QU_Delete(const string & relation, 
		       const string & attrName, 
		       const Operator op,
		       const Datatype type, 
		       const char *attrValue)
{
	RID rid;
	Status status;
	AttrDesc attrInfo;
	Record record;

	// Set up a HeapFileScan
	HeapFileScan hfs(relation, status);
	if (status != OK) return status;

	// Get attribute information
	status = attrCat->getInfo(relation, attrName, attrInfo);
	if (status != OK) return status;

	// Start the scan with the attribute info gained above.
	status = hfs.startScan(attrInfo, attrInfo.attLen, type, attrValue, op);
	if (status != OK) return status;

	// Find all tuples in relation
	while ((status = hfs.scanNext(rid)) == OK) {
		// get record
		status = hfs.getRecord(record);
		if (status != OK) return status;

		// delete record
		status = hfs.deleteRecord();
		if (status != OK) return status;
	}
	
	return OK;
}


