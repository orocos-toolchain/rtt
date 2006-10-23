/***************************************************************************
  tag: Peter Soetens  Thu Jul 15 11:21:05 CEST 2004  PropertyBase.cxx 

                        PropertyBase.cxx -  description
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

#ifdef ORO_PRAGMA_INTERFACE
#pragma implementation
#endif
#include "PropertyBase.hpp"

namespace RTT {

    PropertyBase::~PropertyBase()
    {}

    PropertyBase::PropertyBase()
    {}

    PropertyBase::PropertyBase( std::string name, std::string description)
        : _name(name), _description(description)
    {}

    void PropertyBase::setName(const std::string& name)
    {
        _name = name;
    }

    void PropertyBase::setDescription(const std::string& desc)
    {
        _description = desc;
    }

    std::ostream& operator<<(std::ostream &os, PropertyBase* p)
    {
        os << p->getDataSource();
        return os;
    }
}
