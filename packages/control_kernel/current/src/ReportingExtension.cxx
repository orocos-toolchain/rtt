/***************************************************************************
  tag: Peter Soetens  Mon May 10 19:10:38 CEST 2004  ReportingExtension.cxx 

                        ReportingExtension.cxx -  description
                           -------------------
    begin                : Mon May 10 2004
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

#include "control_kernel/ReportingExtension.hpp"
#include <corelib/PropertyComposition.hpp>

namespace ORO_ControlKernel
{

    ReportingComponent::ReportingComponent(const std::string& _name) 
        : detail::ComponentAspectInterface<ReportingExtension>( _name ),
          exporter( _name ), reports("Data","The Reported Data of this Component"), reporter(0)
    {
        exporter.value().add( &reports );
    }

    ReportingComponent::~ReportingComponent()
    {
        exporter.value().remove( &reports );
    }

    bool ReportingComponent::enableAspect(ReportingExtension* ext)
    {
        if (ext == 0)
            return false;
        reporter = ext;
        return reporter->addComponent(this);
    }

    void ReportingComponent::disableAspect()
    {
        if (reporter != 0)
            {
                reporter->removeComponent(this);
                reporter = 0;
            }
    }

    PropertyExporter* ReportingComponent::getExporter()
    {
        return &exporter;
    }

    Property<PropertyBag>* ReportingComponent::getReports()
    {
        return &reports;
    }

    ReportingExtension::ReportingExtension( KernelBaseFunction* _base )
        : detail::ExtensionInterface("Reporter"),
          period("ReportPeriod","in seconds", 0.1 ), 
          interval("SubSamplingInterval","", 1),
          repFile("ReportFile","", "report.txt"),
          toStdOut("WriteToStdOut","", false),
          writeHeader("WriteHeader","", true),
#ifdef OROINT_OS_STDIOSTREAM
          toFile("WriteToFile","", false),
#endif
          reportServer("ReportServer","The name of the report server", "Default"),
          serverOwner(false), count( 0 ),
          base(_base)
    {
    }

    ReportingExtension::~ReportingExtension() {}

    TaskInterface* ReportingExtension::getTask() const
    {
        return base->getTask();
    }
    
    void ReportingExtension::setTask( TaskInterface* task )
    {
        base->setTask( task );
    }

            
    /**
     * A class for streaming to two ostream objects.
     */
    struct ReportingExtension::SplitStream
    {
        SplitStream(std::ostream* _f, std::ostream* _s = 0) : f(_f), s(_s) {}
                
        template< class V >
        SplitStream& operator<<(const V& v)
        {
            *f << v;
            if (s)
                *s << v;
            return *this;
        }
        SplitStream& operator<<( std::ostream& (*func) (std::ostream&) )
        {
            (*func)( *f);
            if (s)
                (*func)(*s);
            return *this;
        }

        int width( int w)
        {
            if (s)
                s->width(w);
            return f->width(w);
        }
                
        std::ostream* f;
        std::ostream* s;
    };

    bool ReportingExtension::initialize()
    {
        splitStream =0; config =0; nh_config = 0;
        reporter = 0; reporterTask = 0;
#ifdef OROINT_OS_STDIOSTREAM
        if ( toFile && toStdOut )
            {
                fileStream = new ofstream( repFile.get().c_str() );
                splitStream = new SplitStream( fileStream, &std::cout);
            } else
                if ( toFile )
                    {
                        fileStream = new ofstream( repFile.get().c_str() );
                        splitStream = new SplitStream(fileStream);
                    } else
#endif
                        if (toStdOut)
                            {
                                splitStream = new SplitStream( &std::cout );
                            } else
                                {
                                    // do nothing.
                                    return true;
                                }
                    
        if ( writeHeader )
            config = new MarshallTableType( *splitStream );
        else
            nh_config = new NoHeaderMarshallTableType( *splitStream );

        // check for existing servers
        reporter = PropertyReporter<MarshallTableType>::nameserver.getObjectByName( reportServer );
        if ( !reporter )
            {
                reporter = PropertyReporter<NoHeaderMarshallTableType>::nameserver.getObjectByName( reportServer );
                if ( !reporter )
                    {
                        RunnableInterface* tmp_run;
                        if ( writeHeader )
                            {
                                PropertyReporter<MarshallTableType>* tmp_ptr;
                                tmp_ptr = new PropertyReporter<MarshallTableType>( *config, reportServer );
                                tmp_run = tmp_ptr;
                                reporter = tmp_ptr;
                            }
                        else
                            {
                                PropertyReporter<NoHeaderMarshallTableType>* tmp_ptr;
                                tmp_ptr = new PropertyReporter<NoHeaderMarshallTableType>( *nh_config, reportServer );
                                tmp_run = tmp_ptr;
                                reporter = tmp_ptr;
                            }
                        reporterTask = new TaskNonRealTime(period, tmp_run);
                        serverOwner = true;
                    }
            }
                
        // iterate over all external exporters :
        for ( ExpList::iterator it = exporters.begin(); it != exporters.end(); ++it )
            reporter->exporterAdd( *it );
                
        // iterate over all xml-listed components :
        for ( RepList::iterator it = rep_comps.begin(); it != rep_comps.end(); ++it )
            {
                // see if xml-listed component is present in kernel :
                CompMap::iterator ic = comp_map.find( *it );
                if ( ic != comp_map.end() )
                    {
                        // The component is in our map, add to reporting.
                        ic->second->exportReports( ic->second->getReports()->value() );
                        reporter->exporterAdd( ic->second->getExporter() );
                    }
            }

        // iterate over all xml-listed dataobjects :
        for ( RepList::iterator it = rep_dos.begin(); it != rep_dos.end(); ++it )
            {
                unsigned int pos;
                // see if xml-listed data_server is present in this kernel :
                DataObjectReporting* do_server;
                if ( (do_server = DataObjectReporting::nameserver.getObject( base->getKernelName() + "::" + *it ) ) )
                    {
                        // The user gave a "DataObject" identifier
                        // Check for double registration
                        if ( find( active_dos.begin(), active_dos.end(), do_server) == active_dos.end() )
                            {
                                active_dos.push_back( do_server );
                                reporter->exporterAdd( do_server->getExporter() );
                            }
                        else
                            {
                                // This server exists. Request it to export all reports,
                                // but remove the old entry, whatever it was.
                                do_server->cleanupReports( do_server->getReports()->value() );
                            }
                        do_server->exportReports( do_server->getReports()->value() );
                    }
                else if ( (pos = (*it).find("::")) != std::string::npos && (do_server =
                          DataObjectReporting::nameserver.getObject( base->getKernelName()
                                                                     +"::" + std::string( *it, 0, pos ) )) )
                    {
                        PropertyBag tmp_bag;
                        PropertyBase* new_item;
                        // The user gave a "DataObject::DataName" identifier, we need to add one item.
                        // check if it is the first registration...
                        if ( find( active_dos.begin(), active_dos.end(), do_server) == active_dos.end() )
                            {
                                active_dos.push_back( do_server );
                                reporter->exporterAdd( do_server->getExporter() );
                            }
                        do_server->exportReports( tmp_bag );

                        // if the DataName is in the server, but not in the reports yet, add it to the reports.
                        if ( (new_item = tmp_bag.find( std::string( *it, pos+2 ) ) ) &&
                             do_server->getReports()->value().find( std::string( *it, 0, pos+2 ) ) == 0)
                            do_server->getReports()->value().add( new_item->clone() );

                        do_server->cleanupReports( tmp_bag );
                    }
            }
        if (serverOwner)
            reporterTask->start();
        return true;
    }

    void ReportingExtension::step()
    {
        // copy contents, if possible, on frequency of reporter.
        for ( DosList::iterator it = active_dos.begin(); it != active_dos.end(); ++it )
            (*it)->refreshReports( (*it)->getReports()->value() );

        if ( reporter && serverOwner )
            {
                if ( count % interval == 0 )
                    {
                        reporter->trigger(); // instruct copy.
                        count = 0;
                    }
                ++count;
            }
    }

    void ReportingExtension::finalize()
    {
                
        // stop reporter.
        if (reporter )
            {
                if (serverOwner)
                    reporterTask->stop();
                for ( ExpList::iterator it = exporters.begin(); it != exporters.end(); ++it )
                    reporter->exporterRemove( *it );
                for ( CompMap::iterator it = comp_map.begin(); it != comp_map.end(); ++it )
                    {
                        reporter->exporterRemove( it->second->getExporter() );
                        it->second->removeReports();
                    }
                for ( DosList::iterator it = active_dos.begin(); it != active_dos.end(); ++it )
                    {
                        (*it)->cleanupReports( (*it)->getReports()->value() );
                        reporter->exporterRemove( (*it)->getExporter() );
                    }
                delete reporterTask;
                delete reporter;
            }
#ifdef OROINT_OS_STDIOSTREAM
        if ( toFile )
            {
                delete fileStream;
            }
#endif
        delete splitStream;
        delete config;
        delete nh_config;

        active_dos.clear();
    }

    /**
     * Report an external PropertyExporterInterface.
     * You can use this method to add additional exporters during runtime.
     */
    bool ReportingExtension::report(const std::string& exporter)
    {
        PropertyExporterInterface* exp = PropertyExporterInterface::nameserver.getObjectByName( exporter );
        if (exp != 0)
            {
                exporters.push_back(exp);
                return true;
            }
        return false;
    }

    /**
     * Remove an external PropertyExporterrInterface from reporting.
     * You can use this method to remove a previously added
     * exporter during runtime.
     */
    bool ReportingExtension::unReport( const std::string& exporter)
    {
        PropertyExporterInterface* exp = PropertyExporterInterface::nameserver.getObjectByName( exporter );
        if (exp != 0)
            {
                ExpList::iterator it = find(exporters.begin(), exporters.end(), exp);
                if (it != exporters.end() )
                    exporters.erase(it);
                return true;
            }
        return false;
    }

    /**
     * This method registers a component to this Extension.
     * It does not cause the component to be reported yet.
     * This has to be done through configuration.
     */
    bool ReportingExtension::addComponent( ReportingComponent* rep)
    {
        comp_map[ rep->getExporter()->getName() ] = rep;
        return true;
    }

    /**
     * This method removes the component out of the 'scope'
     * of this Extension. If it happend to be reported, it
     * will be unreported.
     */
    void ReportingExtension::removeComponent( ReportingComponent* rep )
    {
        ExpList::iterator it = find(exporters.begin(), exporters.end(), rep->getExporter() );
        if (it != exporters.end() )
            exporters.erase(it);
        comp_map.erase( rep->getExporter()->getName() );
    }

    bool ReportingExtension::updateProperties(const PropertyBag& bag)
    {
        composeProperty(bag, period);
        composeProperty(bag, interval);
        composeProperty(bag, repFile);
        composeProperty(bag, reportServer);
        composeProperty(bag, toStdOut);
        composeProperty(bag, writeHeader);
#ifdef OROINT_OS_STDIOSTREAM
        composeProperty(bag, toFile);
#endif
                
        // a bit harsh, yeah...
        exporters.clear();
        rep_comps.clear();
        rep_dos.clear();
                
        // Reporting exporters, look them up by name.
        PropertyBase* exportbase = bag.find("Exporters");
        Property<PropertyBag>* res;
        if ( exportbase && (res = dynamic_cast<Property<PropertyBag>* >(exportbase) ))
            {
                PropertyBag::const_iterator it = res->value().getProperties().begin();
                while ( it != res->value().getProperties().end() )
                    {
                        Property<std::string>* compName = dynamic_cast<Property<std::string>* >( *it );
                        if ( !compName )
                            std::cout << "[Reporting] Warning : Expected property \""<<
                                (*it)->getName() <<"\" to be of type string."<<endl;
                        else if ( compName->getName() == "Component" )
                            rep_comps.push_back( compName->value() );
                        else if ( compName->getName() == "DataObject" )
                            rep_dos.push_back( compName->value() );
                        else
                            std::cout << "[Reporting] Warning : Expected \"Component\" Or \"DataObject\", got "<<
                                compName->getName() << endl;
                        ++it;
                    }
            }
        return interval > 0;
    }
            
}
