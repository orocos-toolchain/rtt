/***************************************************************************
  tag: Peter Soetens  Thu Apr 22 20:41:00 CEST 2004  DataObjectReporting.hpp 

                        DataObjectReporting.hpp -  description
                           -------------------
    begin                : Thu April 22 2004
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
 
 
#ifndef DATAOBJECTREPORTING_HPP
#define DATAOBJECTREPORTING_HPP

#include <corelib/NameServer.hpp>
#include <corelib/PropertyExporter.hpp>
#include <corelib/PropertyReporter.hpp>
#include <corelib/PropertyBagIntrospector.hpp>


#pragma interface

namespace ORO_ControlKernel
{
    using ORO_CoreLib::Property;
    using ORO_CoreLib::PropertyBag;
    using ORO_CoreLib::PropertyExporter;
    using ORO_CoreLib::PropertyIntrospection;

    /**
     * @brief An interface for gathering reports from DataObjects.
     *
     * It is very similar to the ReportingComponent Interface,
     * but the semantic difference is big enough to motivate
     * a separate interface.
     */
    struct DataObjectReporting
    {
        static ORO_CoreLib::NameServer<DataObjectReporting*> nameserver;
        /**
         * Create a new DataObject suitable for reporting its contents.
         
         * @param name is the globally visible name of this DataObject.
         *  It is usually prefixed by the kernel's name it resides in.
         */
        DataObjectReporting(const std::string& name)
            : exporter( name ), reports( "Data","The Reported Data of this DataObject")
        {
            nameserver.registerObject( this, name );
            exporter.value().add( &reports );
        }

        virtual ~DataObjectReporting() {
            nameserver.unregisterObject(this);
        }

        /**
         * Sets a new name for this DataObject.
         * This is needed if the kernel gets a new name.
         */
//         void setName( const std::string& name)
//         {
//             nameserver.unregisterObject(this);
//             nameserver.registerObject( this, name );
//             exporter.setName( name );
//         }

        /**
         * Returns the name of this DataObject.
         */
        const std::string& getName()
        {
            return nameserver.getName( this );
        }

        virtual void exportReports(  PropertyBag& bag ) const = 0;
        virtual void refreshReports( PropertyBag& bag ) const = 0;
        virtual void cleanupReports( PropertyBag& bag ) const = 0;
        virtual void inspectReports( PropertyIntrospection* introspector ) const = 0;
        
        PropertyExporter* getExporter()
        {
            return &exporter;
        }

        Property<PropertyBag>* getReports()
        {
            return &reports;
        }

    private :
        /**
         * A Report Server which exports reports in the form of
         * PropertyBags. It is a PropertyBag itself by inheritance,
         * which contains a timestamp already.
         */
        PropertyExporter exporter;

        /**
         * A Bag containing the reports of the dataObject.
         */
        Property<PropertyBag> reports;
    };
}

#endif
