
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
#ifdef OROINT_OS_STDIOSTREAM
          toFile("WriteToFile","", false),
#endif
          reportServer("ReportServer","The name of the report server", "Default"),
          serverOwner(false), base(_base)
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
                                    config = 0;
                                    splitStream = 0;
                                    reporter = 0;
                                    reporterTask = 0;
                                    return true;
                                }
                    
        config = new MarshallTableType( *splitStream );
        // check for existing server
        reporter = PropertyReporter<MarshallTableType>::nameserver.getObjectByName( reportServer );
        if ( !reporter )
            {
                // create a new report server with this name.
                reporter = new PropertyReporter<MarshallTableType>( *config, reportServer );
                reporterTask = new TaskNonRealTime(period, reporter);
                serverOwner = true;
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
                        ic->second->exportReports( 
                                                  ic->second->getReports()->value() );
                        reporter->exporterAdd( ic->second->getExporter() );
                    }
            }
                
        if (serverOwner)
            reporterTask->start();
        return true;
    }

    void ReportingExtension::step()
    {
        // copy contents, if possible, on frequency of reporter.
        if ( reporter && serverOwner )
            reporter->trigger(); // instruct copy.
    }

    void ReportingExtension::finalize()
    {
                
        // stop reporter.
        if (reporter && serverOwner )
            {
                reporterTask->stop();
                for ( ExpList::iterator it = exporters.begin(); it != exporters.end(); ++it )
                    reporter->exporterRemove( *it );
                for ( CompMap::iterator it = comp_map.begin(); it != comp_map.end(); ++it )
                    {
                        reporter->exporterRemove( it->second->getExporter() );
                        it->second->removeReports();
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
        if ( splitStream )
            delete splitStream;
        if ( config )
            delete config;
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
#ifdef OROINT_OS_STDIOSTREAM
        composeProperty(bag, toFile);
#endif
                
        // a bit harsh, yeah...
        exporters.clear();
        rep_comps.clear();
                
        // Reporting exporters, look them up by name.
        PropertyBase* exportbase = bag.find("Exporters");
        Property<PropertyBag>* res;
        if ( exportbase && (res = dynamic_cast<Property<PropertyBag>* >(exportbase) ))
            {
                PropertyBag::const_iterator it = res->value().getProperties().begin();
                while ( it != res->value().getProperties().end() )
                    {
                        Property<std::string>* compName = dynamic_cast<Property<std::string>* >( *it );
                        if ( compName && compName->getName() == "Component" )
                            rep_comps.push_back( compName->value() );
                        ++it;
                    }
            }
        return true;
    }
            
}
