/***************************************************************************
  tag: FMTC  Tue Mar 11 21:49:20 CET 2008  TinyDemarshaller.cpp

                        TinyDemarshaller.cpp -  description
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


#include "TinyDemarshaller.hpp"


// Modified tinyxml* to include it in the RTT namespace to avoid clashes
// with possible other libraries.
#include "tinyxml.h"

// This is currently not defined:
#ifdef TIXML_USE_STL
#include <iostream>
#include <sstream>
using namespace std;
#else
#include <cstdio>
#endif

#include <stack>
#include <Property.hpp>
#include <PropertyBag.hpp>
#include <Logger.hpp>

namespace RTT
{
    namespace marsh
    {
        class Tiny2CPFHandler
        {
            /**
             * Stores the results of the parsing.
             */
            PropertyBag &bag;
            std::stack< std::pair<PropertyBag*, Property<PropertyBag>*> > bag_stack;

            enum Tag { TAG_STRUCT, TAG_SIMPLE, TAG_SEQUENCE, TAG_PROPERTIES, TAG_DESCRIPTION, TAG_VALUE, TAG_UNKNOWN};
            std::stack<Tag> tag_stack;

            /**
             * The name of the property.
             */
            std::string name;
            std::string description;
            std::string type;
            std::string value_string;

        public:

            Tiny2CPFHandler( PropertyBag &b ) : bag( b )
            {
                Property<PropertyBag>* dummy = 0;
                bag_stack.push(std::make_pair(&bag, dummy));
            }

            bool endElement()
            {
                switch ( tag_stack.top() )
                    {
                    case TAG_SIMPLE:
                        if ( type == "boolean" )
                        {
                            if ( value_string == "1" || value_string == "true")
                                bag_stack.top().first->add
                                ( new Property<bool>( name, description, true ) );
                            else if ( value_string == "0" || value_string == "false")
                                bag_stack.top().first->add
                                ( new Property<bool>( name, description, false ) );
                            else {
                                log(Error)<< "Wrong value for property '"+type+"'." \
                                    " Value should contain '0' or '1', got '"+ value_string +"'." << endlog();
                                return false;
                            }
                        }
                        else if ( type == "char" ) {
                            if ( value_string.length() > 1 ) {
                                log(Error) << "Wrong value for property '"+type+"'." \
                                    " Value should contain a single character, got '"+ value_string +"'." << endlog();
                                return false;
                            }
                            else
                                bag_stack.top().first->add
                                    ( new Property<char>( name, description, value_string.empty() ? '\0' : value_string[0] ) );
                        }
                        else if ( type == "uchar" || type == "octet" ) {
                            if ( value_string.length() > 1 ) {
                                log(Error) << "Wrong value for property '"+type+"'." \
                                    " Value should contain a single unsigned character, got '"+ value_string +"'." << endlog();
                                return false;
                            }
                            else
                                bag_stack.top().first->add
                                    ( new Property<unsigned char>( name, description, value_string.empty() ? '\0' : value_string[0] ) );
                        }
                        else if ( type == "long" || type == "short")
                        {
                            if (type == "short") {
                                log(Warning) << "Use type='long' instead of type='short' for Property '"<< name << "', since 16bit integers are not supported." <<endlog();
                                log(Warning) << "Future versions of RTT will no longer map XML 'short' to C++ 'int' but to C++ 'short' Property objects." <<endlog();
                            }
                            int v;
                            if ( sscanf(value_string.c_str(), "%d", &v) == 1)
                                bag_stack.top().first->add( new Property<int>( name, description, v ) );
                            else {
                                log(Error) << "Wrong value for property '"+type+"'." \
                                    " Value should contain an integer value, got '"+ value_string +"'." << endlog();
                                return false;
                            }
                        }
                        else if ( type == "ulong" || type == "ushort")
                        {
                            if (type == "ushort") {
                                log(Warning) << "Use type='ulong' instead of type='ushort' for Property '"<< name << "', since 16bit integers are not supported." <<endlog();
                                log(Warning) << "Future versions of RTT will no longer map XML 'ushort' to C++ 'unsigned int' but to C++ 'unsigned short' Property objects." <<endlog();
                            }
                            unsigned int v;
                            if ( sscanf(value_string.c_str(), "%u", &v) == 1)
                                bag_stack.top().first->add( new Property<unsigned int>( name, description, v ) );
                            else {
                                log(Error) << "Wrong value for property '"+type+"'." \
                                    " Value should contain an integer value, got '"+ value_string +"'." << endlog();
                                return false;
                            }
                        }
                        else if ( type == "llong")
                        {
                            long long v;
                            if ( sscanf(value_string.c_str(), "%lld", &v) == 1)
                                bag_stack.top().first->add( new Property<long long>( name, description, v ) );
                            else {
                                log(Error) << "Wrong value for property '"+type+"'." \
                                    " Value should contain an integer value, got '"+ value_string +"'." << endlog();
                                return false;
                            }
                        }
                        else if ( type == "ullong")
                        {
                            unsigned long long v;
                            if ( sscanf(value_string.c_str(), "%llu", &v) == 1)
                                bag_stack.top().first->add( new Property<unsigned long long>( name, description, v ) );
                            else {
                                log(Error) << "Wrong value for property '"+type+"'." \
                                    " Value should contain an integer value, got '"+ value_string +"'." << endlog();
                                return false;
                            }
                        }
                        else if ( type == "double")
                        {
                            double v;
                            if ( sscanf(value_string.c_str(), "%lf", &v) == 1 )
                                bag_stack.top().first->add
                                    ( new Property<double>( name, description, v ) );
                            else {
                                log(Error) << "Wrong value for property '"+type+"'." \
                                    " Value should contain a double value, got '"+ value_string +"'." << endlog();
                                return false;
                            }
                        }
                        else if ( type == "float")
                        {
                            float v;
                            if ( sscanf(value_string.c_str(), "%f", &v) == 1 )
                                bag_stack.top().first->add
                                    ( new Property<float>( name, description, v ) );
                            else {
                                log(Error) << "Wrong value for property '"+type+"'." \
                                    " Value should contain a float value, got '"+ value_string +"'." << endlog();
                                return false;
                            }
                        }
                        else if ( type == "string")
                            bag_stack.top().first->add
                            ( new Property<std::string>( name, description, value_string ) );
                        else{
                        	log(Error)<<"Unknown type \""<<type<< "\" for for tag simple"<<endlog();
                        	return false;
                        }
                        tag_stack.pop();
                        value_string.clear(); // cleanup
                        description.clear();
                        name.clear();
                        break;

                    case TAG_SEQUENCE:
                    case TAG_STRUCT:
                        {
                            Property<PropertyBag>* prop = bag_stack.top().second;
                            bag_stack.pop();
                            bag_stack.top().first->add( prop );
                            //( new Property<PropertyBag>( pn, description, *pb ) );
                            //delete pb;
                            tag_stack.pop();
                            description.clear();
                            name.clear();
                            type.clear();
                        }
                        break;

                    case TAG_DESCRIPTION:
                        tag_stack.pop();
                        if ( tag_stack.top() == TAG_STRUCT ) {
                            // it is a description of a struct that ended
                            bag_stack.top().second->setDescription(description);
                            description.clear();
                        }
                        break;
                    case TAG_VALUE:
                    case TAG_PROPERTIES:
                    case TAG_UNKNOWN:
                        tag_stack.pop();
                        break;

                    }
                return true;
            }


            void startElement(const char* localname,
                              const TiXmlAttribute* attributes )
            {
                std::string ln = localname;

                if ( ln == "properties" )
                    tag_stack.push( TAG_PROPERTIES );
                else
                    if ( ln == "simple" )
                    {
                        tag_stack.push( TAG_SIMPLE );
                        name.clear();
                        type.clear();
                        while (attributes)
                        {
                            std::string an = attributes->Name();
                            if ( an == "name")
                            {
                                name = attributes->Value();
                            }
                            else if ( an == "type")
                            {
                                type = attributes->Value();
                            }
                            attributes = attributes->Next();
                        }
                    }
                    else
                        if ( ln == "struct" || ln == "sequence")
                        {
                            name.clear();
                            type.clear();
                            while (attributes)
                                {
                                    std::string an = attributes->Name();
                                    if ( an == "name")
                                        {
                                            name = attributes->Value();
                                        }
                                    else if ( an == "type")
                                        {
                                            type = attributes->Value();
                                        }
                                    attributes = attributes->Next();
                                }
                            if ( ln == "struct" )
                                tag_stack.push( TAG_STRUCT );
                            else {
                                tag_stack.push( TAG_SEQUENCE );
                                type = "Sequence"; // override
                            }

                            Property<PropertyBag> *prop;
                            prop = new Property<PropertyBag>(name,"",PropertyBag(type));

                            // take reference to bag itself !
                            bag_stack.push(std::make_pair( &(prop->value()), prop));
                        }
                        else
                                if ( ln == "description")
                                    tag_stack.push( TAG_DESCRIPTION );
                                else
                                    if ( ln == "value"  )
                                        tag_stack.push( TAG_VALUE );
                                    else {
                                        log(Warning) << "Unrecognised XML tag :"<< ln <<": ignoring." << endlog();
                                        tag_stack.push( TAG_UNKNOWN );
                                    }
            }

            void characters( const char* chars )
            {
                switch ( tag_stack.top() )
                {
                    case TAG_DESCRIPTION:
                        description = chars;
                        break;

                    case TAG_VALUE:
                        value_string = chars;;
                        break;
                    case TAG_STRUCT:
                    case TAG_SIMPLE:
                    case TAG_SEQUENCE:
                    case TAG_PROPERTIES:
                    case TAG_UNKNOWN:
                        break;
                }
            }

            bool populateBag(TiXmlNode* pParent)
            {
                if ( !pParent )
                    return false;

                TiXmlNode* pChild;
                TiXmlText* pText;
                int t = pParent->Type();

                switch ( t )
                    {
                    case TiXmlNode::ELEMENT:
                        // notify start of new element
                        this->startElement( pParent->Value(), pParent->ToElement()->FirstAttribute() );

                        // recurse in children, if any
                        for ( pChild = pParent->FirstChild(); pChild != 0; pChild = pChild->NextSibling())
                            {
                                if ( this->populateBag( pChild ) == false){
                                	log(Error)<<"Error in element at line "<<pChild->Row() << endlog();
                                    return false;
                                }
                            }

                        // notify end of element
                        if ( this->endElement() == false )
                            return false;
                        break;

                    case TiXmlNode::TEXT:
                        pText = pParent->ToText();
                        this->characters( pText->Value() );
                        break;

                        // not interested in these...
                    case TiXmlNode::DECLARATION:
                    case TiXmlNode::COMMENT:
                    case TiXmlNode::UNKNOWN:
                    case TiXmlNode::DOCUMENT:
                    default:
                        break;
                    }
            return true;
            }
        };
    }

    using namespace detail;

    class TinyDemarshaller::D {
    public:
        D(const std::string& f) : doc( f.c_str() ), loadOkay(false) {}
        TiXmlDocument doc;
        bool loadOkay;
    };

    TinyDemarshaller::TinyDemarshaller( const std::string& filename )
        : d( new TinyDemarshaller::D(filename) )
    {
        Logger::In in("TinyDemarshaller");
        d->loadOkay = d->doc.LoadFile();

        if ( !d->loadOkay ) {
            log(Error) << "Could not load " << filename << " Error: "<< d->doc.ErrorDesc() << endlog();
            return;
        }

    }

    TinyDemarshaller::~TinyDemarshaller()
    {
        delete d;
    }

    bool TinyDemarshaller::deserialize( PropertyBag &v )
    {
        Logger::In in("TinyDemarshaller");

        if ( !d->loadOkay )
            return false;

		TiXmlHandle docHandle( &d->doc );
		TiXmlHandle propHandle = docHandle.FirstChildElement( "properties" );

        if ( ! propHandle.Node() ) {
            log(Error) << "No <properties> element found in document!"<< endlog();
            return false;
        }

        detail::Tiny2CPFHandler proc( v );

        if ( proc.populateBag( propHandle.Node() ) == false) {
            deleteProperties( v );
            return false;
        }
        return true;
    }

}

