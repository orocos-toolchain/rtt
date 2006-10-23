/***************************************************************************
  tag: Peter Soetens  Tue Dec 21 22:43:08 CET 2004  CommandBinary.hpp 

                        CommandBinary.hpp -  description
                           -------------------
    begin                : Tue December 21 2004
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
 
 
#ifndef ORO_COMMANDBINARY_HPP
#define ORO_COMMANDBINARY_HPP

#include "CommandInterface.hpp"
#include "DataSource.hpp"

namespace RTT
{
    

    struct CommandBinary : public CommandInterface
    {
        CommandInterface* _f;
        CommandInterface* _s;
        CommandBinary( CommandInterface* f, CommandInterface* s)
            : _f(f), _s(s) {}
        virtual ~CommandBinary() {
            delete _f;
            delete _s;
        }

        void readArguments()
        {
            _f->readArguments();
            _s->readArguments();
        }

        virtual bool valid() const {
            return _f->valid() && _s->valid();
        }

        virtual bool execute() {
            return _f->execute() && _s->execute();
        }
        virtual void reset() {
            _f->reset();
            _s->reset();
        }
        virtual CommandInterface* clone() const {
            return new CommandBinary( _f->clone(), _s->clone() );
        }
        virtual CommandInterface* copy( std::map<const DataSourceBase*, DataSourceBase*>& alreadyCloned ) const {
            return new CommandBinary( _f->copy( alreadyCloned ), _s->copy( alreadyCloned ) );
        }
    };

}

#endif
