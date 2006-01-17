/***************************************************************************
  tag: Peter Soetens  Mon Jan 19 14:11:19 CET 2004  PropertyExporter.hpp 

                        PropertyExporter.hpp -  description
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
 
#ifndef PROPERTYEXPORTER_HPP
#define PROPERTYEXPORTER_HPP

#include "PropertyBag.hpp"
#include "Property.hpp"
#include "PropertyIntrospection.hpp"
#include "ReportExporterInterface.hpp"
#include "TimeService.hpp"

#ifdef ORO_PRAGMA_INTERFACE
#pragma interface
#endif

namespace ORO_CoreLib
{
    /**
     * @brief This PropertyBag is extended to report its contents
     * as a Report Server.
     *
     * You can use it as a normal PropertyBag
     * and use the add()/remove() methods to insert or remove a Property
     * from the bag.
     *
     */
    class PropertyExporter
        : public Property<PropertyBag>,
          public PropertyExporterInterface
    {
        public:
            
            /**
             * Create a nameserver PropertyExporter.
             *
             * @param name The name under which it is listed as StringExporterInterface.
             * @param copy_on_refresh Indicates if you wish to copy the contents of this
             *        bag on refresh (to make a snapshot), or just update the timestamp.
             *        Setting this to true will introduce a time latency due to the copy
             *        operation, false, the default, assumes it is safe to read the contents
             *        of this bag later on.
             */
            PropertyExporter( const std::string& name, bool copy_on_refresh = false)
                : Property<PropertyBag>(name,"PropertyExporter"), PropertyExporterInterface(name), 
                  timeStamp("TimeStamp","Sample time."), bagCopy(name, "PropertyExporter"), copy(copy_on_refresh)
            {
                this->value().add( &timeStamp );
            }
            
            /**
             * Create a PropertyBag whose contents can be reported.
             */
            PropertyExporter( bool copy_on_refresh = false) 
                : Property<PropertyBag>("Bag","PropertyExporter"),
            timeStamp("TimeStamp","Sample time."), bagCopy("Bag", "PropertyExporter"), copy(copy_on_refresh)
            {
                this->value().add( &timeStamp );
            }

            virtual ~PropertyExporter() 
            {
                this->value().remove( &timeStamp );
            }

            virtual void reportGet(Property<PropertyBag>& report)
            {
                if (copy)
                    report.value().add( &bagCopy );
                else
                    report.value().add( this );
            }

            virtual Property<PropertyBag>& reportGet()
            {
                if (copy)
                    return bagCopy;
                else
                    return *this;
            }

        using Property<PropertyBag>::refresh;

            virtual void refresh( TimeService::Seconds time_stamp )
            {
                timeStamp = time_stamp;
                if (copy)
                {
                    copyProperties(bagCopy.value(), this->value() ); 
                    // this makes a copy of this bag, updating existing values.
                }
            }

        protected:

            Property<double> timeStamp;

            /**
             * A copy of this bag, made on refresh.
             */
            Property<PropertyBag> bagCopy;

            /**
             * On true, the exporter will copy the bag for reporting, on false,
             * it will pass on the original.
             */
            bool copy;
    };     

}

#endif
