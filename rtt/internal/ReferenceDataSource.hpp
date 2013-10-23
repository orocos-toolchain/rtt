/***************************************************************************
  tag: Peter Soetens  Mon Jun 26 13:25:56 CEST 2006  DataSources.hpp

                        DataSources.hpp -  description
                           -------------------
    begin                : Mon June 26 2006
    copyright            : (C) 2006 Peter Soetens
    email                : peter.soetens@fmtc.be

 ***************************************************************************
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU General Public                   *
 *   License as published by the Free Software Foundation;                 *
 *   version 2 of the License.                                             *
 *                                                                         *
 *   As a special exception, you may use this file as part of a free       *
 *   software library without restriction.  Specifically, if other files   *
 *   instantiate templates or use macros or inline functions from this     *
 *   file, or you compile this file and link it with other files to        *
 *   produce an executable, this file does not by itself cause the         *
 *   resulting executable to be covered by the GNU General Public          *
 *   License.  This exception does not however invalidate any other        *
 *   reasons why the executable file might be covered by the GNU General   *
 *   Public License.                                                       *
 *                                                                         *
 *   This library is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU     *
 *   Lesser General Public License for more details.                       *
 *                                                                         *
 *   You should have received a copy of the GNU General Public             *
 *   License along with this library; if not, write to the Free Software   *
 *   Foundation, Inc., 59 Temple Place,                                    *
 *   Suite 330, Boston, MA  02111-1307  USA                                *
 *                                                                         *
 ***************************************************************************/


#ifndef RTT_INTERNAL_REFERENCEDATASOURCE_HPP
#define RTT_INTERNAL_REFERENCEDATASOURCE_HPP

#include "AssignableDataSource.hpp"
#include "Reference.hpp"

namespace RTT
{
    namespace internal {
    /**
     * A DataSource which is used to manipulate a reference to an
     * external value.
     * @param T The result data type of get().
     */
    template<typename T>
    class ReferenceDataSource
	    : public AssignableDataSource<T>, public Reference
    {
        // a pointer to a value_t
        T* mptr;
    public:
        /**
         * Use shared_ptr.
         */
        ~ReferenceDataSource();

        typedef boost::intrusive_ptr<ReferenceDataSource<T> > shared_ptr;

        ReferenceDataSource( typename AssignableDataSource<T>::reference_t ref );

        void setReference(void* ref)
        {
            mptr = static_cast<T*>(ref);
        }
        bool setReference(base::DataSourceBase::shared_ptr dsb)
        {
            typename AssignableDataSource<T>::shared_ptr ads = boost::dynamic_pointer_cast<AssignableDataSource<T> >(dsb);
            if (ads) {
		    ads->evaluate();
		    mptr = &ads->set();
		    return true;
	    } else {
		    return false;
	    }
        }

        typename DataSource<T>::result_t get() const
		{
			return *mptr;
		}

        typename DataSource<T>::result_t value() const
		{
			return *mptr;
		}

        void set( typename AssignableDataSource<T>::param_t t );

        typename AssignableDataSource<T>::reference_t set()
		{
			return *mptr;
		}

        typename AssignableDataSource<T>::const_reference_t rvalue() const
		{
			return *mptr;
		}

        virtual ReferenceDataSource<T>* clone() const;

        virtual ReferenceDataSource<T>* copy( std::map<const base::DataSourceBase*, base::DataSourceBase*>& alreadyCloned ) const;
    };

    template<typename T>
    ReferenceDataSource<T>::~ReferenceDataSource() {}

    template<typename T>
    ReferenceDataSource<T>::ReferenceDataSource( typename AssignableDataSource<T>::reference_t ref )
        : mptr( &ref )
    {
    }
    template<typename T>
    void ReferenceDataSource<T>::set( typename AssignableDataSource<T>::param_t t )
    {
        *mptr = t;
    }

    template<typename T>
    ReferenceDataSource<T>* ReferenceDataSource<T>::clone() const
    {
        return new ReferenceDataSource<T>(*mptr);
    }

    template<typename T>
    ReferenceDataSource<T>* ReferenceDataSource<T>::copy( std::map<const base::DataSourceBase*, base::DataSourceBase*>& alreadyCloned ) const {
        return const_cast<ReferenceDataSource<T>*>(this); // no copy needed, data is outside.
    }

    }
}
#endif

