#include "corelib/ReportHandler.hpp"
#include "corelib/Property.hpp"

using namespace ORO_CoreLib;

ReportHandler::ReportHandler()
    :reportSet( new Property<PropertyBag>("ReportSet", "Reporting Unit") ),
     time(0.0), marshaller(report)
{
    updates.reserve(64);
    monitor(&time,"TimeStamp", "The time a copy of the data was made");
}

ReportHandler::ReportHandler(const std::string& name)
        : StringExporterInterface(name),
        reportSet( new Property<PropertyBag>(name, "Reporting Unit")),
        time(0.0), marshaller(report)
{
    monitor(&time,"TimeStamp", "The time a copy of the data was made");
}

ReportHandler::~ReportHandler()
{
    for(PropertyBag::const_iterator itl = reportSet->value().getProperties().begin(); itl != reportSet->value().getProperties().end(); ++itl)
        delete *itl;// delete properties
    
    for (std::vector<Updater*>::iterator itl= updates.begin(); itl != updates.end(); ++itl)
        delete *itl;
    
    delete reportSet;// delete the set
}

void ReportHandler::monitor( const double* d, const std::string& name, const std::string& desc)
{
    Property<double>* p = new Property<double>(name, desc) ;
    reportSet->value().add(p);
    updates.push_back( new ReportPointer<double>(d, p) );
}

void ReportHandler::monitor( const int* d, const std::string& name, const std::string& desc)
{
    Property<int>* p = new Property<int>(name, desc) ;
    reportSet->value().add(p);
    updates.push_back( new ReportPointer<int>(d, p) );
}

void ReportHandler::refresh(const Seconds timeStamp )
{
    // refresh all properties, needs extension of property interface!
    time = timeStamp;
    for (std::vector<Updater*>::iterator itl= updates.begin(); itl != updates.end(); ++itl)
        (*itl)->update();
}

void ReportHandler::remove(const std::string& name)
{
    for( std::vector<Updater*>::iterator itl=updates.begin(); itl != updates.end(); ++itl)
        if ( (*itl)->base()->getName() == name )
        {
            reportSet->value().remove( (*itl)->base() );
            delete (*itl);
            updates.erase(itl);
            break;
        }
}

void ReportHandler::reportGet(std::string& result)
{
    marshaller.serialize( *reportSet );
    report >> result;
}

std::string& ReportHandler::reportGet()
{
    marshaller.serialize( *reportSet );
    report >> report_copy;
    return report_copy;
}


