/***************************************************************************
  tag: Peter Soetens  Mon Jan 19 14:11:19 CET 2004  PropertyReporter.hpp 

                        PropertyReporter.hpp -  description
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
 
#ifndef PROPERTYREPORTER
#define PROPERTYREPORTER


#include "Time.hpp"
#include "marshalling/MarshallerAdaptors.hpp"
#include "Property.hpp"
#include "PropertyBag.hpp"
#include "ReportExporterInterface.hpp"
#include "ReportCollectorInterface.hpp"
#include <os/MutexLock.hpp>
#include "RunnableInterface.hpp"
#include "NameServerRegistrator.hpp"
#include "TimeService.hpp"

namespace ORO_CoreLib
{

    /**
     * @brief A Server for retrieving and formatting
     * property based reports.
     *
     * @param MarshallConfig The Marshalling of Header, Data and Footer.
     * @see MarshallConfig
     */
    template < class MarshallConfig >
    class PropertyReporter
        :public RunnableInterface, 
         public PropertyCollectorInterface,
         protected NameServerRegistrator<PropertyReporter<MarshallConfig>*>
    {
    public:
        typedef MarshallConfig Configuration;
        
        /**
         * Create A PropertyReporter with a given marshalling configuration
         * without a name.
         */
        PropertyReporter( MarshallConfig& m )
            : adaptor(m), mytask(0) {}
        
        /**
         * Create A PropertyReporter with a given marshalling configuration
         * and a given name.
         */
        PropertyReporter( MarshallConfig& m, const std::string& name ) :
            NameServerRegistrator<PropertyReporter<MarshallConfig>*>(nameserver, name, this),
        adaptor(m), mytask(0) {}
        
        virtual TaskInterface* getTask() const { return mytask; }

        virtual void setTask( TaskInterface* task ) { mytask = task; }

        virtual bool initialize()
        {
            time = TimeService::Instance()->getTicks();
            refreshAll(0);
            streamHeader();
            return true;
        }

        virtual void step()
        {
            // step is blocking on trigger.
            ORO_OS::MutexLock locker(copy_lock);
            streamData();
        }

        virtual void finalize()
        {
            streamFooter();
        }

        virtual void exporterAdd( PropertyExporterInterface* exp )
        {
            exporters.push_back(exp);
        }
        
        virtual void exporterRemove( PropertyExporterInterface* exp )
        {
            Exporters::iterator it( find(exporters.begin(), exporters.end(), exp ) );
            if ( it != exporters.end() )
                exporters.erase(it);
        }
        
        void streamHeader()
        {
            adaptor.header().serialize(root_bag);
            adaptor.header().flush();
        }
        
        void streamData()
        {
            adaptor.body().serialize(root_bag);
            adaptor.body().flush();
        }

        void streamFooter()
        {
            //adaptor.footer().serialize(root_bag);
            //adaptor.body().flush();
        }

        void refreshAll( TimeService::Seconds timeStamp )
        {
            root_bag.clear();
            for (Exporters::iterator it( exporters.begin() ); it != exporters.end(); ++it )
            {
                (*it)->refresh( timeStamp );
                root_bag.add( &(*it)->reportGet() );
            }
        }

        virtual bool trigger()
        {
            // trigger is non blocking.
            ORO_OS::MutexTryLock locker(copy_lock);
            if ( locker.isSuccessful() )
            {
                refreshAll( TimeService::Instance()->secondsSince(time) );
                return true;
            }
            return false;
        }
        
        virtual void resetTime( TimeService::Seconds s=0)
        {
            time = TimeService::Instance()->getTicks();
            time += TimeService::nsecs2ticks( nsecs(s)*nsecs(1000.0*1000.0*1000.0) );
        }

        static NameServer<PropertyReporter<Configuration>* > nameserver;

    protected:
        typedef std::vector<PropertyExporterInterface*> Exporters;

        /**
         * Stores the root of the hierarchical structure.
         */
        PropertyBag root_bag;

        /**
         * All exporters.
         */
        Exporters exporters;

        /**
         * Marshalling info.
         */
        MarshallConfig& adaptor;

        TaskInterface* mytask;

        /**
         * Timestamp since initialize().
         */
        TimeService::ticks time;

        /**
         * Locking for multi threaded reporting.
         */
        ORO_OS::Mutex copy_lock;
    };

         template <typename T>
         NameServer<PropertyReporter<T>* > PropertyReporter<T>::nameserver;

    /**
     * An example of a PropertyReporting using the standard ostream and
     * XML marshalling to serialize the data.
     *
     * usage :
     * <tt>
     * XMLPropertyReporter::Configuration config(std::cout); // use cout
     * XMLPropertyReporter reporter( config );
     *
     * reporter.initialize();
     * reporter.step();
     * reporter.finalize();
     * </tt>
       
         typedef PropertyReporter<MarshallConfiguration< std::ostream, XMLMarshaller, XMLMarshaller > > XMLPropertyReporter;
    
     */

}

#endif

