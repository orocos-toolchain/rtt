/***************************************************************************
  tag: Peter Soetens  Tue Dec 21 22:43:08 CET 2004  FakeComponent.hpp 

                        FakeComponent.hpp -  description
                           -------------------
    begin                : Tue December 21 2004
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
 
 
#include "TemplateCommandFactory.hpp"
#include "TemplateDataSourceFactory.hpp"

#include <iostream>

using namespace ORO_Execution;

class FakeComponent
{
public:
  CommandFactoryInterface* createCommandFactory() {
    TemplateCommandFactory<FakeComponent>* ret = newCommandFactory( this );
    ret->add( "printString",
              command( &FakeComponent::printString,
                       &FakeComponent::returnTrue,
                       "print the specified string", "s", "string to print" ) );
    ret->add( "printInt",
              command( &FakeComponent::printInt,
                       &FakeComponent::returnTrue,
                       "print the specified int", "a", "int to print" ) );
    ret->add( "printDouble",
              command( &FakeComponent::printDouble,
                       &FakeComponent::returnTrue,
                       "print the specified double", "d", "double to print" ) );
    return ret;
  }

  DataSourceFactoryInterface* createDataSourceFactory() {
    TemplateDataSourceFactory<FakeComponent>* ret =
      newDataSourceFactory( this );
    ret->add( "someString",
              data( &FakeComponent::someString, "Returns some string." ) );
    ret->add( "length",
              data( &FakeComponent::length,
                    "Returns the length of a given string",
                    "s", "the string" ) );
    return ret;
  }

  std::string someString() const {
    return "appel";
  }

  int length( std::string s ) const {
    return s.length();
  }

  bool returnTrue() const {
    return true;
  }

  bool printString( std::string s ) {
    std::cout << "printString: " << s << std::endl;
    return true;
  }

  bool printDouble( double d ) {
    std::cout << "printDouble: " << d << std::endl;
  }

  bool printInt( int a ) {
    std::cout << "printInt: " << a << std::endl;
    return true;
  }
};
