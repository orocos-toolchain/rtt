#include "SendHandleC.hpp"
#include "../interface/FactoryExceptions.hpp"
#include "DataSourceCommand.hpp"
#include "../interface/Service.hpp"
#include "../Logger.hpp"
#include "Exceptions.hpp"
#include <vector>

namespace RTT {
    using namespace detail;


    class SendHandleC::D
    {
    public:
        OperationRepositoryPart* mofp;
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

        D( base::DataSourceBase::shared_ptr sh, OperationRepositoryPart* ofp, const string& name)
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
        : d(0), s(), b()
    {
    }

    SendHandleC::SendHandleC( base::DataSourceBase::shared_ptr sh, OperationRepositoryPart* ofp, const string& name )
        : d( ofp ? new D( sh, ofp, name ) : 0 ), s(), b()
    {
        if ( d->s ) {
            this->s = d->s;
            this->b = d->blocking;
            delete d;
            d = 0;
        }
        this->orp = ofp;
    }

    SendHandleC::SendHandleC(const SendHandleC& other)
        : d( other.d ? new D(*other.d) : 0 ), s( other.s ? other.s : 0), b( other.b ? other.b : 0), orp( other.orp ? other.orp : NULL)
    {
    }

    SendHandleC& SendHandleC::operator=(const SendHandleC& other)
    {
        if ( &other == this )
            return *this;
        delete d;
        d = ( other.d ? new D(*other.d) : 0 );
        s = other.s;
        b = other.b;
        orp = other.orp;
        return *this;
    }

    SendHandleC::~SendHandleC()
    {
        delete d;
    }

    SendHandleC& SendHandleC::arg( DataSourceBase::shared_ptr a )
    {
        if (d)
            d->newarg( a );
        else {
            Logger::log() <<Logger::Warning << "Extra argument discarded for SendHandleC."<<Logger::endl;
        }
        if ( d && d->s ) {
            this->s = d->s;
            this->b = d->blocking;
            this->orp = d->mofp;
            delete d;
            d = 0;
        }
        return *this;
    }

    SendStatus SendHandleC::collect() {
        if (s) {
            b->set(true); // blocking
            s->evaluate();
            return s->value();
        }
        else {
            Logger::log() <<Logger::Error << "call() called on incomplete SendHandleC."<<Logger::endl;
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
        if (s) {
            b->set(false); // non blocking
            // does the send.
            s->evaluate();
            // pass on handle.
            return s->value();
        }
        else {
            Logger::log() <<Logger::Error << "send() called on incomplete SendHandleC."<<Logger::endl;
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
        return s;
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


    DataSourceBase::shared_ptr SendHandleC::getSendHandleDataSource() { return s; }

    OperationRepositoryPart* SendHandleC::getOrp() { return orp; }
}
