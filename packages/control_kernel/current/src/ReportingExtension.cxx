
#include "control_kernel/ReportingExtension.hpp"

namespace ORO_ControlKernel
{

    ReportingComponent::ReportingComponent(const std::string& _name) 
        : ComponentAspectInterface<ReportingExtension>( _name ),
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
}
