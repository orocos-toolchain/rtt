/***************************************************************************
  tag: Peter Soetens  Wed Jul 28 09:08:56 CEST 2004  mystd.hpp

                        mystd.hpp -  description
                           -------------------
    begin                : Wed July 28 2004
    copyright            : (C) 2004 Peter Soetens
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

#ifndef MYSTD_HPP
#define MYSTD_HPP

#include <boost/type_traits.hpp>
#include <functional>
#include <algorithm>
#include <vector>

// here we define some generally useful template stuff that is missing
// from the STL..
namespace mystd {

    // combines remove_reference and remove_const
    template<typename T>
    struct remove_cr
    {
      typedef typename boost::remove_const<
        typename boost::remove_reference<T>::type>::type type;
    };

    // SGI extension, does not seem present in current GNU STL
    // implementation...

    template<typename T>
    struct select1st
      : public std::unary_function<T, typename T::first_type>
    {
        typename T::first_type operator()( const T& p ) {
            return p.first;
        }
    };

    template<typename T>
    struct select2nd
      : public std::unary_function<T, typename T::second_type>
    {
        typename T::second_type operator()( const T& p ) {
            return p.second;
        }
    };

#if 0
    // Alternative implementations, return const ref.
    template<typename PairT>
    class select1st
      : public std::unary_function<PairT, typename PairT::first_type>
    {
      typedef typename PairT::first_type ResultT;
    public:
      const ResultT& operator()( const PairT& p )
        {
          return p.first;
        };
    };

    template<typename PairT>
    class select2nd
      : public std::unary_function<PairT, typename PairT::second_type>
    {
      typedef typename PairT::second_type ResultT;
    public:
      const ResultT& operator()( const PairT& p )
        {
          return p.second;
        };
    };
#endif


    // my own handy little extension..
    template<typename MapT>
    std::vector<typename MapT::mapped_type> values( const MapT& map )
    {
        std::vector<typename MapT::mapped_type> ret;
        ret.reserve( map.size() );
        std::transform( map.begin(), map.end(),
                        std::back_inserter( ret ),
                        select2nd<typename MapT::value_type>() );
        return ret;
    }

    template<typename MapT>
    std::vector<typename MapT::key_type> keys( const MapT& map )
    {
        std::vector<typename MapT::key_type> ret;
        ret.reserve( map.size() );
        std::transform( map.begin(), map.end(),
                        std::back_inserter( ret ),
                        select1st<typename MapT::value_type>() );
        return ret;
    }
}

#endif
