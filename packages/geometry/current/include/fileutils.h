#ifndef FILEUTILS_H
#define FILEUTILS_H


#include "utility.h"
#include "utility_io.h"
#include <string>
//#include <windows.h>

#ifdef USE_NAMESPACE
namespace ORO_Geometry {
#endif


/**
 * Windows function for analysing a path, can handle unix-style paths
 *
void AnalyseFilename(const std::string& filename,std::string& path,std::string& file);
*/
/**
 * Move the current directory to a certain path :
 *
void SetCurrentDirectory(const std::string& path);
*/
/**
 * Check whether file is good to read or write
 */
void Check(const ios& is);

#ifdef USE_NAMESPACE
}
#endif



#endif
