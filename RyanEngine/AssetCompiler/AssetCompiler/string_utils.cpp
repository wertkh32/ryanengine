#include "string_utils.h"
#include <string.h>

const char* GetFileNameFromPath ( const char* path )
{
	const char *name = strrchr ( path, '\\' );

	if ( name )
		name++;
	else
		name = path;

	return name;
}