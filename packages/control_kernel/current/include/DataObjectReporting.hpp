/***************************************************************************
  tag: Peter Soetens  Thu Apr 22 20:41:00 CEST 2004  DataObjectReporting.hpp 

                        DataObjectReporting.hpp -  description
                           -------------------
    begin                : Thu April 22 2004
    copyright            : (C) 2004 Peter Soetens
    email                : peter.soetens@mech.kuleuven.ac.be
 
 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
 
 
 
#ifndef DATAOBJECTREPORTING_HPP
#define DATAOBJECTREPORTING_HPP

#include <corelib/NameServer.hpp>
#include <corelib/PropertyExporter.hpp>
#include <corelib/PropertyReporter.hpp>
#include <corelib/PropertyBagIntrospector.hpp>

namespace ORO_ControlKernel
{

    /**
     * @brief An interface for gathering reports from DataObjects.
     *
     * It is very similar to the ReportingComponent Interface,
     * but the semantic difference is big enough to motivate
     * a separate interface.
     */
    struct DataObjectReporting
    {
        static NameServer<DataObjectReporting*> nameserver;
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
