/***************************************************************************
  tag: Peter Soetens  Mon Jan 19 14:11:26 CET 2004  ReportingExtension.hpp 

                        ReportingExtension.hpp -  description
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
 
#ifndef MOTIONREPORTINGEXTENSION_HPP
#define MOTIONREPORTINGEXTENSION_HPP


#include "KernelInterfaces.hpp"
#include "ComponentInterfaces.hpp"
#include "DataServer.hpp"

#include <corelib/PropertyExporter.hpp>
#include <corelib/PropertyReporter.hpp>
#include <corelib/marshalling/MarshallerAdaptors.hpp>

#include <corelib/marshalling/TableMarshaller.hpp>
#include <corelib/marshalling/TableHeaderMarshaller.hpp>
#include <corelib/marshalling/EmptyHeaderMarshaller.hpp>
#include <corelib/TaskNonRealTime.hpp>

#include <pkgconf/os.h>
#ifdef OROINT_OS_STDIOSTREAM
#include <fstream>
#endif

#include <pkgconf/system.h>
#ifdef OROPKG_GEOMETRY
#include <geometry/MotionProperties.hpp>
#endif

#pragma interface

namespace ORO_ControlKernel
{
    using namespace std;

    using namespace ORO_CoreLib;
    class ReportingExtension;
    class BackendReportingExtension;

    /**
     * @brief The base class for reporting Components.
     *
     * @see ReportingExtension
     * @see DefaultAspect
     */
    class ReportingComponent
        : public detail::ComponentAspectInterface<ReportingExtension>
    {
        friend class ORO_ControlKernel::ReportingExtension;
        friend class ORO_ControlKernel::BackendReportingExtension;

    protected:
        using detail::ComponentAspectInterface<ReportingExtension>::enableAspect;
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
        virtual void exportReports( PropertyBag& bag)=0;

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
     * @brief A ControlKernel extension for periodically reporting Component
     * properties and DataObject contents to a human readable
     * text format.
     *
     * It can report to a file or to the screen.
     * This extension is purely meant for text based data gathering.
     * It can also report DataObjects when used in the NSControlKernel.
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
        ReportingExtension( ControlKernelInterface* _base );

        virtual ~ReportingExtension();

            
        /**
         * A class for streaming to two ostream objects.
         */
        struct SplitStream;

//         virtual TaskInterface* getTask() const;

//         virtual void setTask( TaskInterface* task );

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
        virtual bool initialize();

        virtual void step();

        virtual void finalize();

        typedef MarshallConfiguration<TableHeaderMarshaller<SplitStream>,
                                      TableMarshaller<SplitStream> > MarshallTableType;
        typedef MarshallConfiguration<EmptyHeaderMarshaller<SplitStream>,
                                      TableMarshaller<SplitStream> > NoHeaderMarshallTableType;

        virtual ORO_Execution::MethodFactoryInterface* createMethodFactory();

        bool startReporting();
        bool stopReporting() ;
#ifdef OROINT_OS_STDIOSTREAM
        std::ofstream* fileStream;
#endif
        SplitStream*  splitStream;

        MarshallTableType* config;
        NoHeaderMarshallTableType* nh_config;

        /**
         * The reporter for properties.
         */
        PropertyCollectorInterface* reporter;
            
        Property<double> period;
        Property<int> interval;
        Property<std::string> repFile;
        Property<bool> toStdOut;
        Property<bool> writeHeader;
#ifdef OROINT_OS_STDIOSTREAM
        Property<bool> toFile;
#endif
        Property<std::string> reportServer;
        Property<bool> autostart;

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

        typedef std::vector< boost::shared_ptr<DataObjectReporting> > DosList;

        /**
         * All external exporters that want to be reported.
         */
        ExpList exporters;
            
        /**
         * All components listed in the property file that may be reported.
         */
        RepList rep_comps;

        /**
         * All DataObjects listed in the property file that may be reported.
         */
        RepList rep_dos;

        /**
         * All DataObjects actively reporting data.
         */
        DosList active_dos;
        /**
         * True if we own the report server.
         */
        bool serverOwner;

        /**
         * Used for sub-sampling interval
         */
        int count;

        /**
         * The base kernel of this extension.
         */
        ControlKernelInterface* base;
    };

}

#endif
