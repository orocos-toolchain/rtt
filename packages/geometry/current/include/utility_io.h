/***************************************************************************
  tag: Erwin Aertbelien  Mon Jan 10 16:38:38 CET 2005  utility_io.h 

                        utility_io.h -  description
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
 *		$Id: utility_io.h,v 1.1.1.1.2.3 2003/06/26 15:23:59 psoetens Exp $
 *		$Name:  $ 
 *
 *  \file   utility_io.h
 *     Included by most lrl-files to provide some general
 *     functions and macro definitions related to file/stream I/O.
 */

#ifndef UTILITY_IO_H_84822
#define UTILITY_IO_H_84822

#include "utility.h"

#define WANT_STD_IOSTREAM

// Standard includes
#include <iostream>
#include <iomanip>
#include <fstream>


#ifdef USE_NAMESPACE
namespace ORO_Geometry {
#endif

#ifdef WANT_STD_IOSTREAM
	using std::ostream;
	using std::istream;
	using std::endl;
	using std::setw;
	using std::cout;
	using std::cin;
	using std::cerr;
	using std::fstream;
	using std::ifstream;
	using std::ofstream;
	using std::ios;
#endif

	/**
	 * checks validity of basic io of is
	 */
	void _check_istream(std::istream& is);


/** 
 * Eats characters of the stream until the character delim is encountered
 * @param is a stream
 * @param delim eat until this character is encountered
 */
void Eat(std::istream& is, int delim );

/** 
 * Eats characters of the stream as long as they satisfy the description in descript
 * @param is a stream
 * @param descript description string. A sequence of spaces, tabs, 
 *           new-lines and comments is regarded as 1 space in the description string.
 */
void Eat(std::istream& is,const char* descript);

/**
 * Eats a word of the stream delimited by the letters in delim or space(tabs...)
 * @param is a stream
 * @param delim a string containing the delimmiting characters
 * @param storage for returning the word
 * @param maxsize a word can be maximally maxsize-1 long.
 */
void EatWord(std::istream& is,const char* delim,char* storage,int maxsize);

/** 
 * Eats characters of the stream until the character delim is encountered
 * similar to Eat(is,delim) but spaces at the end are not read.
 * @param is a stream
 * @param delim eat until this character is encountered
 */
void EatEnd( std::istream& is, int delim );




#ifdef USE_NAMESPACE
}
#endif


#endif
