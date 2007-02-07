/***************************************************************************
  tag: Peter Soetens  Tue Dec 21 22:43:08 CET 2004  CommandBinary.hpp 

                        CommandBinary.hpp -  description
                           -------------------
    begin                : Tue December 21 2004
    copyright            : (C) 2004 Peter Soetens
    email                : peter.soetens@mech.kuleuven.ac.be
 
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
