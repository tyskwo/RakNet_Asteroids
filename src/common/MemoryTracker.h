#pragma once

#include <map>
#include <iostream>

/*
THIS IS DEAN LAWSON'S CODE FROM HIS LIBRARY "DEANLIB".
WE HAVE ACCESS TO THE AFOREMENTIONED LIBRARY IN HIS CLASSES
AND IT IS QUITE USEFUL SO WE THOUGHT WE'D USE THIS TO CHECK
FOR MEMORY LEAKS
*/

struct AllocationRecord
{
	AllocationRecord( int theNum, size_t theSize ): num(theNum), size(theSize){};
	int num;
	size_t size;
};

class MemoryTracker
{
public:
	MemoryTracker();
	~MemoryTracker();

	void addAllocation( void* ptr, size_t size );
	void removeAllocation( void* ptr );
	
	void reportAllocations( std::ostream& stream );

private:
	//copying not allowed
	MemoryTracker( const MemoryTracker& );
	MemoryTracker& operator=( const MemoryTracker& );

	std::map<void*,AllocationRecord> mAllocations;

	static int msAllocationNum;
};

extern MemoryTracker gMemoryTracker;