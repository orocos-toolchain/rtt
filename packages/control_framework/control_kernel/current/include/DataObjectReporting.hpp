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
    namespace detail {
    using ORO_CoreLib::Property;
    using ORO_CoreLib::PropertyBase;
    using ORO_CoreLib::PropertyBag;
    using ORO_CoreLib::PropertyExporter;
    using ORO_CoreLib::PropertyIntrospection;

    /**
     * All DataObjectServer which wish to populate the
     * DataObjectReporter with data.
     */
    struct ReportingClient {
        virtual ~ReportingClient();
        virtual void reportAll(std::vector<ORO_CoreLib::CommandInterface*>& c,  PropertyBag& bag ) const = 0;
        virtual std::pair<PropertyBase*,ORO_CoreLib::CommandInterface*> createItem( const std::string name ) const = 0;
        virtual void inspectReports( PropertyIntrospection* introspector ) const = 0;
    };

    /**
     * @brief An interface for gathering reports from DataObjects.
     * And acts as a map for a type of kernel DataObjects,
     * serving them as DataSources. For example MyKernel::Inputs.
     */
    struct DataObjectReporting
        : public std::map<std::string, ORO_CoreLib::DataSourceBase::shared_ptr>
    {
        static ORO_CoreLib::NameServer< boost::shared_ptr<DataObjectReporting> > nameserver;
        std::vector<ReportingClient*> clients;
        /**
         * Create a new DataObject suitable for reporting its contents.
         
         * @param name is the globally visible name of this DataObject.
         *  It is usually prefixed by the kernel's name it resides in.
         * for example : MyKernel::Inputs.
         */
        DataObjectReporting(const std::string& name);

        ~DataObjectReporting();

        void addClient( ReportingClient* c);
        void removeClient( ReportingClient* c);

        void inspectReports( PropertyIntrospection* introspector ) const {
            std::for_each(clients.begin(), clients.end(), boost::bind(&ReportingClient::inspectReports,_1, introspector) );
        }

        void refresh() {
            for( std::vector<ORO_CoreLib::CommandInterface*>::iterator it = comms.begin(); it != comms.end(); ++it )
                (*it)->execute();
        }

        void reportAll()
        {
            std::for_each(clients.begin(), clients.end(), boost::bind(&ReportingClient::reportAll,_1, boost::ref(comms), boost::ref(reports.value())) );
        }
        
        void reportNone()
        {
            flattenPropertyBag( reports.value() );
            deleteProperties( reports.value() );
            for( std::vector<ORO_CoreLib::CommandInterface*>::iterator it = comms.begin(); it!= comms.end(); ++it )
                delete *it;
            comms.clear();
        }

        bool reportItem( const std::string& name ) {
            // name is already stripped from 'kernelname::DataObject'
            // check if 'I' am not already reported, then do not add sub-item. 
            unsigned int pos = this->getName().find("::");
            if( reports.value().find( std::string(this->getName(), pos+2) ) != 0)
                return true;

            for(std::vector<ReportingClient*>::iterator it = clients.begin(); it!= clients.end(); ++it) {
                std::pair<PropertyBase*,ORO_CoreLib::CommandInterface*> item = (*it)->createItem( name );
                if ( item.first != 0 && item.second != 0 ) {
                        reports.value().add( item.first );
                        comms.push_back( item.second);
                        return true;
                }
            }
            return false;
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

        std::vector<ORO_CoreLib::CommandInterface*>* getCommands()
        {
            return &comms;
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

        /**
         * These commands update the contents of the PropertyBag.
         */
        std::vector<ORO_CoreLib::CommandInterface*> comms;
    };
    }
}

#endif
