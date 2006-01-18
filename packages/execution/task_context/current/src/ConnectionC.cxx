/***************************************************************************
  tag: Peter Soetens  Wed Jan 18 14:11:40 CET 2006  ConnectionC.cxx 

                        ConnectionC.cxx -  description
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
