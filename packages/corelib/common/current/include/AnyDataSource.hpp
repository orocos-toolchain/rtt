#ifndef ORO_CORELIB_ANY_DATASOURCE_HPP
#define ORO_CORELIB_ANY_DATASOURCE_HPP

#include "DataSource.hpp"
#include <tao/Any.h>

namespace ORO_CoreLib
{
    namespace detail {
        /**
         * A DataSource which wraps another DataSource into a CORBA::Any.
         */
        class AnyDataSource
            : public DataSource<CORBA::Any_ptr>
        {
            DataSourceBase::const_ptr source;
        public:
            AnyDataSource( const DataSourceBase* orig )
                : source(orig)
            {}

            DataSource<CORBA::Any_ptr>::result_t get() const
            {
                Logger::log() << Logger::Debug << "AnyDataSource::get() returns Corba::Any of "<< source->getType() <<Logger::endl;
                return source->getAny();
            }

            DataSource<CORBA::Any_ptr>::result_t value() const
            {
                return source->createAny();
            }

            virtual AnyDataSource* clone() const 
            {
                return new AnyDataSource(source.get());
            }

            virtual AnyDataSource* copy( std::map<const DataSourceBase*, DataSourceBase*>& alreadyCloned ) const 
            {
                return new AnyDataSource( source->copy( alreadyCloned ) );
            }
        };
    }
}

#endif
#ifndef ORO_CORELIB_ANY_DATASOURCE_HPP
#define ORO_CORELIB_ANY_DATASOURCE_HPP

#include "DataSource.hpp"
#include <tao/Any.h>

namespace ORO_CoreLib
{
    namespace detail {
        /**
         * A DataSource which wraps another DataSource into a CORBA::Any.
         */
        class AnyDataSource
            : public DataSource<CORBA::Any_ptr>
        {
            DataSourceBase::const_ptr source;
        public:
            AnyDataSource( const DataSourceBase* orig )
                : source(orig)
            {}

            DataSource<CORBA::Any_ptr>::result_t get() const
            {
                Logger::log() << Logger::Debug << "AnyDataSource::get() returns Corba::Any of "<< source->getType() <<Logger::endl;
                return source->getAny();
            }

            DataSource<CORBA::Any_ptr>::result_t value() const
            {
                return source->createAny();
            }

            virtual AnyDataSource* clone() const 
            {
                return new AnyDataSource(source.get());
            }

            virtual AnyDataSource* copy( std::map<const DataSourceBase*, DataSourceBase*>& alreadyCloned ) const 
            {
                return new AnyDataSource( source->copy( alreadyCloned ) );
            }
        };
    }
}

#endif
#ifndef ORO_CORELIB_ANY_DATASOURCE_HPP
#define ORO_CORELIB_ANY_DATASOURCE_HPP

#include "DataSource.hpp"
#include <tao/Any.h>

namespace ORO_CoreLib
{
    namespace detail {
        /**
         * A DataSource which wraps another DataSource into a CORBA::Any.
         */
        class AnyDataSource
            : public DataSource<CORBA::Any_ptr>
        {
            DataSourceBase::const_ptr source;
        public:
            AnyDataSource( const DataSourceBase* orig )
                : source(orig)
            {}

            DataSource<CORBA::Any_ptr>::result_t get() const
            {
                Logger::log() << Logger::Debug << "AnyDataSource::get() returns Corba::Any of "<< source->getType() <<Logger::endl;
                return source->getAny();
            }

            DataSource<CORBA::Any_ptr>::result_t value() const
            {
                return source->createAny();
            }

            virtual AnyDataSource* clone() const 
            {
                return new AnyDataSource(source.get());
            }

            virtual AnyDataSource* copy( std::map<const DataSourceBase*, DataSourceBase*>& alreadyCloned ) const 
            {
                return new AnyDataSource( source->copy( alreadyCloned ) );
            }
        };
    }
}

#endif
#ifndef ORO_CORELIB_ANY_DATASOURCE_HPP
#define ORO_CORELIB_ANY_DATASOURCE_HPP

#include "DataSource.hpp"
#include <tao/Any.h>

namespace ORO_CoreLib
{
    namespace detail {
        /**
         * A DataSource which wraps another DataSource into a CORBA::Any.
         */
        class AnyDataSource
            : public DataSource<CORBA::Any_ptr>
        {
            DataSourceBase::const_ptr source;
        public:
            AnyDataSource( const DataSourceBase* orig )
                : source(orig)
            {}

            DataSource<CORBA::Any_ptr>::result_t get() const
            {
                Logger::log() << Logger::Debug << "AnyDataSource::get() returns Corba::Any of "<< source->getType() <<Logger::endl;
                return source->getAny();
            }

            DataSource<CORBA::Any_ptr>::result_t value() const
            {
                return source->createAny();
            }

            virtual AnyDataSource* clone() const 
            {
                return new AnyDataSource(source.get());
            }

            virtual AnyDataSource* copy( std::map<const DataSourceBase*, DataSourceBase*>& alreadyCloned ) const 
            {
                return new AnyDataSource( source->copy( alreadyCloned ) );
            }
        };
    }
}

#endif
