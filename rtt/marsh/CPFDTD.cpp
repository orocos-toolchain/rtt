/***************************************************************************
  tag: FMTC  Tue Mar 11 21:49:20 CET 2008  CPFDTD.cpp

                        CPFDTD.cpp -  description
                           -------------------
    begin                : Tue March 11 2008
    copyright            : (C) 2008 FMTC
    email                : peter.soetens@fmtc.be

 ***************************************************************************
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU General Public                   *
 *   License as published by the Free Software Foundation;                 *
 *   version 2 of the License.                                             *
 *                                                                         *
 *   As a special exception, you may use this file as part of a free       *
 *   software library without restriction.  Specifically, if other files   *
 *   instantiate templates or use macros or inline functions from this     *
 *   file, or you compile this file and link it with other files to        *
 *   produce an executable, this file does not by itself cause the         *
 *   resulting executable to be covered by the GNU General Public          *
 *   License.  This exception does not however invalidate any other        *
 *   reasons why the executable file might be covered by the GNU General   *
 *   Public License.                                                       *
 *                                                                         *
 *   This library is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU     *
 *   Lesser General Public License for more details.                       *
 *                                                                         *
 *   You should have received a copy of the GNU General Public             *
 *   License along with this library; if not, write to the Free Software   *
 *   Foundation, Inc., 59 Temple Place,                                    *
 *   Suite 330, Boston, MA  02111-1307  USA                                *
 *                                                                         *
 ***************************************************************************/



#include "CPFDTD.hpp"

namespace RTT {
    namespace marsh
    {
        const char* cpf_dtd =
        "<!ELEMENT properties (description? , (simple|sequence|struct)* )> \
<!ELEMENT choice (#PCDATA)> \
<!ELEMENT choices (choice+)> \
<!ELEMENT defaultvalue (#PCDATA)> \
<!ELEMENT description (#PCDATA)> \
<!ELEMENT simple (description?, value, choices?, defaultvalue?)> \
<!ATTLIST simple name CDATA #IMPLIED type (boolean|char|double|float|short|long|llong|objref|octet|string|ushort|ulong|ullong) #REQUIRED> \
<!ELEMENT sequence (description?, (simple*|struct*|sequence*))> \
<!ATTLIST sequence name CDATA #IMPLIED type CDATA #REQUIRED> \
<!ELEMENT struct (description?, (simple|sequence|struct)*)> \
<!ATTLIST struct name CDATA #IMPLIED type CDATA #REQUIRED> \
<!ELEMENT value (#PCDATA)>";

    }
}
