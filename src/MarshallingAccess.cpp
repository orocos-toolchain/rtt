/***************************************************************************
  tag: FMTC  Tue Mar 11 21:49:27 CET 2008  MarshallingAccess.cpp

                        MarshallingAccess.cpp -  description
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



#include "RTT.hpp"
#include "MarshallingAccess.hpp"
#include "TaskContext.hpp"
#include "TaskObject.hpp"

#if !defined(ORO_EMBEDDED) && defined(OROPKG_EXECUTION_PROGRAM_PARSER)
#include "Method.hpp"
#endif
#include "PropertyLoader.hpp"

namespace RTT
{
    MarshallingAccess::MarshallingAccess(TaskContext* parent)
        : mparent(parent)
    {
#if !defined(ORO_EMBEDDED) && defined(OROPKG_EXECUTION_PROGRAM_PARSER)
        OperationInterface* obj = parent->getObject("marshalling");
        if (!obj)
            obj = new TaskObject("marshalling","Read and write Properties to a file.");
        obj->methods()->addMethod(method("loadProperties",&MarshallingAccess::loadProperties, this),
                                  "Read, and create if necessary, Properties from a file.",
                                  "Filename","The file to read the (new) Properties from.");
        obj->methods()->addMethod(method("updateProperties",&MarshallingAccess::updateProperties, this),
                                  "Read some Properties from a file.",
                                  "Filename","The file to read the Properties from.");
        obj->methods()->addMethod(method("readProperties",&MarshallingAccess::readProperties, this),
                                  "Read all Properties from a file.",
                                  "Filename","The file to read the Properties from.");
        obj->methods()->addMethod(method("readProperty",&MarshallingAccess::readProperty, this),
                                  "Read a single Property from a file.",
                                  "Name", "The name of (or the path to) the property to read.",
                                  "Filename","The file to read the Properties from.");

        obj->methods()->addMethod(method("updateFile",&MarshallingAccess::updateFile, this),
                                  "Write some Properties to a file.",
                                  "Filename","The file to write the Properties to.");
        obj->methods()->addMethod(method("writeProperties",&MarshallingAccess::writeProperties, this),
                                  "Write all Properties to a file.",
                                  "Filename","The file to write the Properties to.");
        obj->methods()->addMethod(method("writeProperty",&MarshallingAccess::writeProperty, this),
                                  "Write a single Properties to a file.",
                                  "Name", "The name of (or the path to) the property to write.",
                                  "Filename","The file to write the Properties to.");
        mparent->addObject( obj );
#endif
    }

    bool MarshallingAccess::loadProperties(const std::string& filename) const
    {
        PropertyLoader pl;
        return pl.load( filename, mparent );
    }

    bool MarshallingAccess::readProperties(const std::string& filename) const
    {
        PropertyLoader pl;
        return pl.configure( filename, mparent, true); // all
    }
    bool MarshallingAccess::updateProperties(const std::string& filename) const
    {
        PropertyLoader pl;
        return pl.configure( filename, mparent, false); // not all
    }
    bool MarshallingAccess::writeProperties(const std::string& filename) const
    {
        PropertyLoader pl;
        return pl.save( filename, mparent, true);
    }
    bool MarshallingAccess::updateFile(const std::string& filename) const
    {
        PropertyLoader pl;
        return pl.save( filename, mparent, false);
    }

    bool MarshallingAccess::readProperty(const std::string& name, const std::string& filename) {
        PropertyLoader p;
        return p.configure(filename, mparent, name);
    }

    bool MarshallingAccess::writeProperty(const std::string& name, const std::string& filename) {
        PropertyLoader p;
        return p.save(filename, mparent, name);
    }


}
