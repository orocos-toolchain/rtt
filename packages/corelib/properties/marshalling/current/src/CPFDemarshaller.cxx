/***************************************************************************
  tag: Peter Soetens  Tue Apr 5 16:53:25 CEST 2005  CPFDemarshaller.cxx 

                        CPFDemarshaller.cxx -  description
                           -------------------
    begin                : Tue April 05 2005
    copyright            : (C) 2005 Peter Soetens
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
 
 

#include "corelib/marshalling/CPFDemarshaller.hpp"

#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/util/TransService.hpp>
#include <xercesc/sax2/SAX2XMLReader.hpp>
#include <xercesc/sax2/XMLReaderFactory.hpp>
#include <xercesc/sax2/DefaultHandler.hpp>
#include <xercesc/sax2/Attributes.hpp>
#include <xercesc/util/XMLUniDefs.hpp>
#include <xercesc/util/BinMemInputStream.hpp>
#include <xercesc/framework/LocalFileInputSource.hpp>

#include <vector>
#include <stack>
#include <map>
#include <string>
#include <iostream>

#include <corelib/Property.hpp>
#include "corelib/PropertyIntrospection.hpp"
#include <corelib/Logger.hpp>

namespace ORO_CoreLib
{
#ifdef XERCES_CPP_NAMESPACE
    using namespace XERCES_CPP_NAMESPACE;
#endif

    inline void XMLChToStdString(const XMLCh* const c, std::string& res)
    {
        char* chholder;
        chholder = XMLString::transcode( c );
        res = chholder;
        delete[] chholder;
    }

    class SAX2CPFHandler : public DefaultHandler
    {

            /**
             * Stores the results of the parsing.
             */
            PropertyBag &bag;
            std::stack< std::pair<PropertyBag*, std::string > > bag_stack;

            enum Tag { TAG_STRUCT, TAG_SIMPLE, TAG_SEQUENCE, TAG_PROPERTIES, TAG_DESCRIPTION, TAG_VALUE};
            std::stack<Tag> tag_stack;

            /**
             * The name of the property.
             */
            std::string name;
            std::string description;
            std::string type;
            std::string value_string;

        public:

            SAX2CPFHandler( PropertyBag &b ) : bag( b )
            {
                bag_stack.push(std::make_pair(&bag,"root"));
            }

            void endElement( const XMLCh* const uri,
                             const XMLCh* const localname,
                             const XMLCh* const qname )
            {
                //char *ln = XMLString::transcode( localname );

//                 if ( value_string.empty() ) {
//                     Logger::log()<<Logger::Debug << "SAX2CPFHandler : Empty value for property."
//                                  <<Logger::endl;
//                 }
                switch ( tag_stack.top() )
                {
                    case TAG_SIMPLE:
                        if ( type == "boolean" )
                        {
                            if ( value_string == "1" )
                                bag_stack.top().first->add
                                ( new Property<bool>( name, description, true ) );
                            else if ( value_string == "0" )
                                bag_stack.top().first->add
                                ( new Property<bool>( name, description, false ) );
                            else
                                throw SAXException(std::string("Wrong value for property '"+type+"'." \
                                                               " Value should contain '0' or '1', got '"+ value_string +"'.").c_str());
                        }
                        else if ( type == "char" ) {
                            if ( value_string.length() != 1 )
                                throw SAXException(std::string("Wrong value for property '"+type+"'." \
                                                               " Value should contain a single character, got '"+ value_string +"'.").c_str());
                            else 
                                bag_stack.top().first->add
                                    ( new Property<char>( name, description, value_string[0] ) );
                        }
                        else if ( type == "long" || type == "short") 
                        {
                            int v;
                            if ( sscanf(value_string.c_str(), "%d", &v) == 1)
                                bag_stack.top().first->add( new Property<int>( name, description, v ) );
                            else
                                throw SAXException(std::string("Wrong value for property '"+type+"'." \
                                                               " Value should contain an integer value, got '"+ value_string +"'.").c_str());
                        }
                        else if ( type == "double") 
                        {
                            double v;
                            if ( sscanf(value_string.c_str(), "%lf", &v) == 1 )
                                bag_stack.top().first->add
                                    ( new Property<double>( name, description, v ) );
                            else
                                throw SAXException(std::string("Wrong value for property '"+type+"'." \
                                                               " Value should contain a double value, got '"+ value_string +"'.").c_str());
                        }
                        else if ( type == "string") 
                            bag_stack.top().first->add
                            ( new Property<std::string>( name, description, value_string ) );
                        tag_stack.pop();
                        value_string.clear(); // cleanup
                        description.clear();
                        name.clear();
                        break;

                    case TAG_SEQUENCE:
                    case TAG_STRUCT:
                        {
                            PropertyBag *pb = bag_stack.top().first;
                            std::string pn = bag_stack.top().second;
                            bag_stack.pop();
                            bag_stack.top().first->add
                            ( new Property<PropertyBag>( pn, description, *pb ) );
                            delete pb;
                            tag_stack.pop();
                            description.clear();
                            name.clear();
                            type.clear();
                        }
                        break;

                    case TAG_VALUE:
                    case TAG_DESCRIPTION:
                    case TAG_PROPERTIES:
                        tag_stack.pop();
                        break;

                }
            }


            void startElement( const XMLCh* const uri,
                               const XMLCh* const localname,
                               const XMLCh* const qname,
                               const Attributes& attributes )
            {
                std::string ln;
                XMLChToStdString( localname, ln );

                if ( ln == "properties" )
                    tag_stack.push( TAG_PROPERTIES );
                else
                    if ( ln == "simple" ) 
                    {

                        tag_stack.push( TAG_SIMPLE );
                        for (unsigned int ac = 0; ac < attributes.getLength(); ++ac)
                        {
                            std::string an;
                            XMLChToStdString( attributes.getLocalName(ac), an );
                            if ( an == "name") 
                            {
                                XMLChToStdString( attributes.getValue(ac), name);
                            }
                            else if ( an == "type")
                            {
                                XMLChToStdString( attributes.getValue(ac), type);
                            }
                        }
                    }
                    else
                        if ( ln == "struct" || ln == "sequence")
                        {
                            if ( ln == "struct" )
                                tag_stack.push( TAG_STRUCT );
                            else 
                                tag_stack.push( TAG_SEQUENCE );

                            //type tag is optional (?)
                            bool hasType = false;
                            for (unsigned int ac = 0; ac < attributes.getLength(); ++ac)
                            {
                                std::string an;
                                XMLChToStdString( attributes.getLocalName(ac), an );
                                if ( an == "name") 
                                {
                                    XMLChToStdString( attributes.getValue(ac), name);
                                }
                                else if ( an == "type") 
                                {
                                    hasType = true;
                                    XMLChToStdString( attributes.getValue(ac), type);
                                }
                            }
                            PropertyBag *pb;
                            if (hasType)
                                pb = new PropertyBag(type);
                            else
                                pb = new PropertyBag();
                            
                            bag_stack.push(std::make_pair(pb, std::string(name)));
                        }
                        else
                                if ( ln == "description") 
                                    tag_stack.push( TAG_DESCRIPTION );
                                else
                                    if ( ln == "value"  )
                                        tag_stack.push( TAG_VALUE );
            }

            void warning( const SAXParseException& exception )
            {
                std::string warn;
                XMLChToStdString( exception.getMessage(), warn);
                Logger::log() << Logger::Warning << "SAX2CPFHandler Parsing: " << warn <<Logger::nl;
                if ( exception.getPublicId() )
                    {
                        XMLChToStdString( exception.getPublicId(), warn);
                        Logger::log() << " At entity "<< warn <<Logger::nl;
                    }
                Logger::log() << " Column "<< exception.getColumnNumber()<< " Line " <<exception.getLineNumber()<<Logger::endl;
                // to not throw.
            }

            void error( const SAXParseException& exception )
            {
                throw exception;
            }
            void fatalError( const SAXParseException& exception )
            {
                throw exception;
            }
            void characters( const XMLCh* const chars, const unsigned int length )
            {
                //char *ln = XMLString::transcode( chars );
                switch ( tag_stack.top() )
                {
                    case TAG_DESCRIPTION:
                        XMLChToStdString( chars, description);
                        break;

                    case TAG_VALUE:
                        XMLChToStdString( chars, value_string);
                        break;
                    case TAG_STRUCT:
                    case TAG_SIMPLE:
                    case TAG_SEQUENCE:
                    case TAG_PROPERTIES:
                        break;
                }
            }


    };

    CPFDemarshaller::CPFDemarshaller( const std::string& filename )
        : name(0)
    {
        XMLPlatformUtils::Initialize();
        name =  XMLString::transcode( filename.c_str() );
        fis  = new LocalFileInputSource( name );
        delete[] name;
    }

    CPFDemarshaller::~CPFDemarshaller()
    {
        delete fis;
        XMLPlatformUtils::Terminate();
    }
    
    bool CPFDemarshaller::deserialize( PropertyBag &v )
    {
        try
            {
                XMLPlatformUtils::Initialize();
            }
        catch ( const XMLException & toCatch )
            {
                std::string error;
                XMLChToStdString(toCatch.getMessage(), error);
                Logger::log() << Logger::Error << "CPFDemarshaller initialization : "
                              << error << Logger::endl;
                return false;
            }
        catch ( ... )
            {
                Logger::log() << Logger::Error << "CPFDemarshaller : General System Exception !" << Logger::endl;
                return false;
            }

        SAX2XMLReader* parser = XMLReaderFactory::createXMLReader();

        int errorCount = 0;

        try
            {
                SAX2CPFHandler handler( v );
                parser->setContentHandler( &handler );
                parser->setErrorHandler( &handler );
                parser->setFeature( XMLUni::fgSAX2CoreValidation, true );
                parser->setFeature( XMLUni::fgXercesValidationErrorAsFatal, true );
                parser->setFeature( XMLUni::fgXercesContinueAfterFatalError, false );
#if 0
                parser->setFeature( XMLUni::fgXercesSchemaFullChecking, false );
                parser->setFeature( XMLUni::fgXercesDynamic, true );
                parser->setFeature( XMLUni::fgSAX2CoreNameSpaces, true );
                parser->setFeature( XMLUni::fgSAX2CoreNameSpacePrefixes, true );
                parser->setFeature( XMLUni::fgXercesSchema, false );
#endif

                parser->parse( *fis );
                errorCount = parser->getErrorCount();
                delete parser;
                XMLPlatformUtils::Terminate();
            }
        catch ( const XMLException & toCatch )
            {
                Logger::log() << Logger::Error << "CPFDemarshaller: An XML parsing error occurred processing file " <<Logger::nl;
                if ( XMLString::transcode(toCatch.getSrcFile()) )
                    Logger::log() <<  XMLString::transcode(toCatch.getSrcFile()) << " parsing line " << toCatch.getSrcLine()<<Logger::nl ;
                Logger::log()  << XMLString::transcode(toCatch.getMessage()) <<Logger::endl;
                delete parser;
                XMLPlatformUtils::Terminate();
                return false;
            }
        catch ( const SAXParseException & toCatch )
            {
                Logger::log() << Logger::Error << "CPFDemarshaller: An XML SAX parsing error occurred processing file " <<Logger::nl;
                Logger::log()  << XMLString::transcode(toCatch.getMessage()) <<Logger::endl;
                if ( toCatch.getPublicId() )
                    {
                        std::string ent;
                        XMLChToStdString(toCatch.getPublicId(), ent);
                        Logger::log() << " At entity "<< ent <<Logger::nl;
                    }
                Logger::log() << " Column "<< toCatch.getColumnNumber()<< " Line " <<toCatch.getLineNumber()<<Logger::endl;
                delete parser;
                XMLPlatformUtils::Terminate();
                return false;
            }
        catch ( const SAXException & toCatch )
            {
                Logger::log() << Logger::Error << "CPFDemarshaller: An XML SAX exception occurred processing file " <<Logger::nl;
                Logger::log()  << XMLString::transcode(toCatch.getMessage()) <<Logger::endl;
                delete parser;
                XMLPlatformUtils::Terminate();
                return false;
            }
        catch ( ... )
            {
                Logger::log() << Logger::Error << "CPFDemarshaller: General System Exception !" << Logger::endl;
                delete parser;
                XMLPlatformUtils::Terminate();
                return false;
            }
        return true;
    }
}

