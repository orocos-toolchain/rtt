/***************************************************************************
  tag: Erwin Aertbelien  Mon Jan 10 16:38:38 CET 2005  debug_macros.h 

                        debug_macros.h -  description
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
 * \file  
 *      defines debug macros that make it easy to write test-sequences
 *      Defined are :
 *          - function logger() that returns a ofstream to the logger-stream.  
 *          this stream is created the first time the function is called.
 *          - D(x,y) tests whether x is equal to y, Equal(..,..) should be defined
 *          on x and y.
 *          - D_eps(x,y,eps) same as previous but with Equal(..,..,eps) (in eps-interval)
 *          - L(x), converts x to a string and write to the logger, and also executes x
 *          - DRESULT(x) displays x and the evaluation of x
 *          - N(x)  calls subroutine x and logs this call
 *          - DEBUG_RETURNVALUE can be used to check whether errors occurred.
 *  \author 
 *      Erwin Aertbelien, Div. PMA, Dep. of Mech. Eng., K.U.Leuven
 *
 *  \version 
 *      ORO_Geometry V0.2
 *
 *  \par History
 *      - $log$ 
 *
 *  \par Release
 *      $Id: debug_macros.h,v 1.1.1.1.2.1 2003/07/18 14:58:36 psoetens Exp $
 *      $Name:  $ 
 ****************************************************************************/
#ifndef DEBUG_MACROS
#define DEBUG_MACROS

#include "utility.h"

#ifdef WANT_STD_IOSTREAM
    #include <iostream>
    #include <fstream>
    std::ofstream& logger();
    std::ofstream& errorlogger();
#else
    #include <iostream.h>
    #include <fstream.h>
    ofstream& logger();
    ofstream& errorlogger();
#endif

extern int DEBUG_RETURNVALUE;

#define D(x,y) \
    {logger() << "Line: "<<__LINE__ << " File:"<<__FILE__<<" ("<< #x  << ") Equal to (" << #y << ") ? " << endl\
          << "    X : " << (x) << endl\
          << "    Y : " << (y) << endl;\
    if (!Equal(x,y)) \
    {cout << "Line: "<<__LINE__ << " File:"<<__FILE__<< " ("<< #x  << ") Equal to (" << #y << ") ? " << endl\
          << "    X : " << (x) << endl\
          << "    Y : " << (y) << endl;\
     logger() << "ERROR : ==== VALUES SHOULD BE EQUAL ====" << endl;\
     errorlogger() << "Line: "<<__LINE__ << " File:"<<__FILE__<< " ("<< #x  << ") Equal to (" << #y << ") ? " << endl\
          << "    X : " << (x) << endl\
          << "    Y : " << (y) << endl;\
     errorlogger() << "ERROR : ==== VALUES SHOULD BE EQUAL ====" << endl;\
     DEBUG_RETURNVALUE = -1; \
    }}

#define D_eps(x,y,eps) \
    {logger() << "Line: "<<__LINE__ << " File:"<<__FILE__<< " ("<< #x  << ") Equal to (" << #y << ") within "<< eps << " range ? " << endl\
          << "    X : " << (x) << endl\
          << "    Y : " << (y) << endl;\
    if (!Equal(x,y,eps)) \
    {cout << "Line: "<<__LINE__ << " File:"<<__FILE__<< " ("<< #x  << ") Equal to (" << #y << ") within " << eps << " range ? " << endl\
          << "    X : " << (x) << endl\
          << "    Y : " << (y) << endl;\
     logger() << "ERROR : ==== VALUES SHOULD BE EQUAL ====" << endl;\
     errorlogger() << "Line: "<<__LINE__ << " File:"<<__FILE__<< " ("<< #x  << ") Equal to (" << #y << ") within " << eps << " range ? " << endl\
          << "    X : " << (x) << endl\
          << "    Y : " << (y) << endl;\
     errorlogger() << "ERROR : ==== VALUES SHOULD BE EQUAL ====" << endl;\
     DEBUG_RETURNVALUE = -1; \
    }}

#define L(x) logger() << "Line: "<<__LINE__ << "File:"<<__FILE__<< " " << #x << endl;x;
#define DRESULT(x) logger() << "Line: "<<__LINE__ << " File:"<<__FILE__<< " " << #x << endl;logger() << x << endl;
#define N(x) logger() << "--------------------------------------------------------------------" << endl\
                      << "Line: "<<__LINE__<< " File:"<<__FILE__<< "  ROUTINE " << #x << endl\
                      << "--------------------------------------------------------------------" << endl;x;
                      
#endif
