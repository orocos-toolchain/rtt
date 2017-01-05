/***************************************************************************
  tag: The SourceWorks  Tue Sep 7 00:55:18 CEST 2010  type_discovery.hpp

                        type_discovery.hpp -  description
                           -------------------
    begin                : Tue September 07 2010
    copyright            : (C) 2010 The SourceWorks
    email                : peter@thesourceworks.com

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


#ifndef TYPE_DISCOVERY_HPP_
#define TYPE_DISCOVERY_HPP_

// (C) Copyright 2002 Robert Ramey - http://www.rrsd.com .
// (C) Copyright 2009 Peter Soetens - http://www.thesourceworks.com .
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org for updates, documentation, and revision history.


/**
 * @file type_discovery.hpp
 *
 * This file implements a 'level 2' type archiver of serializable objects
 * and uses the standard BOOST serialization API for reading structures.
 *
 * It is used to decompose 'Plain Old Data' types into data sources, which
 * are references to parts of the parent data.
 *
 * This archive is header-only and does not depend on the serialization DLL.
 *
 */

#include <cassert>
#include <boost/version.hpp>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/is_bitwise_serializable.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/array.hpp>
#include <boost/archive/detail/iserializer.hpp>
#include <boost/archive/detail/oserializer.hpp>
#include <boost/archive/archive_exception.hpp>
#include <boost/config.hpp>
#include <boost/mpl/bool.hpp>
#include <boost/array.hpp>

#include <vector>
#include <string>
#include "../base/DataSourceBase.hpp"
#include "../internal/PartDataSource.hpp"
#include "../internal/DataSources.hpp"
#include "../internal/Reference.hpp"
#include "carray.hpp"

namespace RTT
{
    namespace types
    {
        /**
         * This archive is capable of decomposing objects of serialization level 1 and 2
         * into part data sources.
         */
        class type_discovery
        {
        public:
            /**
             *  The parent struct we're deserializing
             */
            base::DataSourceBase::shared_ptr mparent;

            typedef std::vector<base::DataSourceBase::shared_ptr> Parts;
            typedef std::vector<std::string> PartNames;
            /**
             * The parts of the parent struct
             */
            Parts mparts;

            /**
             * The names of the parts of the parent struct
             */
            PartNames mnames;

            /**
             * If non-empty, only discover the member with this name.
             */
            std::string membername;

            /**
             * If non-empty, use this reference to the member with name
             * membername.
             */
            internal::Reference* mref;

            typedef char Elem;
            /**
             * Saving Archive Concept::is_loading
             */
            typedef boost::mpl::bool_<true> is_loading;
            /**
             * Saving Archive Concept::is_saving
             */
            typedef boost::mpl::bool_<false> is_saving;

            /**
             * Constructor which inspects part names and creates
             * part data sources.
             */
            type_discovery(base::DataSourceBase::shared_ptr parent) :
                mparent(parent), mref(0)
            {
            }

            /**
             * Constructor which only introspects the part names.
             * No parts will be created.
             */
            type_discovery() :
                mparent(), mref(0)
            {
            }

            base::DataSourceBase::shared_ptr getMember(const std::string name) {
                PartNames::iterator it = find( mnames.begin(), mnames.end(), name);
                if ( it != mnames.end() && mparts.size() == mnames.size() )
                    return mparts.at( it - mnames.begin() );
                return base::DataSourceBase::shared_ptr();
            }

            /**
             * This function discovers all parts of a serializable struct and
             * creates a DataSource object for each member of that struct.
             * Use getMember() afterwards to retrieve the DataSource of each
             * member.
             */
            template<class T>
            void discover( T& t) {
#if BOOST_VERSION >= 104100
                boost::archive::detail::load_non_pointer_type<type_discovery>::load_only::invoke(*this,t);
#else
                boost::archive::detail::load_non_pointer_type<type_discovery,T>::load_only::invoke(*this,t);
#endif
            }

            /**
             * This function discovers a single part of a serializable struct
             * and returns an assignable datasource to that member of that struct, or a null
             * ptr if no such member exists or if the member is not assignable.
             */
            template<class T>
            base::DataSourceBase::shared_ptr discoverMember( T& t, const std::string name) {
                membername = name;
                discover( t );
                if ( ! mparts.empty() )
                    return mparts[0];
                return base::DataSourceBase::shared_ptr();
            }

            /**
             * This function discovers a single part of a serializable struct
             * and sets a reference to that member of that struct.
             */
            template<class T>
            bool referenceMember(internal::Reference* ref, T& t, const std::string name) {
                assert(ref);
                membername = name;
                mref = ref;
                discover( t );
                if (mref == 0) // we found it
                    return true;
                return false;
            }

            /**
             * Loading Archive Concept::get_library_version()
             * @return This library's version.
             */
            unsigned int get_library_version() { return 0; }

            /**
             * Loading Archive Concept::reset_object_address(v,u)
             * @param new_address
             * @param old_address
             */
            void reset_object_address(const void * new_address, const void * old_address) {}

            /**
             * Loading Archive Concept::delete_created_pointers()
             */
            void delete_created_pointers() {}

            /**
             * Loading Archive Concept::register_type<T>() and ::register_type(u)
             * @param The data type to register in this archive.
             * @return
             */
            template<class T>
            const boost::archive::detail::basic_pointer_iserializer *
            register_type(T * = NULL) {return 0;}

            /**
             * Note: not in LoadArchive concept but required when we use archive::load !
             * @param x
             * @param bos
             */
            void load_object(void *x, const boost::archive::detail::basic_oserializer & bos)
            {
                assert(false);
            }

            /**
             * Saving Archive Concept::operator<<
             * @param t The type to load.
             * @return *this
             */
            template<class T>
            type_discovery &operator>>(T &t)
            {
                return load_a_type(t, boost::mpl::bool_<boost::serialization::implementation_level<T>::value == boost::serialization::primitive_type>());
            }

            /**
             * Saving Archive Concept::operator&
             * @param t The type to load.
             * @return *this
             */
            template<class T>
            type_discovery &operator&(T &t)
            {
                return this->operator>>(t);
            }

            /**
             * Specialisation for writing out primitive types.
             * @param t primitive data (bool, int,...)
             * @return *this
             */
            template<class T>
            type_discovery &load_a_type(T &t, boost::mpl::true_)
            {
                // stores the part
                if (mparent) {
                    mparts.push_back(new internal::PartDataSource<T> (t, mparent));
                }
                return *this;
            }

            /**
             * Specialisation for writing out composite types (objects).
             * @param t a serializable class or struct.
             * @return *this
             */
            template<class T>
            type_discovery &load_a_type(T &t, boost::mpl::false_)
            {
                mparts.push_back(new internal::PartDataSource<T> (t, mparent));
                return *this;
            }

            /**
             * Specialisation that converts a boost serialization array into a RTT types carray.
             * @param t
             * @return *this
             */
            template<class T>
#if BOOST_VERSION >= 106100
            type_discovery &load_a_type(const boost::serialization::array_wrapper<T> &t, boost::mpl::false_)
#else
            type_discovery &load_a_type(const boost::serialization::array<T> &t, boost::mpl::false_)
#endif
            {
                mparts.push_back(new internal::PartDataSource< carray<T> > ( carray<T>(t), mparent) );
                return *this;
            }

            /**
             * Specialisation that converts a boost::array into a RTT types carray.
             * @param t
             * @return *this
             */
            template<class T, std::size_t N>
            type_discovery &load_a_type(boost::array<T,N> &t, boost::mpl::false_)
            {
                mparts.push_back(new internal::PartDataSource< carray<T> > ( carray<T>(t), mparent) );
                return *this;
            }

            /**
             * We do not support pointer types.
             * @param t
             * @param
             * @return
             */
            template<class T>
            type_discovery &load_a_type(const T* &, boost::mpl::false_)
            {
                //pointers can not be serialized.
                //BOOST_STATIC_ASSERT( boost::mpl::false_ );
                return *this;
            }

            //! special treatment for name-value pairs.
            //! Since all types pass here, we take some decisions early
            template<class T>
            type_discovery &load_a_type(const boost::serialization::nvp<T> & t, boost::mpl::false_)
            {
                // check for single-member extraction first:
                if ( !membername.empty() ) {
                    // Only serialize if the name matches:
                    if ( t.name() == membername ) {
                        if ( !mref ) {
                            *this & t.value();
                        } else {
                            mref->setReference( (void*) & t.value() );
                            mref = 0; // signals' we're done.
                        }
                    }
                } else {
                    // Full extraction of all parts:
                    // store name of member
                    mnames.push_back( t.name() );

                    // serialize the data as usual
                    if (mparent)
                        *this & t.value();
                }

                return *this;
            }
        };
    }
}

#endif /* TYPE_DISCOVERY_HPP_ */
