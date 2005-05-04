#include "DataSource.hpp"


namespace ORO_CoreLib
{
    template< class T>
    class Property;

    namespace detail {
        /**
         * An AssignableDataSource which wraps around a Property,
         * effectively making the Property available as a DataSource.
         * Copy semantics : any copy of this DataSource will always point
         * to the original Property<T> given upon construction.
         */
        template<typename T>
        class PropertyDataSource
            : public AssignableDataSource<T>
        {
            Property<T>* prop;
        public:

            typedef boost::intrusive_ptr<PropertyDataSource<T> > shared_ptr;

            PropertyDataSource( Property<T>* p )
                : AssignableDataSource<T>(), prop(p)
            {
            }

            typename DataSource<T>::result_t get() const
            {
                return prop->get();
            }

            void set( typename AssignableDataSource<T>::param_t t )
            {
                prop->set(t);
            }

            typename AssignableDataSource<T>::reference_t set() {
                return prop->set();
            }

            virtual PropertyDataSource<T>* clone() const
            {
                return new PropertyDataSource<T>( prop );
            }

            virtual PropertyDataSource<T>* copy( std::map<const DataSourceBase*, DataSourceBase*>& replace) {
                // if somehow a copy exists, return the copy, otherwise return this.
                if ( replace[this] != 0 ) {
                    assert( dynamic_cast<PropertyDataSource<T>*>( replace[this] ) );
                    return static_cast<PropertyDataSource<T>*>( replace[this] );
                }
              
                // this line required that copy is non const !
                replace[this] = this;
                // return this.
                return this;
            }
        };
    }
}
