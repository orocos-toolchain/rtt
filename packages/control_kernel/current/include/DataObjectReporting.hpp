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
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <algorithm>
#include <vector>


#pragma interface

namespace ORO_ControlKernel
{
    using ORO_CoreLib::Property;
    using ORO_CoreLib::PropertyBag;
    using ORO_CoreLib::PropertyExporter;
    using ORO_CoreLib::PropertyIntrospection;

    /**
     * All DataObjectServer which wish to populate the
     * DataObjectReporter with data.
     */
    struct ReportingClient {
        virtual ~ReportingClient();
        virtual void exportReports(  PropertyBag& bag ) const = 0;
        virtual void refreshReports( PropertyBag& bag ) const = 0;
        virtual void cleanupReports( PropertyBag& bag ) const = 0;
        virtual void inspectReports( PropertyIntrospection* introspector ) const = 0;
    };

    /**
     * @brief An interface for gathering reports from DataObjects.
     *
     * It is very similar to the ReportingComponent Interface,
     * but the semantic difference is big enough to motivate
     * a separate interface.
     */
    struct DataObjectReporting
    {
        static ORO_CoreLib::NameServer< boost::shared_ptr<DataObjectReporting> > nameserver;
        std::vector<ReportingClient*> clients;
        /**
         * Create a new DataObject suitable for reporting its contents.
         
         * @param name is the globally visible name of this DataObject.
         *  It is usually prefixed by the kernel's name it resides in.
         */
        DataObjectReporting(const std::string& name);

        ~DataObjectReporting();

        void addClient( ReportingClient* c);
        void removeClient( ReportingClient* c);

        void exportReports( PropertyBag& bag ) const {
            std::for_each(clients.begin(), clients.end(), boost::bind(&ReportingClient::exportReports,_1, boost::ref(bag)) );
        }
        void refreshReports( PropertyBag& bag ) const {
            std::for_each(clients.begin(), clients.end(), boost::bind(&ReportingClient::refreshReports,_1, boost::ref(bag)) );
        }
        void cleanupReports( PropertyBag& bag ) const {
            std::for_each(clients.begin(), clients.end(), boost::bind(&ReportingClient::cleanupReports,_1, boost::ref(bag)) );
        }
        void inspectReports( PropertyIntrospection* introspector ) const {
            std::for_each(clients.begin(), clients.end(), boost::bind(&ReportingClient::inspectReports,_1, introspector) );
        }

        /**
         * Returns the name of this DataObject.
         */
        const std::string& getName() const;

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
