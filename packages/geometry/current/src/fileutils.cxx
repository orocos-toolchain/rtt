/***************************************************************************
  tag: Peter Soetens  Mon May 10 19:10:36 CEST 2004  fileutils.cxx 

                        fileutils.cxx -  description
                           -------------------
    begin                : Mon May 10 2004
    copyright            : (C) 2004 Peter Soetens
    email                : peter.soetens@mech.kuleuven.ac.be
 
 ***************************************************************************
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU Lesser General Public            *
 *   License as published by the Free Software Foundation; either          *
 *   version 2.1 of the License, or (at your option) any later version.    *
 *                                                                         *
 *   This library is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU     *
 *   Lesser General Public License for more details.                       *
 *                                                                         *
 *   You should have received a copy of the GNU Lesser General Public      *
 *   License along with this library; if not, write to the Free Software   *
 *   Foundation, Inc., 59 Temple Place,                                    *
 *   Suite 330, Boston, MA  02111-1307  USA                                *
 *                                                                         *
 ***************************************************************************/
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
