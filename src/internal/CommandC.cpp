/***************************************************************************
  tag: Peter Soetens  Wed Jan 18 14:11:40 CET 2006  CommandC.cxx

                        CommandC.cxx -  description
                           -------------------
    begin                : Wed January 18 2006
    copyright            : (C) 2006 Peter Soetens
    email                : peter.soetens@mech.kuleuven.be

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


#include "CommandC.hpp"
#include "CommandRepository.hpp"
#include "../base/DispatchInterface.hpp"
#include "../scripting/TryCommand.hpp"
#include "../scripting/ConditionComposite.hpp"
#include "FactoryExceptions.hpp"
#include "../Logger.hpp"
#include "Exceptions.hpp"
#include <vector>

namespace RTT
{


    class CommandC::D
    {
    public:
        const CommandRepository::Factory* mcr;
        std::string mname;
        std::pair<DispatchInterface*,ConditionInterface*> comcon;
        std::vector<DataSourceBase::shared_ptr> args;

        void checkAndCreate() {
            Logger::In in("CommandC");
            if (mcr) {
                if (  mcr->hasMember(mname) == false ) {
                    Logger::log() <<Logger::Error << "No such command '"+mname+"' in Command Repository."<<Logger::endl;
                    ORO_THROW(name_not_found_exception(mname));
                }
                size_t sz = mcr->getArity(mname);
                if ( sz == args.size() ) {
                    // may throw or return '0,0' if no exceptions.
                    comcon.first = mcr->produce(mname, args); // the dispatch flag is no longer relevant here.
                    args.clear();
                    if (comcon.first == 0)
                        return;
                    // discard the condition, we do not need it.
                    delete comcon.second;
                }
            }
        }

    public:
        void newarg(DataSourceBase::shared_ptr na)
        {
            this->args.push_back( na );
            this->checkAndCreate();
        }

        D( const CommandRepository::Factory* cr, const std::string& name)
            : mcr(cr), mname(name)
        {
            comcon.first = 0;
            this->checkAndCreate();
        }

        D(const D& other)
            : mcr(other.mcr), mname(other.mname),
              args( other.args )
        {
            if (other.comcon.first )
                comcon.first = other.comcon.first->clone();
            else
                comcon.first = 0;
        }

        ~D()
        {
            delete comcon.first;
        }

    };

    CommandC::CommandC()
        : d(0), cc()
    {
    }

    CommandC::CommandC(DispatchInterface* di)
        : d(0), cc()
    {
        this->cc.reset( di );
    }

    CommandC::CommandC(DispatchInterface::shared_ptr di)
        : d(0), cc(di)
    {
    }

    CommandC::CommandC(const CommandRepository::Factory* cr, const std::string& name)
        : d( cr ? new D( cr, name) : 0 ), cc()
    {
        if ( d->comcon.first ) {
            this->cc.reset( d->comcon.first->clone() );
            delete d;
            d = 0;
        }
    }

    CommandC::CommandC(const CommandC& other)
        : d( other.d ? new D(*other.d) : 0 ), cc()
    {
        if (other.cc)
            this->cc.reset( other.cc->clone() );
    }

    CommandC& CommandC::operator=( const CommandC& other )
    {
        delete d;

        if ( other.d )
            d = new D(*other.d);
        else {
            if (other.cc)
                this->cc.reset( other.cc->clone() );
        }
        return *this;
    }

    CommandC::~CommandC()
    {
        delete d;
    }

    CommandC& CommandC::arg( DataSourceBase::shared_ptr a )
    {
        if (d)
            d->newarg( a );
        else {
            Logger::log() <<Logger::Warning << "Extra argument discarded for CommandC."<<Logger::endl;
        }
        if ( d && d->comcon.first ) {
            this->cc.reset( d->comcon.first->clone() );
            delete d;
            d = 0;
        }
        return *this;
    }

    bool CommandC::ready() const {
        // if no d pointer present, we have built the command.
        // analogous to cc != 0
        return d == 0 && cc;
    }

    bool CommandC::execute() {
        // execute dispatch command
        if (cc) {
            if ( cc->done() )
                this->reset();
            return cc->dispatch();
        } else {
            Logger::log() <<Logger::Error << "execute() called on incomplete CommandC."<<Logger::endl;
            if (d) {
                size_t sz;
		sz = d->mcr->getArity( d->mname );
                Logger::log() <<Logger::Error << "Wrong number of arguments provided for command '"+d->mname+"'"<<Logger::nl;
                Logger::log() <<Logger::Error << "Expected "<< sz << ", got: " << d->args.size() <<Logger::endl;
            }
        }
        return false;
    }

    bool CommandC::sent() const{
        if (cc)
            return cc->sent();
        return false;
    }


    bool CommandC::accepted() const {
        // check if dispatch command was accepted.
        if (cc)
            return cc->accepted();
        return false;
    }

    bool CommandC::executed() const {
        // check if dispatch command was executed by the processor
        if (cc)
            return cc->executed();
        return false;
    }

    bool CommandC::valid() const {
        // check if dispatch command had valid args.
        if (cc)
            return cc->valid();
        return false;
    }

    bool CommandC::done() const {
        // check if done
        if (cc )
            return cc->done();
        return false;
    }

    DispatchInterface::Status CommandC::status() const {
	if (cc)
            return cc->status();
	return DispatchInterface::NotReady;
    }

    void CommandC::reset()
    {
        if (cc)
            cc->reset();
    }
}
