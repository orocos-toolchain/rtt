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

#include <corelib/Property.hpp>
#include "StreamProcessor.hpp"

namespace ORO_CoreLib
{
    using  std::string;
#ifdef XERCES_CPP_NAMESPACE
    using namespace XERCES_CPP_NAMESPACE;
#endif

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
                        else if ( type == "long") 
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
                string ln = XMLString::transcode( localname );


                if ( ln == "properties" )
                    tag_stack.push( TAG_PROPERTIES );
                else
                    if ( ln == "simple" ) 
                    {

                        tag_stack.push( TAG_SIMPLE );

                        for (unsigned int ac = 0; ac < attributes.getLength(); ++ac)
                        {
                            string an = XMLString::transcode( attributes.getLocalName(ac) );
                            if ( an == "name") 
                            {
                                name = XMLString::transcode( attributes.getValue(ac) );
                            }
                            else if ( an == "type")
                            {
                                type = XMLString::transcode( attributes.getValue(ac) );
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
                                string an = XMLString::transcode( attributes.getLocalName(ac) );
                                if ( an == "name") 
                                {
                                    name = XMLString::transcode( attributes.getValue(ac) );
                                }
                                else if ( an == "type") 
                                {
                                    hasType = true;
                                    type = XMLString::transcode( attributes.getValue(ac) );
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
                printf("\nWarning\n");
            }
            void error( const SAXParseException& exception )
            {
                printf( "\nError\n");
            }
            void fatalError( const SAXParseException& exception )
            {
                printf( "\nFatal error\n");
            }
            void characters( const XMLCh* const chars, const unsigned int length )
            {
                //char *ln = XMLString::transcode( chars );
                switch ( tag_stack.top() )
                {
                    case TAG_DESCRIPTION:
                        description = XMLString::transcode( chars );
                        break;

                    case TAG_VALUE:
                        value_string = XMLString::transcode( chars );
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
     * A demarshaller for extracting properties and property bags
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
                    //                cerr << "Error during initialization! :\n" /*<< StrX(toCatch.getMessage())*/ << endl;
                }
                catch ( ... )
                {
                    //              cerr << "other exception" << endl;
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
                }
                catch ( const XMLException & toCatch )
                {
                    //cerr << "\nAn XML parsing error occurred\n  Error: " << endl;
                    printf("error xml");
                    XMLPlatformUtils::Terminate();
                    return false;
                }
                catch ( ... )
                {
                    printf("error");
                    //                    cerr << "General error" << endl;
                    XMLPlatformUtils::Terminate();
                    return false;
                }
                return true;
            }
    };
}
#endif
