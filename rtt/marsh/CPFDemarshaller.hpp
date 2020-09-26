/***************************************************************************
  tag: Peter Soetens  Mon Jan 19 14:11:20 CET 2004  CPFDemarshaller.hpp

                        CPFDemarshaller.hpp -  description
                           -------------------
    begin                : Mon January 19 2004
    copyright            : (C) 2004 Peter Soetens
    email                : peter.soetens@mech.kuleuven.ac.be

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

#ifndef PI_PROPERTIES_CPFDESERIALIZER
#define PI_PROPERTIES_CPFDESERIALIZER

#include "rtt/marsh/rtt-marsh-config.h"

#ifdef OROPKG_SUPPORT_XERCES_C
#include <xercesc/util/XMLUniDefs.hpp>
#include <xercesc/sax/InputSource.hpp>

#include "rtt/marsh/MarshallInterface.hpp"

namespace RTT
{ namespace marsh {

#ifdef XERCES_CPP_NAMESPACE
    using XERCES_CPP_NAMESPACE::InputSource;
#endif

    /**
     * @brief A demarshaller for extracting properties and property bags
     * from a Component Property File (CPF) following the CORBA 3 standard.
     * @see CPFMarshaller to create CPF files.
     */
    class RTT_MARSH_API CPFDemarshaller
        : public DemarshallInterface
    {
#ifdef XERCES_CPP_NAMESPACE
        typedef XERCES_CPP_NAMESPACE::InputSource InputSource;
#endif
        XMLCh* name;
        InputSource* fis;
    public:
        CPFDemarshaller( const std::string& filename );
        ~CPFDemarshaller();
        virtual bool deserialize( PropertyBag &v );
    };
}}
#endif
#endif
