/*****************************************************************************
 *  \author 
 *  	Erwin Aertbelien, Div. PMA, Dep. of Mech. Eng., K.U.Leuven
 *
 *  \version 
 *		ORO_Geometry V0.2
 *
 *	\par History
 *		- $log$
 *
 *	\par Release
 *		$Id: fileutils.cpp,v 1.1.1.1.2.2 2003/02/24 13:13:06 psoetens Exp $
 *		$Name:  $ 
 ****************************************************************************/


#include "geometry/error.h"
#include "geometry/fileutils.h"

#ifdef USE_NAMESPACE
namespace ORO_Geometry {
#endif


// SUPPORT FOR CYGWIN/LINUX to be added, anybody knows the library calls ?
/**
 * Windows function for analysing a path, can handle unix-style paths
 *
void AnalyseFilename(const std::string& filename,std::string& path,std::string& file) {
	char buf[512];
	char* filepart;
	GetFullPathName(filename.c_str(),512,buf,&filepart);
	file = filepart;
	*filepart = 0;
	path = buf;
}
*/
/**
 * Move the current directory to a certain path :
 *
void SetCurrentDirectory(const std::string& path) {
	::SetCurrentDirectory(path.c_str());
	// explicit scope should be given, because of overloading rules within a namespace
}
*/

void Check(const ios& is) {
	if (!is.good()) 
        {
#ifdef HAVE_RTSTL
                Error_BasicIO_Not_Opened err;
                cout << err.Description();
                abort();
#else
                throw Error_BasicIO_Not_Opened();
#endif
        }
}




#ifdef USE_NAMESPACE
}
#endif
