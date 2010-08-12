/***************************************************************************
  tag: FMTC  Tue Mar 11 21:49:27 CET 2008  MarshallingService.cpp

                        MarshallingService.cpp -  description
                           -------------------
    begin                : Tue March 11 2008
    copyright            : (C) 2008 FMTC
    email                : peter.soetens@fmtc.be

 ***************************************************************************
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU General Public                   *
 *   License as published by the Free Software Foundation;                 *
 *   version 2 of the License.                                             *
 *                                                                         *
 *   As a special exception, you may use this file as part of a free       *
 *   software library without restriction.  Specifically, if other files   *
 *   instantiate templates or use macros or inline functions from this     *
 *   file, or you compile this file and link it with other files to        *
 *   produce an executable, this file does not by itself cause the         *
 *   resulting executable to be covered by the GNU General Public          *
 *   License.  This exception does not however invalidate any other        *
 *   reasons why the executable file might be covered by the GNU General   *
 *   Public License.                                                       *
 *                                                                         *
 *   This library is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU     *
 *   Lesser General Public License for more details.                       *
 *                                                                         *
 *   You should have received a copy of the GNU General Public             *
 *   License along with this library; if not, write to the Free Software   *
 *   Foundation, Inc., 59 Temple Place,                                    *
 *   Suite 330, Boston, MA  02111-1307  USA                                *
 *                                                                         *
 ***************************************************************************/



#include "MarshallingService.hpp"
#include "../TaskContext.hpp"
#include "../plugin/ServicePlugin.hpp"

#include "rtt-config.h"
#if !defined(ORO_EMBEDDED)
#include "../OperationCaller.hpp"
#endif
#include "PropertyLoader.hpp"

//ORO_SERVICE_PLUGIN( RTT::marsh::MarshallingService )
ORO_SERVICE_NAMED_PLUGIN(RTT::marsh::MarshallingService, "marshalling")

namespace RTT {
    using namespace detail;

    MarshallingService::shared_ptr MarshallingService::Create(TaskContext* parent){
        shared_ptr sp(new MarshallingService(parent));
        parent->provides()->addService( sp );
        return sp;
    }

    MarshallingService::MarshallingService(TaskContext* parent)
        : Service("marshalling", parent)
    {
        this->doc("Property marshalling interface. Use this service to read and write properties from/to a file.");
        this->addOperation("loadProperties",&MarshallingService::loadProperties, this).doc(
                                  "Read, and create if necessary, Properties from a file.").arg(
                                  "Filename","The file to read the (new) Properties from.");
        this->addOperation("updateProperties", &MarshallingService::updateProperties, this).doc("Read some Properties from a file.").arg("Filename", "The file to read the Properties from.");
        this->addOperation("readProperties", &MarshallingService::readProperties, this).doc("Read all Properties from a file.").arg("Filename", "The file to read the Properties from.");
        this->addOperation("readProperty", &MarshallingService::readProperty, this).doc("Read a single Property from a file.").arg("Name", "The name of (or the path to) the property to read.").arg("Filename", "The file to read the Properties from.");

        this->addOperation("updateFile", &MarshallingService::updateFile, this).doc("Write some Properties to a file.").arg("Filename", "The file to write the Properties to.");
        this->addOperation("writeProperties", &MarshallingService::writeProperties, this).doc("Write all Properties to a file.").arg("Filename", "The file to write the Properties to.");
        this->addOperation("writeProperty", &MarshallingService::writeProperty, this).doc("Write a single Properties to a file.").arg("Name", "The name of (or the path to) the property to write.").arg("Filename", "The file to write the Properties to.");
    }

    bool MarshallingService::loadProperties(const std::string& filename) const
    {
        PropertyLoader pl;
        return pl.load( filename, mowner );
    }

    bool MarshallingService::readProperties(const std::string& filename) const
    {
        PropertyLoader pl;
        return pl.configure( filename, mowner, true); // all
    }
    bool MarshallingService::updateProperties(const std::string& filename) const
    {
        PropertyLoader pl;
        return pl.configure( filename, mowner, false); // not all
    }
    bool MarshallingService::writeProperties(const std::string& filename) const
    {
        PropertyLoader pl;
        return pl.save( filename, mowner, true);
    }
    bool MarshallingService::updateFile(const std::string& filename) const
    {
        PropertyLoader pl;
        return pl.save( filename, mowner, false);
    }

    bool MarshallingService::readProperty(const std::string& name, const std::string& filename) {
        PropertyLoader p;
        return p.configure(filename, mowner, name);
    }

    bool MarshallingService::writeProperty(const std::string& name, const std::string& filename) {
        PropertyLoader p;
        return p.save(filename, mowner, name);
    }


}
