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
