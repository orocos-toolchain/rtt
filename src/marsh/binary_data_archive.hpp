/***************************************************************************
  tag: Peter Soetens  Thu Oct 22 11:59:08 CEST 2009  binary_data_archive.hpp

                        binary_data_archive.hpp -  description
                           -------------------
    begin                : Thu October 22 2009
    copyright            : (C) 2009 Peter Soetens
    email                : peter@thesourcworks.com

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




#ifndef BINARY_DATA_ARCHIVE_HPP_
#define BINARY_DATA_ARCHIVE_HPP_

// (C) Copyright 2002 Robert Ramey - http://www.rrsd.com .
// (C) Copyright 2009 Peter Soetens - http://www.thesourceworks.com .
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org for updates, documentation, and revision history.


/**
 * @file binary_data_archive.hpp
 *
 * This file implements a 'level 2' binary archiver of serializable objects.
 *
 * It can be used if only 'Plain Old Data' must be stored and has the advantage
 * that it doesn't allocate memory, nor during construction of the archive, nor
 * during serializing/deserializing.
 *
 * No class information or cross-references are stored.
 *
 * This archive is header-only and does not depend on the serialization DLL.
 *
 */

#include <cassert>
#include <istream>
#include <ostream>
#include <streambuf>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/is_bitwise_serializable.hpp>
#include <boost/archive/detail/iserializer.hpp>
#include <boost/archive/detail/oserializer.hpp>
#include <boost/archive/archive_exception.hpp>
#include <boost/config.hpp>
#include <boost/mpl/bool.hpp>

namespace RTT
{
    namespace marsh
    {

        /**
         * This archive is capable of loading objects of
         * serialization level 1 and 2 from a binary, non-portable format.
         * @see binary_data_oarchive
         * @see
         */
        class binary_data_iarchive
        {
            std::streambuf& m_sb;
            int data_read;
        public:
            typedef char Elem;
            typedef binary_data_iarchive Archive;

            /**
             * Loading Archive Concept::is_loading
             */
            typedef boost::mpl::bool_<true> is_loading;
            /**
             * Loading Archive Concept::is_saving
             */
            typedef boost::mpl::bool_<false> is_saving;

            /**
             * Constructor from a standard output stream.
             * @param os The stream to serialize from.
             */
            binary_data_iarchive(std::streambuf& bsb) :
                m_sb(bsb), data_read(0)
            {
            }

            /**
             * Constructor from a standard stream buffer.
             * @param os The buffer to serialize from.
             */
            binary_data_iarchive(std::istream& is) :
                m_sb(*is.rdbuf()), data_read(0)
            {
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
             * This function is only used when we call archive::load( *this, t);
             * @param x
             * @param bos
             */
            void load_object(
                void *t,
                const /* BOOST_ARCHIVE_DECL(BOOST_PP_EMPTY()) */ boost::archive::detail::basic_iserializer & bis
            ) {
                assert(false);
            }

            /**
             * The standard type loading function. It forwards any type T
             * to the correct internal load_a_type function.
             */
            template<class T>
            void load_override(T & t, BOOST_PFTO int){
                load_a_type(t, boost::mpl::bool_<boost::serialization::implementation_level<T>::value == boost::serialization::primitive_type>() );
                //archive::load(* this->This(), t);
            }

            /**
             * These load_override functions are required to handle the nvt<T> cases in the
             * serialization code. GCC won't compile that code without these overloads.
             * Others may be required as well and may need to be added later on.
             */
#if 0
            void load_override(const boost::serialization::nvp<boost::serialization::collection_size_type> & t, int){
                 size_t x=0;
                 * this >> x;
                 t.value() = boost::serialization::collection_size_type(x);
             }
#endif
            template<class T>
            void load_override(const boost::serialization::nvp<T> & t, int){
                 T x;
                 * this >> x;
                 t.value() = x;
             }

            /**
             * Loading Archive Concept::operator>>
             * @param t The type to load.
             * @return *this
             */
            template<class T>
            binary_data_iarchive &operator>>(T &t){
                this->load_override(t, 0);
                return * this;
            }

            /**
             * Loading Archive Concept::operator&
             * @param t The type to load.
             * @return *this
             */
            template<class T>
            binary_data_iarchive &operator&(T &t){
                return this->operator>>(t);
            }

            /**
             * Loading Archive Concept::load_binary(u, count)
             * @param address The place in memory where data must be written.
             * @param count The number of bytes to load.
             */
            void load_binary(void *address, std::size_t count)
            {
                // note: an optimizer should eliminate the following for char files
                std::streamsize s = count / sizeof(Elem);
                std::streamsize scount = m_sb.sgetn(
                        static_cast<Elem *> (address), s);
                if (scount != static_cast<std::streamsize> (s))
                    boost::serialization::throw_exception(
                            boost::archive::archive_exception(
                                    boost::archive::archive_exception::stream_error));
                // note: an optimizer should eliminate the following for char files
                s = count % sizeof(Elem);
                if (0 < s)
                {
                    //        if(is.fail())
                    //            boost::serialization::throw_exception(
                    //                archive_exception(archive_exception::stream_error)
                    //        );
                    Elem t;
                    scount = m_sb.sgetn(&t, 1);
                    if (scount != 1)
                        boost::serialization::throw_exception(
                                boost::archive::archive_exception(
                                        boost::archive::archive_exception::stream_error));
                    std::memcpy(static_cast<char*> (address) + (count - s), &t,
                            s);
                }
                data_read += count;
            }

            /**
             * Specialisation for writing out primitive types.
             * @param t primitive data (bool, int,...)
             * @return *this
             */
            template<class T>
            binary_data_iarchive &load_a_type(T &t,boost::mpl::true_){
                  load_binary(&t, sizeof(T));
                  return *this;
            }

            /**
             * Specialisation for writing out composite types (objects).
             * @param t a serializable class or struct.
             * @return *this
             */
            template<class T>
            binary_data_iarchive &load_a_type(T &t,boost::mpl::false_){
                boost::archive::detail::load_non_pointer_type<binary_data_iarchive,T>::load_only::invoke(*this,t);
                return *this;
            }

            /**
             * We provide an optimized load for all fundamental types
             * typedef serialization::is_bitwise_serializable<mpl::_1> use_array_optimization;
             */
            struct use_array_optimization {
                template <class T>
                #if defined(BOOST_NO_DEPENDENT_NESTED_DERIVATIONS)
                    struct apply {
                        typedef BOOST_DEDUCED_TYPENAME boost::serialization::is_bitwise_serializable<T>::type type;
                    };
                #else
                    struct apply : public boost::serialization::is_bitwise_serializable<T> {};
                #endif
            };

            /**
             * The optimized save_array dispatches to save_binary
             */
            template<class ValueType>
            void load_array(boost::serialization::array<ValueType>& a,
                            unsigned int)
            {
                load_binary(a.address(), a.count()
                        * sizeof(ValueType));
            }

            /**
             * Helper method to say how much we read.
             */
            int getArchiveSize() { return data_read; }
        };

        /**
         * This archive is capable of saving objects of serialization level 1 and 2
         * in a binary, non-portable format.
         * @see binary_data_iarchive  
         */
        class binary_data_oarchive
        {
            std::streambuf & m_sb;
            int data_written;
            bool mdo_save;
        public:
            typedef char Elem;
            /**
             * Saving Archive Concept::is_loading
             */
            typedef boost::mpl::bool_<false> is_loading;
            /**
             * Saving Archive Concept::is_saving
             */
            typedef boost::mpl::bool_<true> is_saving;

            /**
             * Constructor from a standard output stream.
             * @param os The stream to serialize to
             * @param do_save Set to false to not actually write nor use
             * the given ostream. After a save operation, only the counter
             * for getArchiveSize() will have increased. Use this to know
             * in advance how much space you will need.
             */
            binary_data_oarchive(std::ostream& os,bool do_save = true) :
                m_sb(*os.rdbuf()), data_written(0), mdo_save(do_save)
            {
            }

            /**
             * Constructor from a standard stream buffer.
             * @param os The buffer to serialize to.
             * @param do_save Set to false to not actually write nor use
             * the given ostream. After a save operation, only the counter
             * for getArchiveSize() will have increased. Use this to know
             * in advance how much space you will need.
             */
            binary_data_oarchive(std::streambuf& sb,bool do_save = true) :
                m_sb(sb), data_written(0), mdo_save(do_save)
            {
            }

            /**
             * Saving Archive Concept::get_library_version()
             * @return This library's version.
             */
            unsigned int get_library_version() { return 0; }

            /**
             * Saving Archive Concept::register_type<T>() and ::register_type(u)
             * @param The data type to register in this archive.
             * @return
             */
            template<class T>
            const boost::archive::detail::basic_pointer_iserializer *
            register_type(T * = NULL) {return 0;}

            /**
             * Note: not in LoadArchive concept but required when we use archive::save !
             * @param x
             * @param bos
             */
            void save_object(
                const void *x,
                const boost::archive::detail::basic_oserializer & bos
            ) {
                assert(false);
                //(bos.save_object_data)(*this, x);
            }

            /**
             * Saving Archive Concept::operator<<
             * @param t The type to save.
             * @return *this
             */
            template<class T>
            binary_data_oarchive &operator<<(T const &t){
                    return save_a_type(t,boost::mpl::bool_< boost::serialization::implementation_level<T>::value == boost::serialization::primitive_type>() );
            }

            /**
             * Saving Archive Concept::operator&
             * @param t The type to save.
             * @return *this
             */
            template<class T>
            binary_data_oarchive &operator&(T const &t){
                    return this->operator<<(t);
            }

            /**
             * Saving Archive Concept::save_binary(u, count)
             * @param address The place where data is located in memory.
             * @param count The number of bytes to save.
             */
            inline void save_binary(const void *address, std::size_t count)
            {
                // figure number of elements to output - round up
                count = (count + sizeof(Elem) - 1) / sizeof(Elem);
                if (mdo_save) {
                    std::streamsize scount = m_sb.sputn(
                            static_cast<const Elem *> (address), count);
                    if (count != static_cast<std::size_t> (scount))
                        boost::serialization::throw_exception(
                                boost::archive::archive_exception(
                                        boost::archive::archive_exception::stream_error));
                }
                data_written += count;
            }

            /**
             * Specialisation for writing out primitive types.
             * @param t primitive data (bool, int,...)
             * @return *this
             */
            template<class T>
            binary_data_oarchive &save_a_type(T const &t,boost::mpl::true_){
                  save_binary(&t, sizeof(T));
                  return *this;
            }

            /**
             * Specialisation for writing out composite types (objects).
             * @param t a serializable class or struct.
             * @return *this
             */
            template<class T>
            binary_data_oarchive &save_a_type(T const &t,boost::mpl::false_){
                  boost::archive::detail::save_non_pointer_type<binary_data_oarchive,T>::save_only::invoke(*this,t);
                  return *this;
            }

            /**
             * We provide an optimized load for all fundamental types
             * typedef serialization::is_bitwise_serializable<mpl::_1> use_array_optimization;
             */
            struct use_array_optimization {
                template <class T>
                #if defined(BOOST_NO_DEPENDENT_NESTED_DERIVATIONS)
                    struct apply {
                        typedef BOOST_DEDUCED_TYPENAME boost::serialization::is_bitwise_serializable<T>::type type;
                    };
                #else
                    struct apply : public boost::serialization::is_bitwise_serializable<T> {};
                #endif
            };

            /**
             * The optimized save_array dispatches to save_binary
             */ 
            template<class ValueType>
            void save_array(boost::serialization::array<ValueType> const& a,
                            unsigned int)
            {
                save_binary(a.address(), a.count()
                        * sizeof(ValueType));
            }

            /**
             * Helper method to say how much we wrote.
             */
            int getArchiveSize() { return data_written; }
        };
    }
}

#endif /* BINARY_DATA_ARCHIVE_HPP_ */
