#ifndef REPORTINGPROPERTYBAG_HPP
#define REPORTINGPROPERTYBAG_HPP  

#include <corelib/PropertyBag.hpp>
#include <corelib/Property.hpp>
#include "ReportExporterInterface.hpp"
#include "ReportCollectorInterface.hpp"
#include <corelib/marshalling/XMLMarshaller.hpp>
#include <corelib/marshalling/SimpleMarshaller.hpp>
#include <corelib/marshalling/Orocos1Marshaller.hpp>
#include <sstream>

namespace ORO_CoreLib
{
    /**
     * This PropertyBag is extended to report its contents
     * as a Report Server. You can use it as a normal PropertyBag
     * and use the add()/remove() methods to insert or remove a Property
     * from the bag.
     *
     */
    class ReportingPropertyBag
        : public PropertyBag, 
          public StringExporterInterface
    {
        public:
            typedef std::stringstream StreamType;
            typedef XMLMarshaller<StreamType> XMLMarshal;
            typedef SimpleMarshaller<StreamType> SimpleMarshal;
            typedef Orocos1Marshaller<StreamType> Orocos1Marshal;

            /**
             * The MarshallType lists the three methods for marshalling
             * the properties to a string format which can then be reported.
             */
            enum MarshallType  { XML, Simple, Orocos };
            
            /**
             * Create a nameserver ReportingPropertyBag.
             *
             * @param name The name under which it is listed as StringExporterInterface.
             */
            ReportingPropertyBag( const std::string& name, MarshallType mt = Simple)
                : PropertyBag(name), StringExporterInterface(name), 
                  xml_m( buffer ), simple_m( buffer ), orocos_m( buffer ),
                  strategy( &orocos_m ), timeStamp("TimeStamp","Sample time.")
            {
                this->add( &timeStamp );
                setMarshaller(mt);
            }
            
            /**
             * Create a PropertyBag whose contents can be reported.
             */
            ReportingPropertyBag( MarshallType mt = Simple ) 
                  : xml_m( buffer ), simple_m( buffer ), orocos_m( buffer ),
                  strategy( &orocos_m ), timeStamp("TimeStamp","Sample time.")
            {
                this->add( &timeStamp );
                setMarshaller(mt);
            }

            virtual ~ReportingPropertyBag() {}

            virtual void reportGet(std::string& report)
            {
                report = buffer.str();
                buffer.str("");
            }

            virtual std::string& reportGet()
            {
                result = buffer.str();
                buffer.str("");
                return result;
            }

            virtual void refresh( HeartBeatGenerator::Seconds time_stamp )
            {
                timeStamp = time_stamp;
                strategy->serialize( *this );
            }

            /**
             * Set the Marshaller to use.
             *
             * @param mt The MarshallType to use.
             */
            void setMarshaller(MarshallType mt)
            {
                switch (mt)
                {
                    case XML:
                        strategy = &xml_m;
                        break;
                    case Simple:
                        strategy = &simple_m;
                        break;
                    case Orocos:
                        strategy = &orocos_m;
                        break;
                }
            }

        protected:

            XMLMarshal xml_m;
            SimpleMarshal simple_m;
            Orocos1Marshal orocos_m;

            Marshaller* strategy;

            StreamType buffer;

            std::string result;
            
            Property<double> timeStamp;

    };     

}

#endif
