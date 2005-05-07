/***************************************************************************
  tag: Peter Soetens  Sat May 7 12:56:52 CEST 2005  PropertyDataSource.hpp 

                        PropertyDataSource.hpp -  description
                           -------------------
    begin                : Sat May 07 2005
    copyright            : (C) 2005 Peter Soetens
    email                : peter.soetens@mech.kuleuven.ac.be
 
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
