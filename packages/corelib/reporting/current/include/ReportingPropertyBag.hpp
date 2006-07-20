/***************************************************************************
  tag: Peter Soetens  Mon Jan 19 14:11:19 CET 2004  ReportingPropertyBag.hpp 

                        ReportingPropertyBag.hpp -  description
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
 
#ifndef REPORTINGPROPERTYBAG_HPP
#define REPORTINGPROPERTYBAG_HPP  

#include "PropertyBag.hpp"
#include "Property.hpp"
#include "TimeService.hpp"
#include "ReportExporterInterface.hpp"
#include "ReportCollectorInterface.hpp"
#include "marshalling/XMLMarshaller.hpp"
#include "marshalling/SimpleMarshaller.hpp"
#include "marshalling/Orocos1Marshaller.hpp"
#include <sstream>

namespace RTT
{
    /**
     * This PropertyBag is extended to report its contents
     * as a Report Server, it reports to a ReportWriter the contents of the bag.
     * You can use it as a normal PropertyBag
     * and use the add()/remove() methods to insert or remove a Property
     * from the bag. If this ReportingPropertyBag instance is added to the
     * ReportWriter, the latter will periodically write out the contents of
     * the PropertyBag.
     *
     * @warning This class is not tested for realtime safeness.
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

            virtual void refresh( TimeService::Seconds time_stamp )
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
