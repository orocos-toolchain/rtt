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
        template<class T, class S = T>
        class UpdatePropertyCommand
            : public CommandInterface
        {
            Property<T>* target;
            const Property<S>* source;
        public:
            UpdatePropertyCommand( Property<T>* tgt, const Property<S>* src)
                : target(tgt), source(src) {}
            bool execute()
            {
                target->update( *source );
                return true;
            }

            virtual UpdatePropertyCommand<T,S>* clone() const {
                return new UpdatePropertyCommand<T,S>(target, source);
            }
        };

        /**
         * Copy a Property<T>.
         */
        template<class T, class S = T>
        class CopyPropertyCommand
            : public CommandInterface
        {
            Property<T>* target;
            const Property<S>* source;
        public:
            CopyPropertyCommand( Property<T>* tgt, const Property<S>* src)
                : target(tgt), source(src) {}
            bool execute()
            {
                target->copy( *source );
                return true;
            }

            virtual CopyPropertyCommand<T,S>* clone() const {
                return new CopyPropertyCommand<T,S>(target, source);
            }
        };

        /**
         * Refresh a Property<T>.
         */
        template<class T, class S = T>
        class RefreshPropertyCommand
            : public CommandInterface
        {
            Property<T>* target;
            const Property<S>* source;
        public:
            RefreshPropertyCommand( Property<T>* tgt, const Property<S>* src)
                : target(tgt), source(src) {}
            bool execute()
            {
                target->refresh( *source );
                return true;
            }

            virtual RefreshPropertyCommand<T,S>* clone() const {
                return new RefreshPropertyCommand<T,S>(target, source);
            }
        };

        /**
         * Refresh a Property<T> with a DataSource<S>.
         */
        template<class T, class S = T>
        class RefreshPropertyFromDSCommand
            : public CommandInterface
        {
            Property<T>* target;
            typename DataSource<S>::shared_ptr source;
        public:
            RefreshPropertyFromDSCommand( Property<T>* tgt, DataSource<S>* src)
                : target(tgt), source(src) {}
            bool execute()
            {
                target->refresh( *source );
                return true;
            }

            virtual RefreshPropertyFromDSCommand<T,S>* clone() const {
                return new RefreshPropertyFromDSCommand<T,S>(target, source.get() );
            }

            virtual RefreshPropertyFromDSCommand<T,S>* copy( std::map<const DataSourceBase*, DataSourceBase*>& alreadyCloned ) const {
                return new RefreshPropertyFromDSCommand<T,S>( target, source->copy(alreadyCloned) );
            }
        };

    }

}

#endif
