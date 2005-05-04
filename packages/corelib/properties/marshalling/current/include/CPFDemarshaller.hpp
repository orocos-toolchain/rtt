/***************************************************************************
  tag: Peter Soetens  Mon Jan 19 14:11:20 CET 2004  CPFDemarshaller.hpp 

                        CPFDemarshaller.hpp -  description
                           -------------------
    begin                : Mon January 19 2004
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
 
#ifndef PI_PROPERTIES_CPFDESERIALIZER
#define PI_PROPERTIES_CPFDESERIALIZER

#include <xercesc/util/XMLUniDefs.hpp>
#include <xercesc/sax/InputSource.hpp>

#include "corelib/Marshaller.hpp"

namespace ORO_CoreLib
{
#ifdef XERCES_CPP_NAMESPACE
    using XERCES_CPP_NAMESPACE::InputSource;
#endif

    /**
     * @brief A demarshaller for extracting properties and property bags
     * from a Component Property File following the CORBA 3 standard.
     */
    class CPFDemarshaller
        : public Demarshaller
    {
#ifdef XERCES_CPP_NAMESPACE
        typedef XERCES_CPP_NAMESPACE::InputSource InputSource;
#endif
        typedef unsigned short XMLCh;
        XMLCh* name;
        InputSource* fis;

        public:
        CPFDemarshaller( const std::string& filename );
        ~CPFDemarshaller();
        virtual bool deserialize( PropertyBag &v );
    };
}
#endif
