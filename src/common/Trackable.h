#pragma once

#include <iostream>

/*
THIS IS DEAN LAWSON'S CODE FROM HIS LIBRARY "DEANLIB".
WE HAVE ACCESS TO THE AFOREMENTIONED LIBRARY IN HIS CLASSES
AND IT IS QUITE USEFUL SO WE THOUGHT WE'D USE THIS TO CHECK
FOR MEMORY LEAKS
*/

class Trackable
{
public:
	void* operator new( std::size_t size );
	void operator delete( void *ptr );	
	void* operator new[]( std::size_t size );
	void operator delete[]( void *ptr );
};
