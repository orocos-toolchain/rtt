/***************************************************************************
  tag: The SourceWorks  Tue Sep 7 00:55:18 CEST 2010  SendHandleC.cpp

                        SendHandleC.cpp -  description
                           -------------------
    begin                : Tue September 07 2010
    copyright            : (C) 2010 The SourceWorks
    email                : peter@thesourceworks.com

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


#include "rtt/internal/SendHandleC.hpp"
#include "rtt/FactoryExceptions.hpp"
#include "rtt/internal/DataSourceCommand.hpp"
#include "rtt/Service.hpp"
#include "rtt/Logger.hpp"
#include "rtt/internal/Exceptions.hpp"
#include <vector>

namespace RTT
{ namespace internal {

    using namespace detail;


    class SendHandleC::E
    {
    public:
        E(base::DataSourceBase::shared_ptr op) : s(), b(), mop(op), orp(0) {}

        ~E() {
            // force synchronisation in case we are the last SendHandleC. We may not cleanup mop (holds data!), until the op
            // completed or failed.
            // Reduce refcount on mopkeeper
            mopkeeper.reset();
        }

        /**
         * This data source will do a collect/collectIfDone when
         * being evaluated().
         */
        internal::DataSource<SendStatus>::shared_ptr s;
        /**
         * Stores the blocking/non blocking flag for collect/collectIfDone.
         */
        internal::AssignableDataSource<bool>::shared_ptr b;
        /**
         * Stores the operation in order to avoid its premature destruction.
         */
        base::DataSourceBase::shared_ptr mop;

    /**
     * This is a custom deleter that blocks on an asynchronous
     * operation
     */
    struct OperationKeeper
    {
	DataSource<SendStatus>::shared_ptr ms;
	AssignableDataSource<bool>::shared_ptr mb;
	bool autocollect;
	OperationKeeper(DataSource<SendStatus>::shared_ptr s, AssignableDataSource<bool>::shared_ptr b) : ms(s), mb(b), autocollect(true) {}
	~OperationKeeper() {
		if (ms && autocollect) {
			mb->set(true); // blocking
			try { // the evaluated function may throw
				ms->evaluate();
			} catch (std::exception&) {
			}
		}
	}
    };

        boost::shared_ptr<OperationKeeper> mopkeeper;

        /**
         * Stores the OperationInterfacePart pointer contained in this SendHandle
         */
        OperationInterfacePart* orp;
    };



    class SendHandleC::D
    {
    public:
        OperationInterfacePart* mofp;
        std::string mname;
        std::vector<DataSourceBase::shared_ptr> args;
        DataSource<SendStatus>::shared_ptr s;
        DataSourceBase::shared_ptr msh;
        AssignableDataSource<bool>::shared_ptr blocking;

        void checkAndCreate() {
            Logger::In in("SendHandleC");
            if ( mofp ) {
                size_t sz = mofp->collectArity();
                if ( sz == args.size() ) {
                    // insert the send handle first.
                    args.insert( args.begin(), msh );
                    // may throw or return nill
                    s = boost::dynamic_pointer_cast<DataSource<SendStatus> >( mofp->produceCollect(args, blocking ) );
                    args.clear();
                    if ( !s ) {
                        log(Error) << "Failed to produce collector for "<< mname << " with " << sz << " arguments." << endlog();
                        return;
                    }
                }
            }
        }

        void newarg(DataSourceBase::shared_ptr na)
        {
            this->args.push_back( na );
            this->checkAndCreate();
        }

        D( base::DataSourceBase::shared_ptr sh, OperationInterfacePart* ofp, const std::string& name)
            : mofp(ofp), mname(name), s(), msh(sh), blocking( new ValueDataSource<bool>(false) )
        {
            this->checkAndCreate();
        }

        D(const D& other)
            : mofp(other.mofp), mname(other.mname),
              args( other.args ), s( other.s ), msh(other.msh), blocking(new ValueDataSource<bool>(false))
        {
        }

        ~D()
        {
        }

    };

    SendHandleC::SendHandleC()
        : d(0), e( new E(0) )
    {
    }

    SendHandleC::SendHandleC( base::DataSourceBase::shared_ptr op, base::DataSourceBase::shared_ptr sh, OperationInterfacePart* ofp, const std::string& name )
        : d( ofp ? new D( sh, ofp, name ) : 0 ), e( new E(op) )
    {
        if ( d->s ) {
            e->s = d->s;
            e->b = d->blocking;
            e->mopkeeper.reset( new E::OperationKeeper( e->s, e->b) );
            delete d;
            d = 0;
        }
        this->e->orp = ofp;
    }

    SendHandleC::SendHandleC(const SendHandleC& other)
        : d( other.d ? new D(*other.d) : 0 ), e( new E(*other.e) )
    {
    }

    SendHandleC& SendHandleC::operator=(const SendHandleC& other)
    {
        if ( &other == this )
            return *this;
        delete d;
        d = ( other.d ? new D(*other.d) : 0 );
        e->s = other.e->s;
        e->b = other.e->b;
        e->mop = other.e->mop;
        e->mopkeeper = other.e->mopkeeper;
        e->orp = other.e->orp;
        return *this;
    }

    SendHandleC::~SendHandleC()
    {
        delete d;
        delete e;
    }

    SendHandleC& SendHandleC::arg( DataSourceBase::shared_ptr a )
    {
        if (d)
            d->newarg( a );
        else {
            Logger::log() <<Logger::Warning << "Extra argument discarded for SendHandleC."<<Logger::endl;
        }
        if ( d && d->s ) {
            e->s = d->s;
            e->b = d->blocking;
            e->orp = d->mofp;
            e->mopkeeper.reset( new E::OperationKeeper( e->s, e->b) );
            delete d;
            d = 0;
        }
        return *this;
    }

    SendStatus SendHandleC::collect() {
        if (e->s) {
            e->b->set(true); // blocking
            e->s->evaluate();
            return e->s->value();
        }
        else {
            Logger::log() <<Logger::Error << "collect() called on incomplete SendHandleC."<<Logger::endl;
            if (d) {
                size_t sz;
                sz = d->mofp->collectArity();
                Logger::log() <<Logger::Error << "Wrong number of arguments provided for method '"+d->mname+"'"<<Logger::nl;
                Logger::log() <<Logger::Error << "Expected "<< sz << ", got: " << d->args.size() <<Logger::endl;
            }
        }
        return SendFailure;
    }

    SendStatus SendHandleC::collectIfDone() {
        if (e->s) {
            e->b->set(false); // non blocking
            // does the send.
            e->s->evaluate();
            // pass on handle.
            return e->s->value();
        }
        else {
            Logger::log() <<Logger::Error << "collectIfDone() called on incomplete SendHandleC."<<Logger::endl;
            if (d) {
                size_t sz;
                sz = d->mofp->collectArity();
                Logger::log() <<Logger::Error << "Wrong number of arguments provided for method '"+d->mname+"'"<<Logger::nl;
                Logger::log() <<Logger::Error << "Expected "<< sz << ", got: " << d->args.size() <<Logger::endl;
            }
        }
        return SendFailure;
    }

    bool SendHandleC::ready() const
    {
        return e->s != 0;
    }

    void SendHandleC::setAutoCollect(bool on_off) {
    	if (e->mopkeeper)
    		e->mopkeeper->autocollect = on_off;
    }

    void SendHandleC::check() {
        if (d) {
            // something went wrong, let producer throw
            if (d->mofp)
                DataSourceBase::shared_ptr dummy = d->mofp->produceCollect( d->args, d->blocking );
            else
                throw invalid_handle_exception();
        }
    }


    DataSourceBase::shared_ptr SendHandleC::getSendHandleDataSource() { return e->s; }

    OperationInterfacePart* SendHandleC::getOrp() { return e->orp; }

}}
