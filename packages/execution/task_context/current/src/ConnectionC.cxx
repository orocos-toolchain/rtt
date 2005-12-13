#include "execution/EventService.hpp"
#include "execution/ConnectionC.hpp"
#include "execution/FactoryExceptions.hpp"
#include <corelib/EventProcessor.hpp>
#include <corelib/Logger.hpp>
#include <vector>

namespace ORO_Execution
{
    using namespace ORO_CoreLib;
    
    class ConnectionC::D
    {
    public:
        const EventService* mgcf;
        std::string mname;
        std::vector<DataSourceBase::shared_ptr> args;
        boost::function<void(void)> mfunc;
        EventProcessor* mep;
        Handle h;

        void checkAndCreate() {
            int sz = mgcf->arity(mname);
            if ( sz == -1 )
                throw name_not_found_exception(mname);
            if ( h )
                throw wrong_number_of_args_exception( sz, sz + 1 );
            if ( mgcf->hasEvent(mname) && size_t(sz) == args.size() ) {
                Logger::log() << Logger::Debug << "Creating connection to "+ mname +"."<<Logger::endl;
                // may throw.
                if ( mep )
                    h = mgcf->setupAsyn(mname, mfunc, args, mep );
                else
                    h = mgcf->setupSyn(mname, mfunc, args);
                args.clear();
            }
        }

        void newarg(DataSourceBase::shared_ptr na)
        {
            this->args.push_back( na );
            this->checkAndCreate();
        }

        D( const EventService* gcf, const std::string& name,
           boost::function<void(void)> func, EventProcessor* ep)
            : mgcf(gcf), mname(name), mfunc(func), mep(ep), h()
        {
            this->checkAndCreate();
        }

        D(const D& other)
            : mgcf( other.mgcf), mname(other.mname),
              args( other.args ), mfunc(other.mfunc), h( other.h )
        {
        }

        ~D()
        {
        }

        Handle handle() const {
            return h;
        }

    };

    ConnectionC::ConnectionC()
    : d(0), h()
    {}
    
    ConnectionC::ConnectionC(const EventService* gcf, const std::string& name,
                             boost::function<void(void)> func, EventProcessor* ep)
        : d( new D( gcf, name, func, ep) ), h()
    {
        h = d->handle();
        if (h) {
            delete d;
            d = 0;
        }
    }

    ConnectionC::ConnectionC(const ConnectionC& other)
        : d( other.d ? new D(*other.d) : 0 ), h( other.h )
    {
    }

    ConnectionC::~ConnectionC()
    {
        delete d;
    }

    ConnectionC& ConnectionC::arg( DataSourceBase::shared_ptr a )
    {
        if (d)
            d->newarg( a );
        if ( d && d->handle() ) {
            h = d->handle();
            Logger::log() << Logger::Debug << "Connection to "+ d->mname +" Created."<<Logger::endl;
            delete d;
            d = 0;
        }
        return *this;
    }

    Handle ConnectionC::handle() const {
        return h;
    }
}
