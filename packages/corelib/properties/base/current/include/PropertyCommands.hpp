#ifndef ORO_CORELIB_PROPERTY_COMMANDS
#define ORO_CORELIB_PROPERTY_COMMANDS

#include "CommandInterface.hpp"
#include "DataSource.hpp"

namespace ORO_CoreLib
{

    namespace detail {

        /**
         * Update a Property<T>.
         */
        template<class T>
        class UpdatePropertyCommand
            : public CommandInterface
        {
            Property<T>* target;
            const Property<T>* source;
        public:
            UpdatePropertyCommand( Property<T>* tgt, const Property<T>* src)
                : target(tgt), source(src) {}
            bool execute()
            {
                target->update( *source );
                return true;
            }

            virtual UpdatePropertyCommand<T>* clone() const {
                return new UpdatePropertyCommand<T>(target, source);
            }
        };

        /**
         * Copy a Property<T>.
         */
        template<class T>
        class CopyPropertyCommand
            : public CommandInterface
        {
            Property<T>* target;
            const Property<T>* source;
        public:
            CopyPropertyCommand( Property<T>* tgt, const Property<T>* src)
                : target(tgt), source(src) {}
            bool execute()
            {
                target->copy( *source );
                return true;
            }

            virtual CopyPropertyCommand<T>* clone() const {
                return new CopyPropertyCommand<T>(target, source);
            }
        };

        /**
         * Refresh a Property<T>.
         */
        template<class T>
        class RefreshPropertyCommand
            : public CommandInterface
        {
            Property<T>* target;
            const Property<T>* source;
        public:
            RefreshPropertyCommand( Property<T>* tgt, const Property<T>* src)
                : target(tgt), source(src) {}
            bool execute()
            {
                target->refresh( *source );
                return true;
            }

            virtual RefreshPropertyCommand<T>* clone() const {
                return new RefreshPropertyCommand<T>(target, source);
            }
        };

        /**
         * Refresh a Property<T> with a DataSource<T>.
         */
        template<class T>
        class RefreshPropertyFromDSCommand
            : public CommandInterface
        {
            Property<T>* target;
            typename DataSource<T>::shared_ptr source;
        public:
            RefreshPropertyFromDSCommand( Property<T>* tgt, DataSource<T>* src)
                : target(tgt), source(src) {}
            bool execute()
            {
                target->refresh( *source );
                return true;
            }

            virtual RefreshPropertyFromDSCommand<T>* clone() const {
                return new RefreshPropertyFromDSCommand<T>(target, source.get() );
            }

            virtual RefreshPropertyFromDSCommand<T>* copy( std::map<const DataSourceBase*, DataSourceBase*>& alreadyCloned ) const {
                return new RefreshPropertyFromDSCommand<T>( target, source->copy(alreadyCloned) );
            }
        };

    }

}

#endif
