/***************************************************************************
  tag: Peter Soetens  Wed Jan 18 14:11:40 CET 2006  CommandC.cxx 

                        CommandC.cxx -  description
                           -------------------
    begin                : Wed January 18 2006
    copyright            : (C) 2006 Peter Soetens
    email                : peter.soetens@mech.kuleuven.be
 
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
 
 
#include "execution/GlobalCommandFactory.hpp"
#include "execution/CommandC.hpp"
#include "execution/OperationInterface.hpp"
#include "execution/DispatchInterface.hpp"
#include "execution/TryCommand.hpp"
#include "execution/ConditionComposite.hpp"
#include "execution/ConditionBoolDataSource.hpp"
#include "execution/FactoryExceptions.hpp"
#include "corelib/Logger.hpp"
#include <corelib/Exceptions.hpp>
#include <vector>

namespace ORO_Execution
{
    using namespace ORO_CoreLib;
    
    class CommandC::D
    {
    public:
        const GlobalCommandFactory* mgcf;
        const CommandRepository* mcr;
        std::string mobject, mname;
        std::pair<DispatchInterface*,ORO_CoreLib::ConditionInterface*> comcon;
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
                    comcon.first = mcr->getCommand(mname, args); // the dispatch flag is no longer relevant here.
                    args.clear();
                    if (comcon.first == 0)
                        return;
                    // discard the condition, we do not need it.
                    delete comcon.second;
                }
            }
            else {
                if ( mgcf->getObjectFactory(mobject) == 0 ) {
                    Logger::log() <<Logger::Error << "No '"<<mobject<<"' object in this Command Factory."<<Logger::endl;
                    ORO_THROW(name_not_found_exception(mobject));
                }
                if (  mgcf->hasCommand(mobject, mname) == false ) {
                    Logger::log() <<Logger::Error << "No such command '"+mname+"' in '"+mobject+"' Command Factory."<<Logger::endl;
                    ORO_THROW(name_not_found_exception(mname));
                }
                size_t sz = mgcf->getObjectFactory(mobject)->getArgumentList(mname).size();
                if ( sz == args.size() ) {
                    // may throw or return '0,0' if no exceptions.
                    ComCon cc = mgcf->getObjectFactory(mobject)->create(mname, args, false );
                    args.clear();
                    if (cc.first == 0)
                        return;
                    // below we assume that the result is a DispatchInterface object.
                    assert( static_cast<DispatchInterface*>(cc.first) == dynamic_cast<DispatchInterface*>(cc.first) );
                    comcon.first = static_cast<DispatchInterface*>(cc.first);
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

        D( const GlobalCommandFactory* gcf, const std::string& obj, const std::string& name)
            : mgcf(gcf), mcr(0), mobject(obj), mname(name)
        {
            comcon.first = 0;
            this->checkAndCreate();
        }

        D( const CommandRepository* cr, const std::string& name)
            : mgcf(0), mcr(cr), mname(name)
        {
            comcon.first = 0;
            this->checkAndCreate();
        }

        D(const D& other)
            : mgcf( other.mgcf), mcr(other.mcr), mobject(other.mobject), mname(other.mname),
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
        this->cc = di;
    }

    CommandC::CommandC(const GlobalCommandFactory* gcf, const std::string& obj, const std::string& name)
        : d( gcf ? new D( gcf, obj, name) : 0 ), cc()
    {
        if ( d->comcon.first ) {
            this->cc = d->comcon.first->clone();
            delete d;
            d = 0;
        }
    }

    CommandC::CommandC(const CommandRepository* cr, const std::string& name)
        : d( cr ? new D( cr, name) : 0 ), cc()
    {
        if ( d->comcon.first ) {
            this->cc = d->comcon.first->clone();
            delete d;
            d = 0;
        }
    }

    CommandC::CommandC(const CommandC& other)
        : d( other.d ? new D(*other.d) : 0 ), cc()
    {
        if (other.cc)
            this->cc = other.cc->clone();
    }

    CommandC& CommandC::operator=( const CommandC& other )
    {
        delete d;
        delete cc;

        if ( other.d )
            d = new D(*other.d);
        else {
            if (other.cc)
                this->cc = other.cc->clone();
        }
        return *this;
    }

    CommandC::~CommandC()
    {
        delete d;
        delete cc;
    }

    CommandC& CommandC::arg( DataSourceBase::shared_ptr a )
    {
        if (d)
            d->newarg( a );
        else {
            Logger::log() <<Logger::Warning << "Extra argument discarded for CommandC."<<Logger::endl;
        }
        if ( d && d->comcon.first ) {
            this->cc = d->comcon.first->clone();
            delete d;
            d = 0;
        }
        return *this;
    }

    bool CommandC::ready() const {
        // if no d pointer present, we have built the command.
        // analogous to cc != 0
        return d == 0 && cc != 0;
    }

    bool CommandC::execute() {
        // execute dispatch command
        if (cc) {
            if ( cc->evaluate() )
                this->reset();
            return cc->dispatch();
        } else {
            Logger::log() <<Logger::Error << "execute() called on incomplete CommandC."<<Logger::endl;
            if (d) {
                size_t sz;
                if (d->mgcf)
                    sz = d->mgcf->getObjectFactory(d->mobject)->getArity( d->mname );
                else
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

    bool CommandC::evaluate() {
        // check if done
        if (cc )
            return cc->evaluate();
        return false;
    }

    void CommandC::reset()
    {
        if (cc)
            cc->reset();
    }
}
