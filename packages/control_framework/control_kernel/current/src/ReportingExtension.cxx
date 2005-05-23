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

#ifdef ORO_PRAGMA_INTERFACE
#pragma implementation
#endif

#include <pkgconf/system.h>
#include "control_kernel/ReportingExtension.hpp"
#include <corelib/PropertyComposition.hpp>
#include <corelib/Logger.hpp>
#ifdef OROPKG_CONTROL_KERNEL_EXTENSIONS_EXECUTION
#include <execution/TemplateFactories.hpp>
#endif

namespace ORO_ControlKernel
{
    using namespace detail;

    ReportingComponent::ReportingComponent(const std::string& _name) 
        : detail::ComponentFacetInterface<ReportingExtension>( _name ),
          exporter( _name ), reports("Data","The Reported Data of this Component"), reporter(0)
    {
        exporter.value().add( &reports );
    }

    ReportingComponent::~ReportingComponent()
    {
        exporter.value().remove( &reports );
    }

    bool ReportingComponent::enableFacet(ReportingExtension* ext)
    {
        if (ext == 0)
            return false;
        reporter = ext;
        return reporter->addComponent(this);
    }

    void ReportingComponent::disableFacet()
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

    ReportingExtension::ReportingExtension( ControlKernelInterface* _base )
        : detail::ExtensionInterface(_base, "Reporter"),
          splitStream(0), config(0), nh_config(0),
          reporter(0),
          period("ReportPeriod","in seconds", 0.1 ), 
          interval("SubSamplingInterval","", 1),
          repFile("ReportFile","", "report.txt"),
          toStdOut("WriteToStdOut","", false),
          writeHeader("WriteHeader","", true),
#ifdef OROINT_OS_STDIOSTREAM
          toFile("WriteToFile","", false),
#endif
          reportServer("ReportServer","The name of the report server", _base->getKernelName() ),
          autostart("AutoStart","Start reporting automatically.", true),
          reporterTask(0), serverOwner(false), count( 0 ),
          base(_base)
    {
    }

    ReportingExtension::~ReportingExtension() {
        if (serverOwner) {
            // only cleanup upon destruction, if this reportserver is still used, the application
            // will crash.
            PropertyReporter<NoHeaderMarshallTableType>::nameserver.unregisterName( reportServer );
            PropertyReporter<MarshallTableType>::nameserver.unregisterName( reportServer );
            delete reporterTask;
            delete reporter;
            delete config;
            delete nh_config;
            delete splitStream;
#ifdef OROINT_OS_STDIOSTREAM
            if ( toFile )
                {
                    delete fileStream;
                }
#endif
        }
    }

//     TaskInterface* ReportingExtension::getTask() const
//     {
//         return base->getTask();
//     }
    
//     void ReportingExtension::setTask( TaskInterface* task )
//     {
//         base->setTask( task );
//     }

    bool ReportingExtension::initialize()
    {
        // check for existing servers
        reporter = PropertyReporter<MarshallTableType>::nameserver.getObject( reportServer );
        if ( !reporter )
            {
                reporter = PropertyReporter<NoHeaderMarshallTableType>::nameserver.getObject( reportServer );
                if ( !reporter )
                    {
                        // no server found, create it.
#ifdef OROINT_OS_STDIOSTREAM
                        if ( toFile && toStdOut )
                            {
                                Logger::log() <<Logger::Info << "ReportingExtension : ";
                                Logger::log() <<Logger::Info << "Reporting to file "<<repFile<<" and std output."<< Logger::endl;
                                fileStream = new ofstream( repFile.get().c_str() );
                                splitStream = new SplitStream( fileStream, &std::cout);
                            } else
                                if ( toFile )
                                    {
                                        Logger::log() <<Logger::Info << "ReportingExtension : ";
                                        Logger::log() <<Logger::Info << "Reporting to file "<< repFile <<"."<< Logger::endl;
                                        fileStream = new ofstream( repFile.get().c_str() );
                                        splitStream = new SplitStream(fileStream);
                                    } else
#endif
                                        if (toStdOut)
                                            {
                                                Logger::log() <<Logger::Info << "ReportingExtension : ";
                                                Logger::log() <<Logger::Info << "Reporting to std output."<< Logger::endl;
                                                splitStream = new SplitStream( &std::cout );
                                            } else
                                                {
                                                    Logger::log() <<Logger::Info << "ReportingExtension : ";
                                                    Logger::log() <<Logger::Info << "Not Reporting."<< Logger::endl;
                                                    // do nothing.
                                                    return true;
                                                }
                    
                        if ( writeHeader )
                            config = new MarshallTableType( *splitStream );
                        else
                            nh_config = new NoHeaderMarshallTableType( *splitStream );


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
        if ( serverOwner == false ) {
            Logger::log() << Logger::Info << "ReportingExtension : "
                          << "Re-using running ReportServer \""
                          << reportServer <<"\". Properties for Reportin of this Kernel have no effect !"<< Logger::endl;
        }
        // from here on, we got a pointer to 'reporter' being it our own or a remote server.
                
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
                else 
                    Logger::log() <<Logger::Error << "ReportingExtension : "
                                  << *it << " not found !"<< Logger::endl;
            }

        // iterate over all xml-listed dataobjects :
        for ( RepList::iterator it = rep_dos.begin(); it != rep_dos.end(); ++it )
            {
                unsigned int pos;
                // see if xml-listed data_server is present in this kernel :
                boost::shared_ptr<DataObjectReporting>
                    do_server(DataObjectReporting::nameserver.getObject( base->getKernelName() + "::" + *it ));
                //std::cerr << "Looking up " << *it;
                if ( do_server.get() )
                    {
                        //std::cerr <<" found as '" + base->getKernelName() + "::" + *it+"'";
                        // The user gave a "DataObject" identifier
                        // Check for double registration
                        if ( find( active_dos.begin(), active_dos.end(), do_server) == active_dos.end() )
                            {
                                //std::cerr <<" first time, thus adding ";
                                active_dos.push_back( do_server );
                                reporter->exporterAdd( do_server->getExporter() );
                            }
                        else
                            {
                                //std::cerr <<" already present in reporter, cleanup bag ";
                                // This server exists. Request it to export all reports,
                                // but remove the old entry, whatever it was.
                                do_server->reportNone();
                            }
                        //std::cerr <<" exporting Reports to bag "<<std::endl;
                        do_server->reportAll();
                    }
                else if ( (pos = (*it).find("::")) != std::string::npos ) {
                    // extract the server's name :
                    do_server =
                        DataObjectReporting::nameserver.getObject( base->getKernelName()
                                                                   +"::" + std::string( *it, 0, pos ) );
                    if ( do_server.get() )
                        {
                            //std::cerr <<" found as '" + base->getKernelName()+"::" + std::string( *it, 0, pos )<<"'";
                            //PropertyBag tmp_bag;
                            //PropertyBase* new_item;
                            // The user gave a "DataObject::DataName" identifier, we need to add one item.
                            // check if it is the first registration...
                            if ( find( active_dos.begin(), active_dos.end(), do_server) == active_dos.end() )
                                {
                                    //std::cerr <<" first time, thus adding ";
                                    active_dos.push_back( do_server );
                                    reporter->exporterAdd( do_server->getExporter() );
                                }
                            
                            //std::cerr <<" checking if '"+ std::string( *it, pos+2 )+"' is present in DataObject : "<<endl;
                            // if the DataName is in the server, but not in the reports yet, add it to the reports.
                            if ( do_server->reportItem( std::string( *it, pos+2 ) ) == false ) {
                                Logger::log() <<Logger::Error << "ReportingExtension : "
                                              << " Looking up '"<< *it <<"' : "
                                              << std::string( *it, pos+2 ) << " not found in "<< do_server->getName() <<"."<< Logger::endl;
                            }

                            //do_server->cleanupReports( tmp_bag );
                        }
                }
                else {
                    Logger::log() << Logger::Error << "ReportingExtension : Looking up '"<< *it <<"' : not found !" << Logger::nl
                                  <<"Tried " + base->getKernelName()+"::" + std::string( *it, 0, pos ) << Logger::endl;
                }
            }
        if (serverOwner && autostart.get() )
            reporterTask->start();
        Logger::log() <<Logger::Debug << "ReportingExtension successfully initialized()."<< Logger::endl;
        return true;
    }

    void ReportingExtension::step()
    {
        // copy contents, if possible, on frequency of reporter.
        for ( DosList::iterator it = active_dos.begin(); it != active_dos.end(); ++it )
            (*it)->refresh();

        //(*it)->refreshReports( (*it)->getReports()->value() );

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
                // only stop the reporting, cleanup reporting infrastructure
                // in the destructor, since other kernels may be using it too.
                if (serverOwner) {
                    reporterTask->stop();
                }
                for ( ExpList::iterator it = exporters.begin(); it != exporters.end(); ++it )
                    reporter->exporterRemove( *it );
                for ( CompMap::iterator it = comp_map.begin(); it != comp_map.end(); ++it )
                    {
                        reporter->exporterRemove( it->second->getExporter() );
                        it->second->removeReports();
                    }
                for ( DosList::iterator it = active_dos.begin(); it != active_dos.end(); ++it )
                    {
                        (*it)->reportNone();
                        reporter->exporterRemove( (*it)->getExporter() );
                    }
                if ( serverOwner && reporter->nbOfExporters() == 0 ) // safe to cleanup
                    {
                        Logger::log() << Logger::Info << "ReportingExtension : "
                                      << "No more clients present, cleaning up ReportServer \""
                                      << reportServer <<"\"."<< Logger::endl;
                        PropertyReporter<NoHeaderMarshallTableType>::nameserver.unregisterName( reportServer );
                        PropertyReporter<MarshallTableType>::nameserver.unregisterName( reportServer );
                        delete reporterTask;
                        delete reporter;
                        delete config;
                        delete nh_config;
                        delete splitStream;
                        reporterTask = 0;
                        reporter = 0;
                        config = 0;
                        nh_config = 0;
                        splitStream = 0;
#ifdef OROINT_OS_STDIOSTREAM
                        if ( toFile )
                            {
                                delete fileStream;
                                fileStream = 0;
                            }
#endif
                        serverOwner = false;
                    }
                else
                    Logger::log() << Logger::Info << "ReportingExtension : "
                                  << "Other clients present in ReportServer \""
                                  << reportServer <<"\". Not cleaning up."<< Logger::endl;


            }
        active_dos.clear();
    }

#ifdef OROPKG_CONTROL_KERNEL_EXTENSIONS_EXECUTION
    using namespace ORO_Execution;

    bool ReportingExtension::exportProperties(AttributeRepository& bag)
    {
        return bag.addProperty( &period) &&
            bag.addProperty( &interval) &&
            bag.addProperty( &repFile) &&
            bag.addProperty( &reportServer) &&
            bag.addProperty( &toStdOut) &&
            bag.addProperty( &writeHeader) &&
#ifdef OROINT_OS_STDIOSTREAM
            bag.addProperty( &toFile) &&
#endif
            bag.addProperty( &autostart);
    }

    MethodFactoryInterface* ReportingExtension::createMethodFactory()
    {
        // Add the methods, methods make sure that they are 
        // executed in the context of the (non realtime) caller.
        TemplateMethodFactory< ReportingExtension  >* ret =
            newMethodFactory( this );
        ret->add( "start",
                  method
                  ( &ReportingExtension::startReporting ,
                    "Start reporting.") );
        ret->add( "stop",
                  method
                  ( &ReportingExtension::stopReporting ,
                    "Stop reporting.") );
        ret->add( "reportComponent",
                  method
                  ( &ReportingExtension::reportComponent ,
                    "Add a Component for reporting. Only works if Component exists and Kernel is not running.",
                    "Component", "Name of the Component") );
        ret->add( "reportDataObject",
                  method
                  ( &ReportingExtension::reportDataObject ,
                    "Add a DataObject for reporting. Only works if DataObject exists and Kernel is not running.",
                    "DataObject", "Name of the DataObject. For example, 'Inputs' or 'Inputs::ChannelValues'.") );
        ret->add( "unreportComponent",
                  method
                  ( &ReportingExtension::unreportComponent ,
                    "Remove a Component for reporting. Only works if Component exists and Kernel is not running.",
                    "Component", "Name of the Component") );
        ret->add( "reportDataObject",
                  method
                  ( &ReportingExtension::unreportDataObject ,
                    "Remove a DataObject for reporting. Only works if DataObject exists and Kernel is not running.",
                    "DataObject", "Name of the DataObject. For example, 'Inputs' or 'Inputs::ChannelValues'.") );
        return ret;
    }
#endif

    bool ReportingExtension::startReporting()
    {
        return reporterTask->start();
    }

    bool ReportingExtension::stopReporting()
    {
        reporterTask->stop();
        return true;
    }

    /**
     * Report an external PropertyExporterInterface.
     * You can use this method to add additional exporters during runtime.
     */
    bool ReportingExtension::report(const std::string& exporter)
    {
        PropertyExporterInterface* exp = PropertyExporterInterface::nameserver.getObject( exporter );
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
        PropertyExporterInterface* exp = PropertyExporterInterface::nameserver.getObject( exporter );
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

    bool ReportingExtension::reportComponent( const std::string& comp )
    {
        if ( comp_map.count( comp ) == 0 || (kernel()->getTask() && kernel()->getTask()->isRunning()) )
            return false;
        rep_comps.push_back( comp );
        return true;
    }

    bool ReportingExtension::reportDataObject( const std::string& dob )
    {
        if ( kernel()->getTask() && kernel()->getTask()->isRunning() )
            return false;
        rep_dos.push_back( dob );
        return true;
    }

    bool ReportingExtension::unreportDataObject( const std::string& dob )
    {
        if ( kernel()->getTask() && kernel()->getTask()->isRunning() )
            return false;
        RepList::iterator it = find( rep_dos.begin(), rep_dos.end(), dob );
        if ( it != rep_dos.end() ) {
            rep_dos.erase( it );
            return true;
        }
        return false;
    }

    bool ReportingExtension::unreportComponent( const std::string& comp )
    {
        if ( kernel()->getTask() && kernel()->getTask()->isRunning() )
            return false;
        RepList::iterator it = find( rep_comps.begin(), rep_comps.end(), comp );
        if ( it != rep_comps.end() ) {
            rep_comps.erase( it );
            return true;
        }
        return false;
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
        composeProperty(bag, autostart);

        Logger::log() << Logger::Info << "ReportingExtension Properties : "<<Logger::nl
                      << period.getName()<< " : " << period.get() << Logger::nl
                      << interval.getName()<< " : " << interval.get() << Logger::nl
                      << repFile.getName()<< " : " << repFile.get() << Logger::nl
                      << reportServer.getName()<< " : " << reportServer.get() << Logger::nl
                      << toStdOut.getName()<< " : " << toStdOut.get() << Logger::nl
                      << writeHeader.getName()<< " : " << writeHeader.get() << Logger::nl
                      << toFile.getName()<< " : " << toFile.get() << Logger::nl
                      << autostart.getName()<< " : " << autostart.get() << Logger::endl;
                
        // Reporting exporters, look them up by name.
        PropertyBase* exportbase = bag.find("Exporters");
        Property<PropertyBag>* res;
        if ( exportbase && (res = dynamic_cast<Property<PropertyBag>* >(exportbase) ))
            {
                // a bit harsh, yeah...
                exporters.clear();
                rep_comps.clear();
                rep_dos.clear();
                
                PropertyBag::const_iterator it = res->value().getProperties().begin();
                while ( it != res->value().getProperties().end() )
                    {
                        Property<std::string>* compName = dynamic_cast<Property<std::string>* >( *it );
                        if ( !compName )
                            Logger::log() << Logger::Error << "ReportingExtension : "
                                          << "Expected property \""
                                          << (*it)->getName() <<"\" to be of type string."<< Logger::endl;
                        else if ( compName->getName() == "Component" )
                            rep_comps.push_back( compName->value() );
                        else if ( compName->getName() == "DataObject" )
                            rep_dos.push_back( compName->value() );
                        else
                            Logger::log() << Logger::Error << "ReportingExtension : "
                                          << "Expected \"Component\" Or \"DataObject\", got "
                                          << compName->getName() << Logger::endl;
                        ++it;
                    }
            }
        return interval > 0;
    }
            
}
