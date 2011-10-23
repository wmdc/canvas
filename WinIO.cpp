#include "precompiled.h"

#include <windows.h>
#include "WinIO.h"

/*
   Takes a directory path, dirStr, and finds all files matching the query
   string qStr.  This functionality requires Windows-specific calls.

   The function returns strings with full filepaths and names that can be
   used to open and process the files with the standard stdio methods.
*/
vector<string> WinIO::getDirectoryFilenames( string dirStr, string qStr ) {
	WIN32_FIND_DATA findData;
    HANDLE hFile;
    string fName;

	vector<string> fList = vector<string>();

    string query = dirStr + string("/" ) + qStr;

    hFile = FindFirstFile( query.c_str(), &findData );
    
	fName = string( dirStr + string( "/" ) + string( findData.cFileName ) );

    fList.push_back( fName );

	while( FindNextFile( hFile, &findData ) ) {
        fName = string( dirStr + string( "/" ) + string( findData.cFileName ) );
		fList.push_back( fName );
	}
	FindClose( hFile );

	return fList;
}