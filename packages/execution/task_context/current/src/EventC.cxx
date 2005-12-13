#include "execution/EventService.hpp"
#include "execution/EventC.hpp"
#include "execution/FactoryExceptions.hpp"
#include <vector>

namespace ORO_Execution
{
    
    class EventC::D
    {
    public:
        const EventService* mgcf;
        std::string mname;
        std::vector<DataSourceBase::shared_ptr> args;
        DataSourceBase::shared_ptr m;

        void checkAndCreate() {
            size_t sz = mgcf->arity(mname);
            if ( m )
                throw wrong_number_of_args_exception( sz, sz + 1 );
            if ( mgcf->hasEvent(mname) && sz == args.size() ) {
                // may throw.
                m = mgcf->setupEmit(mname, args );
                args.clear();
            }
        }

        void newarg(DataSourceBase::shared_ptr na)
        {
            this->args.push_back( na );
            this->checkAndCreate();
        }

        D( const EventService* gcf, const std::string& name)
            : mgcf(gcf), mname(name), m()
        {
            this->checkAndCreate();
        }

        D(const D& other)
            : mgcf( other.mgcf), mname(other.mname),
              args( other.args ), m( other.m )
        {
        }

        ~D()
        {
        }

        void emit() {
            if (m)
                m->evaluate();
        }

    };

    EventC::EventC(const EventService* gcf, const std::string& name)
        : d( new D( gcf, name) ), m()
    {
        if (d->m) {
            this->m = d->m;
            delete d;
            d = 0;
        }
    }

    EventC::EventC()
        : d( 0 ), m()
    {
    }

    EventC::EventC(const EventC& other)
        : d( other.d ? new D(*other.d) : 0 ), m(other.m)
    {
    }

    EventC::~EventC()
    {
        delete d;
    }

    EventC& EventC::arg( DataSourceBase::shared_ptr a )
    {
        if (d)
            d->newarg( a );

        if (d && d->m) {
            this->m = d->m;
            delete d;
            d = 0;
        }
            
        return *this;
    }

    void EventC::emit() {
        if (m)
            m->evaluate();
    }
}
