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

#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/util/TransService.hpp>
#include <xercesc/sax2/SAX2XMLReader.hpp>
#include <xercesc/sax2/XMLReaderFactory.hpp>
#include <xercesc/sax2/DefaultHandler.hpp>
#include <xercesc/sax2/Attributes.hpp>
#include <xercesc/util/XMLUniDefs.hpp>
#include <xercesc/util/BinMemInputStream.hpp>
#include <xercesc/framework/StdInInputSource.hpp>

#include <vector>
#include <stack>
#include <map>
#include <string>
#include <iostream>

#include <corelib/Property.hpp>
#include "StreamProcessor.hpp"

namespace ORO_CoreLib
{
    using  std::string;
    using  std::cerr;
    using  std::endl;

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
            string name;
            string description;
            string type;
            string value_string;

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

                switch ( tag_stack.top() )
                {
                    case TAG_SIMPLE:
                        if ( type == "boolean" )
                        {
                            if ( value_string == "1" )
                                bag_stack.top().first->add
                                ( new Property<bool>( name, description, true ) );
                            else
                                bag_stack.top().first->add
                                ( new Property<bool>( name, description, false ) );
                        }
                        else if ( type == "char" )
                            bag_stack.top().first->add
                            ( new Property<char>( name, description, value_string[0] ) );
                        else if ( type == "long" || type == "short") 
                        {
                            int v;
                            sscanf(value_string.c_str(), "%d", &v);
                            bag_stack.top().first->add
                            ( new Property<int>( name, description, v ) );
                        }
                        else if ( type == "double") 
                        {
                            double v;
                            sscanf(value_string.c_str(), "%lf", &v);
                            bag_stack.top().first->add
                            ( new Property<double>( name, description, v ) );
                        }
                        else if ( type == "string") 
                            bag_stack.top().first->add
                            ( new Property<std::string>( name, description, value_string ) );
                        tag_stack.pop();
                        break;

                    case TAG_SEQUENCE:
                    case TAG_STRUCT:
                        {
                            PropertyBag *pb = bag_stack.top().first;
                            std::string pn = bag_stack.top().second;
                            bag_stack.pop();
                            bag_stack.top().first->add
                            ( new Property<PropertyBag>( pn, description, *pb ) );
                            tag_stack.pop();
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
                string ln;
                XMLChToStdString( localname, ln );

                if ( ln == "properties" )
                    tag_stack.push( TAG_PROPERTIES );
                else
                    if ( ln == "simple" ) 
                    {

                        tag_stack.push( TAG_SIMPLE );

                        for (unsigned int ac = 0; ac < attributes.getLength(); ++ac)
                        {
                            string an;
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
                                string an;
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
                string warn;
                XMLChToStdString( exception.getMessage(), warn);
                cerr << "Parse Warning : " << warn <<endl;
                if ( exception.getPublicId() )
                    {
                        XMLChToStdString( exception.getPublicId(), warn);
                        cerr << " At entity "<< warn <<endl;
                    }
                cerr << " Column "<< exception.getColumnNumber()<< " Line " <<exception.getLineNumber()<<endl;
            }

            void error( const SAXParseException& exception )
            {
                string warn;
                XMLChToStdString( exception.getMessage(), warn);
                cerr << "Parse Error : " << warn <<endl;
                if ( exception.getPublicId() )
                    {
                        XMLChToStdString( exception.getPublicId(), warn);
                        cerr << " At entity "<< warn <<endl;
                    }
                cerr << " Column "<< exception.getColumnNumber()<< " Line " <<exception.getLineNumber()<<endl;
            }
            void fatalError( const SAXParseException& exception )
            {
                string warn;
                XMLChToStdString(exception.getMessage(), warn);
                cerr << "Parse Fatal Error : " << warn <<endl;
                if ( exception.getPublicId() )
                    {
                        XMLChToStdString(exception.getPublicId(), warn);
                        cerr << " At entity "<< warn <<endl;
                    }
                cerr << " Column "<< exception.getColumnNumber()<< " Line " <<exception.getLineNumber()<<endl;
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


    /**
     * @brief A demarshaller for extracting properties and property bags
     * from a Component Property File following the CORBA 3 standard.
     */
    template <typename input_stream>
    class CPFDemarshaller : public Demarshaller, public StreamProcessor<input_stream>
    {

        public:
            CPFDemarshaller( input_stream &is ) :
                    StreamProcessor<input_stream>( is )
            {}

            virtual bool deserialize( PropertyBag &v )
            {
                try
                {
                    XMLPlatformUtils::Initialize();
                }
                catch ( const XMLException & toCatch )
                {
                    string error;
                    XMLChToStdString(toCatch.getMessage(), error);
                    cerr << "Error during initialization! : " <<endl;
                    cerr << error << endl;
                    return false;
                }
                catch ( ... )
                {
                    cerr << "General System Exception !" << endl;
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

                    parser->parse( *s );
                    errorCount = parser->getErrorCount();
                    delete parser;
                    XMLPlatformUtils::Terminate();
                }
                catch ( const XMLException & toCatch )
                {
                    cerr << "An XML parsing error occurred processing file " <<endl;
                    if ( XMLString::transcode(toCatch.getSrcFile()) )
                        cerr <<  XMLString::transcode(toCatch.getSrcFile()) << " parsing line " << toCatch.getSrcLine()<<endl ;
                    cerr << " Error: " << endl;
                    cerr << XMLString::transcode(toCatch.getMessage()) <<endl;
                    delete parser;
                    XMLPlatformUtils::Terminate();
                    return false;
                }
                catch ( ... )
                {
                    cerr << "General System Exception !" << endl;
                    delete parser;
                    XMLPlatformUtils::Terminate();
                    return false;
                }
                return true;
            }
    };
}
#endif
