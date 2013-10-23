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


#ifndef RTT_INTERNAL_BINARYDATASOURCE_HPP
#define RTT_INTERNAL_BINARYDATASOURCE_HPP

#include "DataSource.hpp"

namespace RTT
{
    namespace internal {
  /**
   * A generic binary composite DataSource.  It takes a function
   * object which is a model of the STL Adaptable Binary Function
   * concept, and two DataSource's with result types matching the
   * argument types of the given function.  Its get() method then
   * applies the function to the results of the get() methods of the
   * two DataSource's given.
   *
   * The parser system uses a hell of a lot of these for evaluating
   * expressions, comparisons etc.  I must say I think this is pretty
   * cool ;)  C++ rocks ! ;)
   */
  template<typename function>
  class BinaryDataSource
    : public DataSource< typename remove_cr<typename function::result_type>::type >
  {
    typedef typename remove_cr<typename function::result_type>::type value_t;
    typedef typename remove_cr<typename function::first_argument_type>::type  first_arg_t;
    typedef typename remove_cr<typename function::second_argument_type>::type second_arg_t;
    typename DataSource<first_arg_t>::shared_ptr mdsa;
    typename DataSource<second_arg_t>::shared_ptr mdsb;
    function fun;
    mutable value_t mdata;
  public:
    typedef boost::intrusive_ptr<BinaryDataSource<function> > shared_ptr;

      /**
       * Create a DataSource which returns the return value of a function
       * \a f which is given argument \a a and \a b.
       */
    BinaryDataSource( typename DataSource<first_arg_t>::shared_ptr a,
                      typename DataSource<second_arg_t>::shared_ptr b,
                      function f )
      : mdsa( a ), mdsb( b ), fun( f )
      {
      }

    virtual value_t get() const
      {
        first_arg_t a = mdsa->get();
        second_arg_t b = mdsb->get();
        return mdata = fun( a, b );
      }

    virtual value_t value() const
      {
        return mdata;
      }

    typename DataSource<value_t>::const_reference_t rvalue() const
    {
        return mdata;
    }

    virtual void reset()
      {
        mdsa->reset();
        mdsb->reset();
      }

      virtual BinaryDataSource<function>* clone() const
      {
          return new BinaryDataSource<function>(mdsa.get(), mdsb.get(), fun);
      }

      virtual BinaryDataSource<function>* copy( std::map<const base::DataSourceBase*, base::DataSourceBase*>& alreadyCloned ) const {
          return new BinaryDataSource<function>( mdsa->copy( alreadyCloned ), mdsb->copy( alreadyCloned ), fun );
      }
  };

    }
}
#endif


