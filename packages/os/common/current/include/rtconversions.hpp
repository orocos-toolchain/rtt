/***************************************************************************
  tag: Peter Soetens  Thu Oct 10 16:16:59 CEST 2002  rtconversions.hpp 

                        rtconversions.hpp -  description
                           -------------------
    begin                : Thu October 10 2002
    copyright            : (C) 2002 Peter Soetens
    email                : peter.soetens@mech.kuleuven.ac.be
 
 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
 
 
#include <string>

int string_to_int(const std::string& s );
// float string_to_float(const std::string& s);
char string_to_char( const std::string& s );
unsigned int string_to_unsigned_int( const std::string& s );
std::string int_to_string( int i );
std::string float_to_string(float f);
std::string unsigned_int_to_string( unsigned int u );
