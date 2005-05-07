/***************************************************************************
  tag: Peter Soetens  Thu Jul 15 11:21:07 CEST 2004  DataObjectReporting.cxx 

                        DataObjectReporting.cxx -  description
                           -------------------
    begin                : Thu July 15 2004
    copyright            : (C) 2004 Peter Soetens
    email                : peter.soetens at mech.kuleuven.ac.be
 
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

#pragma implementation
#include "control_kernel/DataObjectReporting.hpp"


namespace ORO_ControlKernel
{
    using namespace detail;

    ORO_CoreLib::NameServer<boost::shared_ptr<DataObjectReporting> > DataObjectReporting::nameserver;

    ReportingClient::~ReportingClient() {}

    DataObjectReporting::DataObjectReporting(const std::string& name)
        : exporter( name ), reports( "Data","The Reported Data of this DataObject")
    {
        nameserver.registerObject( boost::shared_ptr<DataObjectReporting>(this), name );
        exporter.value().add( &reports );
    }

    DataObjectReporting::~DataObjectReporting() {
        // The nameserver should destroy if it holds the only reference...
        //nameserver.unregisterName( this->getName()  );
    }

    void DataObjectReporting::addClient( ReportingClient* c)
    {
        clients.push_back(c);
    }
    void DataObjectReporting::removeClient( ReportingClient* c)
    {
        std::vector<ReportingClient*>::iterator it = std::find(clients.begin(), clients.end(), c);
        if ( it != clients.end() )
            clients.erase( it );
    }

    const std::string& DataObjectReporting::getName() const
    {
        return exporter.getName();
    }
}
