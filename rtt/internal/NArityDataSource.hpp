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


#ifndef RTT_INTERNAL_NARITYDATASOURCE_HPP
#define RTT_INTERNAL_NARITYDATASOURCE_HPP

#include "DataSource.hpp"
#include "bits/remove_cr.hpp"

namespace RTT
{
    namespace internal {
  /**
   * A generic N-arity composite DataSource. It collects data sources of the same type
   * and passes the values found on to \a function by means of a std::vector.
   * @param function Is a struct which defines value_t operator()(const std::vector<arg_t>&),
   * where \a value_t is defined as \a function::result_type
   * and \a arg_t is defined as \a function::argument_type.
   */
  template<typename function>
  class NArityDataSource
    : public DataSource<typename remove_cr<typename function::result_type>::type>
  {
      typedef typename remove_cr<typename function::result_type>::type value_t;
      typedef typename remove_cr<typename function::argument_type>::type  arg_t;
      mutable std::vector<arg_t> margs;
      std::vector<typename DataSource<arg_t>::shared_ptr > mdsargs;
      function fun;
      mutable value_t mdata;
  public:
      typedef boost::intrusive_ptr<NArityDataSource<function> > shared_ptr;

      /**
       * Create a DataSource which returns the return value of a function
       * \a f.
       */
      NArityDataSource( function f = function() )
          : fun( f )
      {
      }

      /**
       * Create a DataSource which returns the return value of a function
       * \a f.
       */
      NArityDataSource( function f, const std::vector<typename DataSource<arg_t>::shared_ptr >& dsargs )
          : margs( dsargs.size() ), mdsargs(dsargs), fun( f )
      {
      }

      void add( typename DataSource<arg_t>::shared_ptr ds ) {
          mdsargs.push_back(ds);
          margs.push_back( ds->value() );
      }

      virtual value_t get() const
      {
          assert( mdsargs.size() == margs.size() );
          for( unsigned int i=0; i !=mdsargs.size(); ++i)
              margs[i] = mdsargs[i]->get();
          return mdata = fun( margs );
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
          for( unsigned int i=0; i !=mdsargs.size(); ++i)
              mdsargs[i]->reset();
      }

      virtual NArityDataSource<function>* clone() const
      {
          return new NArityDataSource<function>(fun, mdsargs);
      }

      virtual NArityDataSource<function>* copy( std::map<const base::DataSourceBase*, base::DataSourceBase*>& alreadyCloned ) const {
          std::vector<typename DataSource<arg_t>::shared_ptr > newargs( mdsargs.size() );
          for( unsigned int i=0; i !=mdsargs.size(); ++i)
              newargs[i] = mdsargs[i]->copy(alreadyCloned);
          return new NArityDataSource<function>( fun, newargs );
      }
  };
    }
}
#endif


