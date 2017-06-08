/***************************************************************************
  tag: Peter Soetens  Wed Jan 18 14:11:40 CET 2006  OperationCallerC.cxx

                        OperationCallerC.cxx -  description
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


#include "OperationCallerC.hpp"
#include "../FactoryExceptions.hpp"
#include "DataSourceCommand.hpp"
#include "../Service.hpp"
#include "../Logger.hpp"
#include "Exceptions.hpp"
#include <vector>

namespace RTT {
    using namespace detail;


    class OperationCallerC::D
    {
    public:
        OperationInterfacePart* ofp;
        ExecutionEngine* caller;
        std::string mname;
        std::vector<DataSourceBase::shared_ptr> args;
        DataSourceBase::shared_ptr rta;
        DataSourceBase::shared_ptr m;
        DataSourceBase::shared_ptr s;

        void checkAndCreate() {
            if ( ofp ) {
                size_t sz = ofp->arity();
                if ( sz == args.size() ) {
                    // may throw or return nill
                    m = ofp->produce(args, caller );
                    try {
                        s = ofp->produceSend(args, caller );
                    } catch( no_asynchronous_operation_exception const& /*e*/) {
                        // leave s empty.
                    }
                    args.clear();
                    if ( !m )
                        return;
                    if (rta)
                        try {
                            m = new DataSourceCommand( rta->updateAction( m.get() ) );
                        } catch( bad_assignment& /*ba*/ ) {
                            Logger::In in("OperationCallerC");
                            log(Error) << "Error in OperationCallerC::ret : can not convert return value of type "<< m->getType() << " to given type "<< rta->getType()<<endlog();
                        }

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
            this->rta = r->getDataSource();
        }

        void ret(DataSourceBase::shared_ptr d)
        {
            this->rta = d;
        }

        D( OperationInterfacePart* mr, const std::string& name, ExecutionEngine* caller)
            : ofp(mr), caller(caller), mname(name), rta(), m(), s()
        {
            this->checkAndCreate();
        }

        D(const D& other)
            : ofp(other.ofp), caller(other.caller), mname(other.mname),
              args( other.args ), rta( other.rta ), m( other.m ), s(other.s)
        {
        }

        ~D()
        {
        }

    };

    OperationCallerC::OperationCallerC()
        : d(0), m()
    {
    }

    OperationCallerC::OperationCallerC(OperationInterfacePart* mr, const std::string& name, ExecutionEngine* caller)
        : d( mr ? new D( mr, name, caller) : 0 ), m(), ofp(mr), mname(name)
    {
        if ( d && d->m ) {
            this->m = d->m;
            this->s = d->s;
            delete d;
            d = 0;
        } else {
            if (mr == 0)
                log(Error) <<"Can not construct OperationCallerC for '"<<name<<"' from null OperationInterfacePart."<<endlog();
        }
    }

    OperationCallerC::OperationCallerC(const OperationCallerC& other)
        : d( other.d ? new D(*other.d) : 0 ), m( other.m ? other.m : 0), ofp(other.ofp), mname(other.mname)
    {
    }

    OperationCallerC& OperationCallerC::operator=(const OperationCallerC& other)
    {
        if ( &other == this )
            return *this;
        delete d;
        d = ( other.d ? new D(*other.d) : 0 );
        m = other.m;
        s = other.s;
        ofp = other.ofp;
        mname = other.mname;
        return *this;
    }

    OperationCallerC::~OperationCallerC()
    {
        delete d;
    }

    OperationCallerC& OperationCallerC::arg( DataSourceBase::shared_ptr a )
    {
        if (d)
            d->newarg( a );
        else {
            Logger::log() <<Logger::Warning << "Extra argument discarded for OperationCallerC."<<Logger::endl;
        }
        if ( d && d->m ) {
            this->m = d->m;
            this->s = d->s;
            delete d;
            d = 0;
        }
        return *this;
    }

    OperationCallerC& OperationCallerC::ret( AttributeBase* r )
    {
        if (d)
            d->ret( r );
        else {
            if (m) {
                try {
                    m = new DataSourceCommand(r->getDataSource()->updateAction( m.get() ) );
                } catch( bad_assignment& /*ba*/ ) {
                    log(Error) << "Error in OperationCallerC::ret : can not convert return value of type "<< m->getType() << " to given type "<< r->getDataSource()->getType()<<endlog();
                }
            } else
                log(Error) <<"Can not add return argument to invalid OperationCallerC."<<endlog();
        }
        return *this;
    }

    OperationCallerC& OperationCallerC::ret(DataSourceBase::shared_ptr r)
    {
        if (d)
            d->ret( r );
        else {
            // no d, store manually:
            if (m)
                m = new DataSourceCommand(r->updateAction( m.get() ) );
            else
                log(Error) <<"Can not add return argument to invalid OperationCallerC."<<endlog();
        }
        return *this;
    }


    bool OperationCallerC::call() {
        if (m) {
            m->reset();
            return m->evaluate();
        } else {
            Logger::log() <<Logger::Error << "call() called on incomplete OperationCallerC."<<Logger::endl;
            if (d) {
                size_t sz;
                sz = d->ofp->arity();
                Logger::log() <<Logger::Error << "Wrong number of arguments provided for method '"+d->mname+"'"<<Logger::nl;
                Logger::log() <<Logger::Error << "Expected "<< sz << ", got: " << d->args.size() <<Logger::endl;
            }
        }
        return false;
    }

    void OperationCallerC::check() {
        if (d) {
            // something went wrong, let producer throw
            if (d->ofp)
                DataSourceBase::shared_ptr dummy = d->ofp->produce( d->args, d->caller );
            else
                throw name_not_found_exception( d->mname );
        }
    }

    SendHandleC OperationCallerC::send() {
        DataSourceBase::shared_ptr h;
        try {
            h = ofp->produceHandle();
        } catch( no_asynchronous_operation_exception const& nao) {
            log(Error) <<"OperationCallerC::send(): Can not send the '" << ofp->getName() << "' operation:" << nao.what()  << endlog();
            return SendHandleC();
        }
        if (s) {
            // evaluate and copy result of s to handle and pass handle to SendHandleC.
            s->reset();
#ifndef NDEBUG
            bool result =
#endif
                    h->update( s.get() );
            assert( result );
            return SendHandleC( s, h, ofp, mname );
        }
        else {
            Logger::log() <<Logger::Error << "send() called on incomplete OperationCallerC."<<Logger::endl;
            if (d) {
                size_t sz;
                sz = d->ofp->arity();
                Logger::log() <<Logger::Error << "Wrong number of arguments provided for method '"+d->mname+"'"<<Logger::nl;
                Logger::log() <<Logger::Error << "Expected "<< sz << ", got: " << d->args.size() <<Logger::endl;
            }
        }
        return SendHandleC();
    }

    bool OperationCallerC::ready() const
    {
        return m != 0;
    }

    OperationInterfacePart* OperationCallerC::getOrp() const {
        return ofp;
    }

    std::string const& OperationCallerC::getName() const {
        return mname;
    }

    DataSourceBase::shared_ptr OperationCallerC::getCallDataSource() { return m; }
    DataSourceBase::shared_ptr OperationCallerC::getSendDataSource() { return s; }
}
