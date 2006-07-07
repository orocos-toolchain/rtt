/***************************************************************************
  tag: Peter Soetens  Wed Jan 18 14:11:40 CET 2006  MethodC.cxx 

                        MethodC.cxx -  description
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
 
 
#include "execution/GlobalMethodFactory.hpp"
#include "execution/OperationInterface.hpp"
#include "execution/MethodC.hpp"
#include "execution/FactoryExceptions.hpp"
#include "corelib/DataSourceCommand.hpp"
#include "corelib/Logger.hpp"
#include <corelib/Exceptions.hpp>
#include <vector>

namespace ORO_Execution
{
    using namespace ORO_CoreLib;
    
    class MethodC::D
    {
    public:
        const GlobalMethodFactory* mgcf;
        const MethodRepository* mmr;
        std::string mobject, mname;
        std::vector<DataSourceBase::shared_ptr> args;
        AttributeBase* rta;
        DataSourceBase::shared_ptr m;

        void checkAndCreate() {
            Logger::In in("MethodC");
            if ( mmr ) {
                if ( mmr->hasMember(mname) == false ) {
                    Logger::log() <<Logger::Error << "No '"<<mname<<"' method in this Method Repository."<<Logger::endl;
                    ORO_THROW(name_not_found_exception(mname));
                }
                size_t sz = mmr->getArity(mname);
                if ( sz == args.size() ) {
                    // may throw or return nill
                    m = mmr->getMethod(mname, args );
                    args.clear();
                    if ( !m )
                        return;
                    if (rta)
                        m = new DataSourceCommand( rta->getDataSource()->updateCommand( m.get() ) );
                }
            }
            else {
                assert(mgcf);
                // check validity:
                if ( mgcf->getObjectFactory(mobject) == 0 ) {
                    Logger::log() <<Logger::Error << "No '"<<mobject<<"' object in this Method Factory."<<Logger::endl;
                    ORO_THROW(name_not_found_exception(mobject));
                }
                if ( ! mgcf->getObjectFactory(mobject)->hasMember(mname) ) {
                    Logger::log() <<Logger::Error << "No such method '"+mname+"' in '"+mobject+"' Method Factory."<<Logger::endl;
                    ORO_THROW(name_not_found_exception(mname));
                }
                size_t sz = mgcf->getObjectFactory(mobject)->getArgumentList(mname).size();
                // not created, check if it is time to create:
                if ( sz == args.size() ) {
                    // may throw or return nill
                    m = mgcf->getObjectFactory(mobject)->create(mname, args );
                    args.clear();
                    if ( !m )
                        return;
                    if (rta)
                        m = new DataSourceCommand( rta->getDataSource()->updateCommand( m.get() ) );
                }
            }
        }

        void newarg(DataSourceBase::shared_ptr na)
        {
            this->args.push_back( na );
            this->checkAndCreate();
        }

        void ret(AttributeBase* r)
        {
            this->rta = r->clone();
        }

        D( const GlobalMethodFactory* gcf, const std::string& obj, const std::string& name)
            : mgcf(gcf), mmr(0), mobject(obj), mname(name), rta(0), m()
        {
            this->checkAndCreate();
        }

        D( const MethodRepository* mr, const std::string& name)
            : mgcf(0), mmr(mr), mname(name), rta(0), m()
        {
            this->checkAndCreate();
        }

        D(const D& other)
            : mgcf( other.mgcf), mmr(other.mmr), mobject(other.mobject), mname(other.mname),
              args( other.args ), rta( other.rta ? other.rta->clone() : 0 ), m( other.m )
        {
        }

        ~D()
        {
            delete rta;
        }

    };

    MethodC::MethodC()
        : d(0), m()
    {
    }

    MethodC::MethodC(const GlobalMethodFactory* gcf, const std::string& obj, const std::string& name)
        : d( gcf ? new D( gcf, obj, name) : 0 ), m()
    {
        if ( d->m ) {
            this->m = d->m;
            delete d;
            d = 0;
        }
    }

    MethodC::MethodC(const MethodRepository* mr, const std::string& name)
        : d( mr ? new D( mr, name) : 0 ), m()
    {
        if ( d->m ) {
            this->m = d->m;
            delete d;
            d = 0;
        }
    }

    MethodC::MethodC(const MethodC& other)
        : d( other.d ? new D(*other.d) : 0 ), m( other.m ? other.m : 0)
    {
    }

    MethodC& MethodC::operator=(const MethodC& other)
    {
        delete d;
        d = ( other.d ? new D(*other.d) : 0 );
        m = other.m;
        return *this;
    }

    MethodC::~MethodC()
    {
        delete d;
    }

    MethodC& MethodC::arg( DataSourceBase::shared_ptr a )
    {
        if (d)
            d->newarg( a );
        else {
            Logger::log() <<Logger::Warning << "Extra argument discarded for MethodC."<<Logger::endl;
        }
        if ( d && d->m ) {
            this->m = d->m;
            delete d;
            d = 0;
        }
        return *this;
    }

    MethodC& MethodC::ret( AttributeBase* r )
    {
        if (d)
            d->ret( r );
        else {
            m = new DataSourceCommand(r->getDataSource()->updateCommand( m.get() ) );
        }
        if ( d && d->m ) {
            this->m = d->m;
            delete d;
            d = 0;
        }
        return *this;
    }


    bool MethodC::execute() {
        if (m)
            return m->evaluate();
        else {
            Logger::log() <<Logger::Error << "execute() called on incomplete MethodC."<<Logger::endl;
            if (d) {
                size_t sz;
                if (d->mgcf)
                    sz = d->mgcf->getObjectFactory(d->mobject)->getArity( d->mname );
                else
                    sz = d->mmr->getArity( d->mname );
                Logger::log() <<Logger::Error << "Wrong number of arguments provided for method '"+d->mname+"'"<<Logger::nl;
                Logger::log() <<Logger::Error << "Expected "<< sz << ", got: " << d->args.size() <<Logger::endl;
            }
        }
        return false;
    }

    void MethodC::reset()
    {
        if (m)
            m->reset();
    }
}
