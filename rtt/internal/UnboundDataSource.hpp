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


#ifndef RTT_INTERNAL_UNBOUNDDATASOURCE_HPP
#define RTT_INTERNAL_UNBOUNDDATASOURCE_HPP

#include "../base/DataSourceBase.hpp"

namespace RTT
{
    namespace internal {
        /**
         * A special DataSource only to be used for if you understand
         * the copy()/clone() semantics very well.
         *
         * UnboundDataSource is required to represent variables ('var') in program scripts.
         * Otherwise copy/clone done in ParsedStateMachine will not work as expected.
         *
         * It has different copy semantics in comparison to 'bound' DataSources
         * (like ValueDataSource), being that, it returns new copies in copy(),
         * and returns a clone() of the contained datasource in clone().
         * A clone() reduces this 'UnboundDataSource' to a 'BoundType' data source.
         * @param BoundType The type to inherit from and to return in clone().
         * Typically a ValueDataSource or IndexedValueDataSource type.
         */
        template<typename BoundType>
        class UnboundDataSource
            : public BoundType
        {
        public:
            typedef typename BoundType::result_t T;
            typedef boost::intrusive_ptr< UnboundDataSource<BoundType> > shared_ptr;

            UnboundDataSource( T data );

            UnboundDataSource( );

            ~UnboundDataSource() {
            }

            virtual BoundType* clone() const {
                return BoundType::clone();
            }

            virtual UnboundDataSource<BoundType>* copy( std::map<const base::DataSourceBase*, base::DataSourceBase*>& replace) const;
        };

        template< typename BoundT>
        UnboundDataSource<BoundT>::UnboundDataSource( typename BoundT::result_t data )
            : BoundT( data )
        {
        }

        template< typename BoundT>
        UnboundDataSource<BoundT>::UnboundDataSource( )
        {
        }

        template< typename BoundT>
        UnboundDataSource<BoundT>* UnboundDataSource<BoundT>::copy( std::map<const base::DataSourceBase*, base::DataSourceBase*>& replace) const {
            if ( replace[this] != 0 )
                return static_cast<UnboundDataSource<BoundT>*>(replace[this]);
            replace[this] = new UnboundDataSource<BoundT>( this->get() );
            return static_cast<UnboundDataSource<BoundT>*>(replace[this]);
        }
    }
}
#endif


