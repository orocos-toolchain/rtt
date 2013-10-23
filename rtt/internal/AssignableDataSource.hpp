/***************************************************************************
  tag: Peter Soetens  Mon Jan 19 14:11:25 CET 2004  DataSource.hpp

                        DataSource.hpp -  description
                           -------------------
    begin                : Mon January 19 2004
    copyright            : (C) 2004 Peter Soetens
    email                : peter.soetens@mech.kuleuven.ac.be

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

#ifndef RTT_INTERNAL_ASSIGNABLEDATASOURCE_HPP
#define RTT_INTERNAL_ASSIGNABLEDATASOURCE_HPP

#include "DataSource.hpp"
#include "DataSourceTypeInfo.hpp"
#include <boost/call_traits.hpp>
#include <string>
#include <exception>
#include "../rtt-config.h"

namespace RTT
{ namespace internal {

#ifndef ORO_EMBEDDED
    /**
     * This exception is thrown if the target and source type
     * of an assignment of a DataSource with a base::DataSourceBase
     * differ.
     */
    struct RTT_API bad_assignment : public std::exception
    {
        virtual ~bad_assignment() throw() {}
        virtual const char* what() const throw() { return "Bad DataSource assignment: incompatible types."; }
    };
#endif

  /**
   * A DataSource which has set() methods.
   * @param T See DataSource for the semantics of \a T.
   */
  template<typename T>
  class AssignableDataSource
    : public DataSource<T>
  {
  protected:
      ~AssignableDataSource();
  public:
      typedef typename DataSource<T>::value_t value_t;
      typedef typename DataSource<T>::const_reference_t const_reference_t;
      typedef typename boost::call_traits<value_t>::param_type param_t;
      typedef typename boost::call_traits<value_t>::reference reference_t;

      /**
       * Use this type to store a pointer to an AssignableDataSource.
       */
      typedef boost::intrusive_ptr<AssignableDataSource<T> > shared_ptr;
      typedef typename boost::intrusive_ptr<const AssignableDataSource<T> > const_ptr;

      /**
       * Set this DataSource with a value.
       */
      virtual void set( param_t t ) = 0;

      /**
       * Get a reference to the value of this DataSource.
       * Getting a reference to an internal data structure is not thread-safe.
       */
      virtual reference_t set() = 0;

      /** Generic implementation of getRawPointer() based on set()
       */
      void* getRawPointer() { return &set(); }

      virtual bool isAssignable() const { return true; }

      virtual bool update( base::DataSourceBase* other );

      virtual base::ActionInterface* updateAction( base::DataSourceBase* other);

      virtual AssignableDataSource<T>* clone() const = 0;

      virtual AssignableDataSource<T>* copy( std::map<const base::DataSourceBase*, base::DataSourceBase*>& alreadyCloned ) const = 0;

      /**
       * This method narrows a base::DataSourceBase to a typeded AssignableDataSource,
       * possibly returning a new object.
       */
      static AssignableDataSource<T>* narrow(base::DataSourceBase* db);

  };

    template<typename T>
    AssignableDataSource<T>::~AssignableDataSource()
    {}

    template<class T>
    DataSource<T>* DataSource<T>::narrow(base::DataSourceBase* dsb) {
        DataSource<T>* ret = dynamic_cast< DataSource<T>* >( dsb );
	return ret;
    }

    template<class T>
    AssignableDataSource<T>* AssignableDataSource<T>::narrow(base::DataSourceBase* dsb) {
        AssignableDataSource<T>* ret = dynamic_cast< AssignableDataSource<T>* >( dsb );
	return ret;
    }

    template<class T>
    bool AssignableDataSource<T>::update( base::DataSourceBase* other ) {
        if (!other) return false;

        base::DataSourceBase::shared_ptr r( other );
        typename DataSource<T>::shared_ptr o = boost::dynamic_pointer_cast< DataSource<T> >( DataSourceTypeInfo<T>::getTypeInfo()->convert(r) );
        if (o) {
            if ( o->evaluate() ) {
                this->set( o->value() );
                return true;
            }
            return false;
        }
        return false;
    }
}}

#include "AssignCommand.hpp"

namespace RTT
{ namespace internal {
    template<class T>
    base::ActionInterface* AssignableDataSource<T>::updateAction( base::DataSourceBase* other) {
#ifndef ORO_EMBEDDED
        if (!other) throw bad_assignment();
#else
        if (!other) return 0;
#endif

        // Use the same rules of parameter passing as C++, but no const for 'int',...
        base::DataSourceBase::shared_ptr r( other );
        typename DataSource<value_t>::shared_ptr t = boost::dynamic_pointer_cast< DataSource<value_t> >( DataSourceTypeInfo<T>::getTypeInfo()->convert(r) );
        if ( t )
            return new AssignCommand<value_t>( this, t );

#ifndef ORO_EMBEDDED
        throw bad_assignment();
#endif

        return 0;
    }
}}

#endif

