#ifndef MOTIONREPORTINGEXTENSION_HPP
#define MOTIONREPORTINGEXTENSION_HPP


#include "KernelInterfaces.hpp"
#include <corelib/PropertyExporter.hpp>
#include <corelib/PropertyReporter.hpp>
#include <corelib/marshalling/MarshallerAdaptors.hpp>

#include <corelib/marshalling/TableMarshaller.hpp>
#include <corelib/marshalling/TableHeaderMarshaller.hpp>
#include <corelib/TaskNonRealTime.hpp>
#include <corelib/PropertyComposition.hpp>

#ifdef OROINT_OS_STDFILEIO
#include <fstream>
#endif

namespace ORO_ControlKernel
{
    using namespace std;

    using namespace ORO_CoreLib;
    class ReportingExtension;
    class BackendReportingExtension;

    /**
     * The base class for reporting components.
     *
     * @see DefaultAspect
     */
  class ReportingComponent
      : public ComponentAspectInterface<ReportingExtension>
        {
	  friend class ORO_ControlKernel::ReportingExtension;
	  friend class ORO_ControlKernel::BackendReportingExtension;

            public:
                /**
                 * Initialize the components name.
                 */
                ReportingComponent(const std::string& _name);

                virtual ~ReportingComponent();

                virtual bool enableAspect(ReportingExtension* ext);

                virtual void disableAspect();

                /**
                 * A user defined method for adding its to be reported
                 * properties to a bag meant for exporting.
                 *
                 * @param bag The bag to which you must add your properties
                 *            which will then get reported.
                 */
                virtual void exportReports( PropertyBag& bag) {}

            private:
                /**
                 * Cleans up the reports.
                 */
                virtual void removeReports()
                {
                    reports.value().clear();
                }
                
                /**
                 * A method for the ReportingExtension to retrieve the
                 * exporter.
                 */
                PropertyExporter* getExporter();

                /**
                 * A method for the ReportingExtension to retrieve the
                 * reports.
                 */
                Property<PropertyBag>* getReports();

                /**
                 * A Report Server which exports reports in the form of
                 * PropertyBags. It is a PropertyBag itself by inheritance.
                 */
                PropertyExporter exporter;

                /**
                 * A Bag containing the reports of the component.
                 */
                Property<PropertyBag> reports;
    
                /**
                 * The client, situated in the Kernel, asking for reports.
                 */
                ReportingExtension* reporter;
        };

    /**
     * An extension for reporting properties to a human readable
     * text format. It can report to a file or to the screen.
     * This extension is purely meant for text based data gathering.
     *
     */
    class ReportingExtension
        :  public ExtensionInterface
    {
        public:
                
            /**
             * The Common Base class for all components.
             */
            typedef ReportingComponent CommonBase;

            /**
             * Set up a control kernel extension for reporting.
             *
             */
            ReportingExtension( KernelBaseFunction* _base=0 )
                : ExtensionInterface("Reporter"), 
                  period("ReportPeriod","in seconds", 0.1 ), 
                  interval("SubSamplingInterval","", 1),
                  repFile("ReportFile","", "report.txt"),
                  toStdOut("WriteToStdOut","", false),
#ifdef OROINT_OS_STDFILEIO
                  toFile("WriteToFile","", false),
#endif
                  reportServer("ReportServer","The name of the report server", "Default"),
                  serverOwner(false), base(_base)
            {
            }

            virtual ~ReportingExtension() {}

            
            /**
             * A class for streaming to two ostream objects.
             */
            struct SplitStream
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

            virtual bool initialize()
            {
#ifdef OROINT_OS_STDFILEIO
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

            virtual void step()
            {
                // copy contents, if possible, on frequency of reporter.
                if ( reporter && serverOwner )
                    reporter->trigger(); // instruct copy.
            }

            virtual void finalize()
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
#ifdef OROINT_OS_STDFILEIO
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
            bool report(const std::string& exporter)
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
            bool unReport( const std::string& exporter)
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
            bool addComponent( ReportingComponent* rep)
            {
                comp_map[ rep->getExporter()->getName() ] = rep;
                return true;
            }

            /**
             * This method removes the component out of the 'scope'
             * of this Extension. If it happend to be reported, it
             * will be unreported.
             */
            void removeComponent( ReportingComponent* rep )
            {
                ExpList::iterator it = find(exporters.begin(), exporters.end(), rep->getExporter() );
                if (it != exporters.end() )
                    exporters.erase(it);
                comp_map.erase( rep->getExporter()->getName() );
            }

            virtual bool updateProperties(const PropertyBag& bag)
            {
                composeProperty(bag, period);
                composeProperty(bag, interval);
                composeProperty(bag, repFile);
                composeProperty(bag, reportServer);
                composeProperty(bag, toStdOut);
#ifdef OROINT_OS_STDFILEIO
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
            
        protected:
            typedef MarshallConfiguration<SplitStream, TableHeaderMarshaller<SplitStream>, TableMarshaller<SplitStream> > MarshallTableType;

#ifdef OROINT_OS_STDFILEIO
            std::ofstream* fileStream;
#endif
            SplitStream*  splitStream;

            MarshallTableType* config;

            /**
             * The reporter for properties.
             */
            PropertyReporter<MarshallTableType>* reporter;
            
            Property<double> period;
            Property<int> interval;
            Property<std::string> repFile;
            Property<bool> toStdOut;
#ifdef OROINT_OS_STDFILEIO
      Property<bool> toFile;
#endif
            Property<std::string> reportServer;

            TaskNonRealTime* reporterTask;
            
            /**
             * A mapping of names and components.
             */
            typedef std::map<std::string, ReportingComponent*> CompMap;
            
            /**
             * This map tells us which component is associated with
             * which name, it are all the components present in the kernel.
             */
            CompMap comp_map;
            
            /**
             * A list of names of all requested component exporters.
             */
            typedef std::vector< std::string > RepList;
            
            /**
             * A list of external exporters.
             */
            typedef std::vector<PropertyExporterInterface*> ExpList;

            /**
             * All external exporters that want to be reported.
             */
            ExpList exporters;
            
            /**
             * All components listed in the property file that may be reported.
             */
            RepList rep_comps;

            /**
             * True if we own the report server.
             */
            bool serverOwner;

            /**
             * The base kernel of this extension.
             */
            KernelBaseFunction* base;
    };

}

#endif
