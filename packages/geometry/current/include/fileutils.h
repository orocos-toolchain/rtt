/***************************************************************************
  tag: Erwin Aertbelien  Mon Jan 10 16:38:38 CET 2005  fileutils.h 

                        fileutils.h -  description
                           -------------------
    begin                : Mon January 10 2005
    copyright            : (C) 2005 Erwin Aertbelien
    email                : erwin.aertbelien@mech.kuleuven.ac.be
 
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
