#ifndef MOTIONREPORTINGEXTENSION_HPP
#define MOTIONREPORTINGEXTENSION_HPP


#include "KernelInterfaces.hpp"
#include <corelib/PropertyExporter.hpp>
#include <corelib/PropertyReporter.hpp>
#include <corelib/marshalling/MarshallerAdaptors.hpp>

#include <corelib/marshalling/TableMarshaller.hpp>
#include <corelib/marshalling/TableHeaderMarshaller.hpp>
#include <corelib/TaskNonRealTime.hpp>

#include <pkgconf/os.h>
#ifdef OROINT_OS_STDIOSTREAM
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
     * @see ReportingExtension
     * @see DefaultAspect
     */
    class ReportingComponent
        : public detail::ComponentAspectInterface<ReportingExtension>
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
        :  public detail::ExtensionInterface
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
        ReportingExtension( KernelBaseFunction* _base=0 );

        virtual ~ReportingExtension();

            
        /**
         * A class for streaming to two ostream objects.
         */
        struct SplitStream;

        virtual bool initialize();

        virtual void step();

        virtual void finalize();

        /**
         * Report an external PropertyExporterInterface.
         * You can use this method to add additional exporters during runtime.
         */
        bool report(const std::string& exporter);

        /**
         * Remove an external PropertyExporterrInterface from reporting.
         * You can use this method to remove a previously added
         * exporter during runtime.
         */
        bool unReport( const std::string& exporter);

        /**
         * This method registers a component to this Extension.
         * It does not cause the component to be reported yet.
         * This has to be done through configuration.
         */
        bool addComponent( ReportingComponent* rep);

        /**
         * This method removes the component out of the 'scope'
         * of this Extension. If it happend to be reported, it
         * will be unreported.
         */
        void removeComponent( ReportingComponent* rep );

        virtual bool updateProperties(const PropertyBag& bag);
            
    protected:
        typedef MarshallConfiguration<SplitStream, TableHeaderMarshaller<SplitStream>, TableMarshaller<SplitStream> > MarshallTableType;

#ifdef OROINT_OS_STDIOSTREAM
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
#ifdef OROINT_OS_STDIOSTREAM
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
