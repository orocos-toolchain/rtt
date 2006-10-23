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
 
 
#include "OperationInterface.hpp"
#include "MethodC.hpp"
#include "FactoryExceptions.hpp"
#include "DataSourceCommand.hpp"
#include "Logger.hpp"
#include <Exceptions.hpp>
#include <vector>

namespace RTT
{
    
    
    class MethodC::D
    {
    public:
        const MethodRepository::Factory* mmr;
        std::string mname;
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
                    m = mmr->produce(mname, args );
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

        D( const MethodRepository::Factory* mr, const std::string& name)
            : mmr(mr), mname(name), rta(0), m()
        {
            this->checkAndCreate();
        }

        D(const D& other)
            : mmr(other.mmr), mname(other.mname),
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

    MethodC::MethodC(const MethodRepository::Factory* mr, const std::string& name)
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

    bool MethodC::ready() const
    {
        return m;
    }

    DataSourceBase::shared_ptr MethodC::getDataSource() { return m; }
}
