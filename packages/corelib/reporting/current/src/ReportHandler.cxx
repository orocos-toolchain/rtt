/***************************************************************************
  tag: Peter Soetens  Mon May 10 19:10:29 CEST 2004  ReportHandler.cxx 

                        ReportHandler.cxx -  description
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


