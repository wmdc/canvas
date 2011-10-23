#pragma once

#include <string>
#include <vector>
#include <iostream>

using namespace std;
/*
   Windows-specific IO utility functions.
*/
class WinIO {
public:
	//Get all the files of the given file type within the given directory.
	//Queries are directory paths plus wildcards for filenames, e.g.
	//"C://ABC/*.txt"
	static vector<string> WinIO::getDirectoryFilenames( const string dirStr, const string qStr );
};