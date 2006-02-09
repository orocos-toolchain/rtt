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
#include "execution/MethodC.hpp"
#include "execution/FactoryExceptions.hpp"
#include "execution/DataSourceCommand.hpp"
#include <vector>

namespace ORO_Execution
{
    
    class MethodC::D
    {
        const GlobalMethodFactory* mgcf;
        std::string mobject, mname;
        std::vector<DataSourceBase::shared_ptr> args;
        TaskAttributeBase* rta;
    public:
        DataSourceBase::shared_ptr m;

        void checkAndCreate() {
            // check validity:
            if ( mgcf == 0 || mgcf->getObjectFactory(mobject) == 0 )
                throw name_not_found_exception(mobject);
            if ( ! mgcf->getObjectFactory(mobject)->hasMember(mname) )
                throw name_not_found_exception(mname);
            size_t sz = mgcf->getObjectFactory(mobject)->getArgumentList(mname).size();
            // throw if already created.
            if ( m )
                throw wrong_number_of_args_exception( sz, sz + 1 );
            // not created, check if it is time to create:
            if ( mgcf->hasMember(mobject, mname) && sz == args.size() ) {
                // may throw.
                m = mgcf->getObjectFactory(mobject)->create(mname, args );
                args.clear();
                if (rta)
                    m = new DataSourceCommand( rta->assignCommand( m, false ) );
            }
        }

        void newarg(DataSourceBase::shared_ptr na)
        {
            this->args.push_back( na );
            this->checkAndCreate();
        }

        void ret(TaskAttributeBase* r)
        {
            this->rta = r->clone();
        }

        D( const GlobalMethodFactory* gcf, const std::string& obj, const std::string& name)
            : mgcf(gcf), mobject(obj), mname(name), rta(0), m()
        {
            this->checkAndCreate();
        }

        D(const D& other)
            : mgcf( other.mgcf), mobject(other.mobject), mname(other.mname),
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
        : d( new D( gcf, obj, name) ), m()
    {
        if ( d->m ) {
            this->m = d->m;
            delete d;
            d = 0;
        }
    }

    MethodC::MethodC(const MethodC& other)
        : d( other.d ? new D(*other.d) : 0 ), m( d->m ? d->m : 0)
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
        if ( d && d->m ) {
            this->m = d->m;
            delete d;
            d = 0;
        }
        return *this;
    }

    MethodC& MethodC::ret( TaskAttributeBase* r )
    {
        if (d)
            d->ret( r );
        else {
            m = new DataSourceCommand(r->assignCommand( m, false ) );
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
        return false;
    }

    void MethodC::reset()
    {
        if (m)
            m->reset();
    }
}
