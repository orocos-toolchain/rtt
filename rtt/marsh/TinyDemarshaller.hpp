/***************************************************************************
  tag: Peter Soetens  do nov 2 13:05:59 CET 2006  TinyDemarshaller.hpp

                        TinyDemarshaller.hpp -  description
                           -------------------
    begin                : do november 02 2006
    copyright            : (C) 2006 Peter Soetens
    email                : peter.soetens@gmail.com

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


#ifndef PROPERTIES_TINY_DEMARSHALLER
#define PROPERTIES_TINY_DEMARSHALLER

#include "rtt/marsh/MarshallInterface.hpp"
#include <string>

namespace RTT
{ namespace marsh {

    /**
     * @brief A TinyXML demarshaller for extracting properties and property bags
     * from a Component Property File (CPF) following the CORBA 3 standard.
     * @see CPFMarshaller to create CPF files.
     */
    class RTT_MARSH_API TinyDemarshaller
        : public DemarshallInterface
    {
        class D;
        D* d;
    public:
        TinyDemarshaller( const std::string& filename );
        ~TinyDemarshaller();
        virtual bool deserialize( PropertyBag &v );
    };
}}
#endif
