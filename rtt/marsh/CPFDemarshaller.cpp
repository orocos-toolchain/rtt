/***************************************************************************
  tag: Peter Soetens  Tue Apr 5 16:53:25 CEST 2005  CPFDemarshaller.cxx

                        CPFDemarshaller.cxx -  description
                           -------------------
    begin                : Tue April 05 2005
    copyright            : (C) 2005 Peter Soetens
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



#include "CPFDemarshaller.hpp"
#include "CPFDTD.hpp"

#ifdef OROPKG_SUPPORT_XERCES_C
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/util/TransService.hpp>
#include <xercesc/sax2/SAX2XMLReader.hpp>
#include <xercesc/sax2/XMLReaderFactory.hpp>
#include <xercesc/sax2/DefaultHandler.hpp>
#include <xercesc/sax2/Attributes.hpp>
#include <xercesc/util/XMLUniDefs.hpp>
#include <xercesc/util/BinMemInputStream.hpp>
#include <xercesc/framework/LocalFileInputSource.hpp>
#include <xercesc/framework/MemBufInputSource.hpp>
#include <xercesc/validators/common/Grammar.hpp>

#include <vector>
#include <stack>
#include <map>
#include <string>
#include <iostream>
#include <cstdio>

#include <Property.hpp>
#include "../base/PropertyIntrospection.hpp"
#include <Logger.hpp>

namespace RTT
{
#ifdef XERCES_CPP_NAMESPACE
    using namespace XERCES_CPP_NAMESPACE;
#endif
    using namespace marsh;
    using namespace base;

    inline bool XMLChToStdString(const XMLCh* const c, std::string& res)
    {
        char* chholder;
        chholder = XMLString::transcode( c );
        if ( chholder ) {
            res = chholder;
            XMLString::release( &chholder );
            return true;
        }
        log(Error) << "Could not transcode XMLCh* !" <<endlog();
        return false;
    }

    inline std::string  XMLgetString(const XMLCh* const c)
    {
        std::string res;
        char* chholder;
        chholder = XMLString::transcode( c );
        if ( chholder ) {
            res = chholder;
            XMLString::release( &chholder );
            return res;
        }
        log(Error) << "Could not transcode XMLCh* !" <<endlog();
        return res;
    }

    class SAX2CPFHandler : public DefaultHandler
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

            SAX2CPFHandler( PropertyBag &b ) : bag( b )
            {
                Property<PropertyBag>* dummy = 0;
                bag_stack.push(std::make_pair(&bag, dummy));
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
                            if ( value_string == "1" || value_string == "true")
                                bag_stack.top().first->ownProperty
                                ( new Property<bool>( name, description, true ) );
                            else if ( value_string == "0" || value_string == "false" )
                                bag_stack.top().first->ownProperty
                                ( new Property<bool>( name, description, false ) );
                            else
                                throw SAXException(std::string("Wrong value for property '"+type+"'." \
                                                               " Value should contain '0' or '1', got '"+ value_string +"'.").c_str());
                        }
                        else if ( type == "char" ) {
                            if ( value_string.empty() )
                                bag_stack.top().first->ownProperty( new Property<char>( name, description, '\0' ) );
                            else
                                if ( value_string.length() != 1 )
                                    throw SAXException(std::string("Wrong value for property '"+type+"'." \
                                                               " Value should contain a single character, got '"+ value_string +"'.").c_str());
                                else
                                    bag_stack.top().first->ownProperty( new Property<char>( name, description, value_string[0] ) );
                        }
                        else if ( type == "uchar" || type == "octet" ) {
                            if ( value_string.length() != 1 )
                                throw SAXException(std::string("Wrong value for property '"+type+"'." \
                                                               " Value should contain a single unsigned character, got '"+ value_string +"'.").c_str());
                            else
                                bag_stack.top().first->ownProperty
                                    ( new Property<unsigned char>( name, description, value_string[0] ) );
                        }
                        else if ( type == "long" || type == "short")
                        {
                            int v;
                            if ( sscanf(value_string.c_str(), "%d", &v) == 1)
                                bag_stack.top().first->ownProperty( new Property<int>( name, description, v ) );
                            else
                                throw SAXException(std::string("Wrong value for property '"+type+"'." \
                                                               " Value should contain an integer value, got '"+ value_string +"'.").c_str());
                        }
                        else if ( type == "ulong" || type == "ushort")
                        {
                            unsigned int v;
                            if ( sscanf(value_string.c_str(), "%u", &v) == 1)
                                bag_stack.top().first->ownProperty( new Property<unsigned int>( name, description, v ) );
                            else
                                throw SAXException(std::string("Wrong value for property '"+type+"'." \
                                                               " Value should contain an integer value, got '"+ value_string +"'.").c_str());
                        }
                        else if ( type == "llong")
                        {
                            long long v;
                            if ( sscanf(value_string.c_str(), "%lld", &v) == 1)
                                bag_stack.top().first->ownProperty( new Property<long long>( name, description, v ) );
                            else
                                throw SAXException(std::string("Wrong value for property '"+type+"'." \
                                                               " Value should contain an integer value, got '"+ value_string +"'.").c_str());
                        }
                        else if ( type == "ullong")
                        {
                            unsigned long long v;
                            if ( sscanf(value_string.c_str(), "%llu", &v) == 1)
                                bag_stack.top().first->ownProperty( new Property<unsigned long long>( name, description, v ) );
                            else
                                throw SAXException(std::string("Wrong value for property '"+type+"'." \
                                                               " Value should contain an integer value, got '"+ value_string +"'.").c_str());
                        }
                        else if ( type == "double")
                        {
                            double v;
                            if ( sscanf(value_string.c_str(), "%lf", &v) == 1 )
                                bag_stack.top().first->ownProperty
                                    ( new Property<double>( name, description, v ) );
                            else
                                throw SAXException(std::string("Wrong value for property '"+type+"'." \
                                                               " Value should contain a double value, got '"+ value_string +"'.").c_str());
                        }
                        else if ( type == "float")
                        {
                            float v;
                            if ( sscanf(value_string.c_str(), "%f", &v) == 1 )
                                bag_stack.top().first->ownProperty
                                    ( new Property<float>( name, description, v ) );
                            else
                                throw SAXException(std::string("Wrong value for property '"+type+"'." \
                                                               " Value should contain a float value, got '"+ value_string +"'.").c_str());
                        }
                        else if ( type == "string")
                            bag_stack.top().first->ownProperty
                            ( new Property<std::string>( name, description, value_string ) );
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
                            bag_stack.top().first->ownProperty( prop );
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
                        name.clear();
                        type.clear();
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
                            name.clear();
                            type.clear();

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
#if XERCES_VERSION_MAJOR < 3
            void characters( const XMLCh* const chars, const unsigned int length )
#else
            void characters( const XMLCh* const chars, const XMLSize_t length )
#endif
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
                    case TAG_UNKNOWN:
                        break;
                }
            }


    };

    CPFDemarshaller::CPFDemarshaller( const std::string& filename )
        : name(0), fis(0)
    {
        Logger::In in("CPFDemarshaller");
        try
            {
                XMLPlatformUtils::Initialize();
            }
        catch ( const XMLException & toCatch )
            {
                std::string error;
                XMLChToStdString(toCatch.getMessage(), error);
                Logger::log() << Logger::Error << "XML Initialization : "
                              << error << Logger::endl;
            }
        catch ( ... )
            {
                Logger::log() << Logger::Error << "XML Init: General System Exception !" << Logger::endl;
            }

        try {
            name =  XMLString::transcode( filename.c_str() );
            fis  = new LocalFileInputSource( name );
        }
        catch ( XMLException& xe )
            {
                Logger::log() << Logger::Error << "Failed to open file " <<filename << Logger::endl;
                Logger::log() << Logger::Error << xe.getMessage() << Logger::endl;

                fis = 0;
            }
        catch ( ... )
            {
                Logger::log() << Logger::Error << "Opening file: General System Exception !" << Logger::endl;
            }
        XMLString::release( &name );
    }

    CPFDemarshaller::~CPFDemarshaller()
    {
        delete fis;
        XMLPlatformUtils::Terminate();
    }

    bool CPFDemarshaller::deserialize( PropertyBag &v )
    {
        if ( fis == 0 )
            return false;

        SAX2XMLReader* parser = 0;
        try {
            XMLPlatformUtils::Initialize();
            parser = XMLReaderFactory::createXMLReader();
        }
        catch ( ... )
            {
                Logger::log() << Logger::Error << "SAX2XMLReader System Exception !" << Logger::endl;
                XMLPlatformUtils::Terminate();
                return false;
            }


        Logger::In in("CPFDemarshaller");

        try
            {
                SAX2CPFHandler handler( v );
                parser->setContentHandler( &handler );
                parser->setErrorHandler( &handler );
                parser->setFeature( XMLUni::fgSAX2CoreValidation, false );
                parser->setFeature( XMLUni::fgXercesValidationErrorAsFatal, false );
                parser->setFeature( XMLUni::fgXercesContinueAfterFatalError, false );
#if 0
                parser->setFeature( XMLUni::fgXercesSchemaFullChecking, false );
                parser->setFeature( XMLUni::fgXercesDynamic, true );
                parser->setFeature( XMLUni::fgSAX2CoreNameSpaces, true );
                parser->setFeature( XMLUni::fgSAX2CoreNameSpacePrefixes, true );
                parser->setFeature( XMLUni::fgXercesSchema, false );
#endif
                // try to avoid loading the DTD mentioned in the xml file,
                // and load our own grammer.
                using namespace detail;
                parser->setFeature( XMLUni::fgXercesLoadExternalDTD, false );
                //parser->setDoValidation(true);
                int length  = XMLString::stringLen(cpf_dtd);// string length
                XMLByte* buffer = new XMLByte[length];
                memcpy( buffer, cpf_dtd, length );
                MemBufInputSource dtd(buffer, length, "internal_cpf.dtd");
                parser->loadGrammar( dtd, Grammar::DTDGrammarType );
                delete[] buffer;

                parser->parse( *fis );
                parser->getErrorCount();
                delete parser;
                XMLPlatformUtils::Terminate();
            }
        catch ( const XMLException & toCatch )
            {
                Logger::log() << Logger::Error << "An XML parsing error occurred processing file " <<Logger::nl;
                if ( toCatch.getSrcFile() ) {
                    Logger::log() <<  toCatch.getSrcFile() << " parsing line " << toCatch.getSrcLine()<<Logger::nl ;
                }
                Logger::log()  << XMLgetString(toCatch.getMessage()) <<Logger::endl;
                delete parser;
                XMLPlatformUtils::Terminate();
                return false;
            }
        catch ( const SAXParseException & toCatch )
            {
                Logger::log() << Logger::Error << "An XML SAX parsing error occurred processing file " <<Logger::nl;
                Logger::log()  << XMLgetString(toCatch.getMessage()) <<Logger::endl;
                if ( toCatch.getPublicId() )
                    {
                        Logger::log() << " At entity "<< XMLgetString(toCatch.getPublicId()) <<Logger::nl;
                    }
                Logger::log() << " Column "<< toCatch.getColumnNumber()<< " Line " <<toCatch.getLineNumber()<<Logger::endl;
                delete parser;
                XMLPlatformUtils::Terminate();
                return false;
            }
        catch ( const SAXException & toCatch )
            {
                Logger::log() << Logger::Error << "An XML SAX exception occurred processing file " <<Logger::nl;
                Logger::log()  << XMLgetString(toCatch.getMessage()) <<Logger::endl;
                delete parser;
                XMLPlatformUtils::Terminate();
                return false;
            }
        catch ( ... )
            {
                Logger::log() << Logger::Error << "General System Exception !" << Logger::endl;
                delete parser;
                XMLPlatformUtils::Terminate();
                return false;
            }
        return true;
    }
}

#endif

