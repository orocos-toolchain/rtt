/***************************************************************************
  tag: The SourceWorks  Tue Sep 7 00:55:18 CEST 2010  CreateSequence.hpp

                        CreateSequence.hpp -  description
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


#ifndef ORO_CREATESEQUENCE_HPP_
#define ORO_CREATESEQUENCE_HPP_

#include <boost/fusion/include/cons.hpp>
#include <boost/fusion/include/front.hpp>
#include <boost/fusion/include/vector.hpp>

#include <vector>
#include <boost/mpl/front.hpp>
#include <boost/mpl/pop_front.hpp>
#include <boost/mpl/print.hpp>
// The fusion <--> MPL link header
#include <boost/fusion/mpl.hpp>
#include <boost/utility/enable_if.hpp>

#include "BindStorage.hpp"
#include "DataSource.hpp"
#include "Exceptions.hpp"
#include "../FactoryExceptions.hpp"
#include "mystd.hpp"

#include <iostream>

namespace RTT
{
    namespace internal
    {
        namespace bf = boost::fusion;
        namespace mpl = boost::mpl;

        /**
         * Helper class for extracting the bare pointer from a shared_ptr
         * data source. Used in create_sequence::data() to unwrap the shared_ptr;
         */
        template<class Seq, class Data, class Enable = void >
        struct GetArgument {
            Data operator()(Seq s) { bf::front(s)->evaluate(); return Data(bf::front(s)->rvalue()); /* front(s) is a DataSource<Data> */}
        }; // normal type

        /**
         * In this case, Data is a pure reference and the first element of Seq is an AssignableDataSource.
         */
        template<class Seq, class Data>
        struct GetArgument<Seq, Data, typename boost::enable_if< is_pure_reference<Data> >::type> {
            Data operator()(Seq s) { return Data(bf::front(s)->set() ); /* Case of reference.*/ }
        }; // shared_ptr type

        /**
         * Helper class for avoiding assigning a bare pointer to a shared_ptr
         * data source. Used in create_sequence::set() to ignore the shared_ptr;
         */
        template<class Seq, class Data, class Enable = void >
        struct AssignHelper {
            static void set( Seq seq, Data in) { bf::front(seq)->set( bf::front(in) ); }
        }; // normal type

        template<class Seq, class Data>
        struct AssignHelper<Seq, Data, typename boost::enable_if< boost::is_pointer<typename mpl::front<Data>::type> >::type> {
            static void set(Seq , Data ) {} // nop
        }; // shared_ptr type

        /**
         * Helper to only update data sources that hold references.
         */
        template<class T>
        struct UpdateHelper {
            static void update(typename DataSource<typename remove_cr<T>::type >::shared_ptr) {}
        };

        template<class T>
        struct UpdateHelper<T&> {
            static void update(typename DataSource<typename remove_cr<T>::type >::shared_ptr s) { s->updated(); }
        };

        /**
         * Helper to convert a single data source base to a DataSource or AssignableDataSource.
         * Used by create_sequence_impl.
         */
        struct create_sequence_helper {
            template<class ds_arg_type, class ds_type>
            static ds_type sources(std::vector<base::DataSourceBase::shared_ptr>::const_iterator front, int argnbr, std::string const& tname )
            {
                typedef typename ds_type::element_type element_type;

                ds_type a =
                    boost::dynamic_pointer_cast< element_type >( *front );
                if ( ! a ) {
                    a = boost::dynamic_pointer_cast< element_type >( DataSourceTypeInfo<ds_arg_type>::getTypeInfo()->convert(*front) );
                }
                if ( ! a ) {
                    //cout << typeid(DataSource<ds_arg_type>).name() << endl;
                    ORO_THROW_OR_RETURN(wrong_types_of_args_exception( argnbr, tname, (*front)->getType() ), ds_type());
                    //ORO_THROW_OR_RETURN(wrong_types_of_args_exception( argnbr, typeid(DataSource<ds_arg_type>).name(), typeid(front).name() ), type());
                }
                return a;
            }

            template<class ds_arg_type, class ads_type>
            static ads_type assignable(std::vector<base::DataSourceBase::shared_ptr>::const_iterator front, int argnbr, std::string const& tname )
            {
                typedef typename ads_type::element_type element_type;

                ads_type a =
                    boost::dynamic_pointer_cast< element_type >( *front ); // note: no conversion done, must be same type.
                if ( ! a ) {
                    ORO_THROW_OR_RETURN(wrong_types_of_args_exception( argnbr, tname, (*front)->getType() ), ads_type());
                }
                return a;
            }
        };

        template<class List, int size>
        struct create_sequence_impl;

        /**
         * This class can create three kinds of Boost Fusion
         * Sequences.
         *
         * sources() creates a fusion sequence of (Assignable)DataSource<T>::shared_ptr from an mpl sequence
         * and a std::vector<DataSourceBase*>. Both must have same length.
         * The mpl sequence is typically obtained from the
         * function_types parameter_types traits class.
         *
         * assignable() creates a fusion sequence of AssignableDataSource<T>::shared_ptr from an mpl sequence
         * and a std::vector<DataSourceBase*>. Both must have same length.
         * The mpl sequence is typically obtained from the
         * function_types parameter_types traits class. If the mpl sequence elements contain const
         * qualifiers, these are removed, so T is always a value type.
         *
         * data() creates a fusion sequence of T,U,V,... from the fusion sequence
         * returned by operator(). It contains the values of the data sources
         * obtained by calling get().
         *
         * copy() creates a fusion sequence of DataSource<T>::shared_ptr from an mpl sequence
         * and a sequence returned by operator() to do the scripting copy/clone semantics on each
         * element of the sequence.
         *
         * This is a typical head-tail recursive implementation
         * where operator() calls itself, but in another type
         * specialisation.
         *
         */
        template<class List>
        struct create_sequence: public create_sequence_impl<List, mpl::size<
                List>::value>
        {
        };

        template<class List, int size>
        struct create_sequence_impl
        {
            /**
             * The tail is ourselves minus the head.
             */
            typedef create_sequence<typename mpl::pop_front<List>::type> tail;

            /**
             * The argument type
             */
            typedef typename mpl::front<List>::type arg_type;

            /**
             * The argument storage type
             */
            typedef AStore<arg_type> arg_store_type;

            /**
             * The data source value type of an assignable data source is non-const, non-reference.
             */
            typedef typename remove_cr<arg_type>::type ds_arg_type;

            /**
             * The type of a single element of the vector.
             */
            typedef typename mpl::if_<typename is_pure_reference<arg_type>::type,
                    typename AssignableDataSource< ds_arg_type >::shared_ptr,
                    typename DataSource<ds_arg_type>::shared_ptr>::type ds_type;

            typedef typename AssignableDataSource< ds_arg_type >::shared_ptr ads_type;

            /**
             * The type of the tail (List - head) of our sequence. It is recursively formulated
             * in terms of create_sequence.
             */
            typedef typename tail::type tail_type;

            /**
             * The joint DataSource<T>::shared_ptr type of head and tail, again a fusion cons.
             */
            typedef bf::cons<ds_type, tail_type> type;

            typedef typename tail::atype atail_type;
            typedef bf::cons<ads_type, atail_type> atype;

            typedef typename tail::data_type arg_tail_type;
            typedef typename tail::data_store_type arg_store_tail_type;

            /**
             * The joint T data type of head and tail.
             */
            typedef bf::cons<arg_type, arg_tail_type> data_type;

            /**
             * The joint T data storage type of head and tail.
             */
            typedef bf::cons<arg_store_type, arg_store_tail_type> data_store_type;

            /**
             * Converts a std::vector of DataSourceBase types into a boost::fusion Sequence of DataSources
             * of the types given in List. Will throw if an element of the vector could not
             * be dynamic casted to the respective element type of List.
             * @param args A vector of data sources
             * @param argnbr Leave as default. Used internally to count recursive calls.
             * @return a Fusion Sequence of DataSource<T>::shared_ptr objects
             */
            static type sources(std::vector<base::DataSourceBase::shared_ptr>::const_iterator args, int argnbr = 1 )
            {
                std::vector<base::DataSourceBase::shared_ptr>::const_iterator next = args;
                return bf::cons<ds_type, tail_type>
                    (create_sequence_helper::sources<ds_arg_type, ds_type>(args, argnbr, DataSourceTypeInfo<arg_type>::getType()),
                     tail::sources( ++next, argnbr + 1));
            }

            /**
             * Converts a std::vector of DataSourceBase types into a boost::fusion Sequence of AssignableDataSources
             * of the types given in List. Will throw if an element of the vector could not
             * be dynamic casted to the respective element type of List.
             * @param args A vector of data sources
             * @param argnbr Leave as default. Used internally to count recursive calls.
             * @return a Fusion Sequence of DataSource<T>::shared_ptr objects
             */
            static atype assignable(std::vector<base::DataSourceBase::shared_ptr>::const_iterator args, int argnbr = 1 )
            {
                std::vector<base::DataSourceBase::shared_ptr>::const_iterator next = args;
                return atype(
                        create_sequence_helper::assignable<ds_arg_type, ads_type>(args, argnbr, DataSourceTypeInfo<arg_type>::getType()),
                        tail::assignable(++next, argnbr + 1));
            }

            /**
             * Returns the data contained in the data sources
             * as a Fusion Sequence.
             * @param seq A Fusion Sequence of DataSource<T> types.
             * @return A sequence of type T holding the values of the DataSource<T>.
             */
            static data_type data(const type& seq) {
                return data_type( GetArgument<type,arg_type>()(seq), tail::data( bf::pop_front(seq) ) );
            }

            /**
             * Sets the values of a sequence of AssignableDataSource<T>
             * data sources ot the values contained in \a in using set().
             * @param in The values to write.
             * @param seq The receiving assignable data sources.
             */
            static void set(const data_type& in, const atype& seq) {
                AssignHelper<atype, data_type>::set(seq, in);
                tail::set( bf::pop_front(in), bf::pop_front(seq) );
            }

            /**
             * Sets the values of a sequence of AssignableDataSource<T>
             * data sources ot the values contained in \a in using set().
             * @param in The values to write.
             * @param seq The receiving assignable data sources. Because
             * the datasources are shared pointers, it's ok to work on the
             * temporary copies of seq.
             */
            static void load(const data_store_type& in, const atype& seq) {
                AssignHelper<atype, data_store_type>::set(seq, in);
                tail::load( bf::pop_front(in), bf::pop_front(seq) );
            }

            /**
             * Stores the values of a sequence of data_type into
             * a data_store_type sequence for later retrieval during load.
             * We must return the resulting sequence by value, since boost fusion
             * returns temporaries, which we can't take a reference to.
             * @param in The values to store
             * @return The receiving AStore sequence.
             */
            static data_store_type store(const data_type& in ) {
                return data_store_type(bf::front(in), tail::store(bf::pop_front(in)));
            }

            /**
             * Calls the DataSourceBase::updated() function for each element
             * in sequence \a seq, in case the element is a non-const reference type.
             * @param seq A sequence of DataSource<T> objects
             */
            static void update(const type&seq) {
                UpdateHelper<arg_type>::update( bf::front(seq) );
                tail::update( bf::pop_front(seq) );
            }

            /**
             * Copies a sequence of DataSource<T>::shared_ptr according to the
             * copy/clone semantics of data sources.
             * @param seq A Fusion Sequence of DataSource<T>::shared_ptr
             * @param alreadyCloned the copy/clone map
             * @return A Fusion Sequence of DataSource<T>::shared_ptr containing the copies.
             */
            static type copy(const type& seq, std::map<
                              const base::DataSourceBase*,
                              base::DataSourceBase*>& alreadyCloned) {
                return type( bf::front(seq)->copy(alreadyCloned), tail::copy( bf::pop_front(seq), alreadyCloned ) );
            }

            /**
             * Returns the i'th argument type info as returned by
             * DataSource<ArgI>::GetTypeInfo(), starting from 1.
             * @param i A number between 1..N with N being the
             * number of types in the mpl List of this class.
             * @return An Orocos registered type info object.
             */
            static const types::TypeInfo* GetTypeInfo(int i) {
                if ( i <= 0 || i > size)
                    return 0;
                if ( i == 1 ) {
                    return DataSourceTypeInfo<arg_type>::getTypeInfo();
                } else {
                    return tail::GetTypeInfo(i-1);
                }
            }

            /**
             * Returns the i'th argument type name as returned by
             * DataSource<ArgI>::GetType(), starting from 1.
             * @param i A number between 1..N with N being the
             * number of types in the mpl List of this class.
             * @return An full qualified type name.
             */
            static std::string GetType(int i) {
                if ( i <= 0 || i > size)
                    return "na";
                if ( i == 1 ) {
                    return DataSourceTypeInfo<arg_type>::getType();
                } else {
                    return tail::GetType(i-1);
                }
            }
};

        template<class List>
        struct create_sequence_impl<List, 1> // mpl list of one
        {
            typedef typename mpl::front<List>::type arg_type;
            typedef typename remove_cr<arg_type>::type ds_arg_type;
            typedef bf::cons<arg_type> data_type;

            typedef AStore<arg_type> arg_store_type;
            typedef bf::cons<arg_store_type > data_store_type;

            /**
             * The type of a single element of the vector.
             */
            typedef typename mpl::if_<typename is_pure_reference<arg_type>::type,
                    typename AssignableDataSource< ds_arg_type >::shared_ptr,
                    typename DataSource<ds_arg_type>::shared_ptr>::type ds_type;
            typedef typename AssignableDataSource< ds_arg_type >::shared_ptr ads_type;


            // the result sequence type is a cons of the last argument in the vector.
            typedef bf::cons<ds_type> type;

            typedef bf::cons<ads_type> atype;

            static type sources(std::vector<base::DataSourceBase::shared_ptr>::const_iterator front, int argnbr = 1)
            {
                return type(
                        create_sequence_helper::sources<ds_arg_type, ds_type>(front, argnbr, DataSourceTypeInfo<arg_type>::getType()));
            }

            static atype assignable(std::vector<base::DataSourceBase::shared_ptr>::const_iterator args, int argnbr = 1)
            {
                return atype(
                        create_sequence_helper::assignable<ds_arg_type, ads_type>(args, argnbr, DataSourceTypeInfo<arg_type>::getType()));
            }

            /**
             * Returns the data contained in the data source
             * as a Fusion Sequence.
             * @param seq A Fusion Sequence of DataSource<T> types.
             * @return A sequence of type T holding the values of the DataSource<T>.
             */
            static data_type data(const type& seq) {
                return data_type( GetArgument<type,arg_type>()(seq) );
            }

            static void update(const type&seq) {
                UpdateHelper<arg_type>::update( bf::front(seq) );
            }

            static void set(const data_type& in, const atype& seq) {
                AssignHelper<atype, data_type>::set(seq, in);
            }

            static void load(const data_store_type& in, const atype& seq) {
                AssignHelper<atype, data_store_type>::set(seq, in);
            }

            static data_store_type store(const data_type& in ) {
                return data_store_type( bf::front(in) );
            }

            /**
             * Copies a sequence of DataSource<T>::shared_ptr according to the
             * copy/clone semantics of data sources.
             * @param seq A Fusion Sequence of DataSource<T>::shared_ptr
             * @param alreadyCloned the copy/clone map
             * @return A Fusion Sequence of DataSource<T>::shared_ptr containing the copies.
             */
            static type copy(const type& seq, std::map<
                              const base::DataSourceBase*,
                              base::DataSourceBase*>& alreadyCloned) {
                return type( bf::front(seq)->copy(alreadyCloned) );
            }

            static const types::TypeInfo* GetTypeInfo(int i) {
                if ( i != 1)
                    return 0;
                return DataSource<ds_arg_type>::GetTypeInfo();
            }
            static std::string GetType(int i) {
                if ( i != 1)
                    return "na";
                return DataSourceTypeInfo<arg_type>::getType();
            }
        };

        template<class List>
        struct create_sequence_impl<List, 0> // empty mpl list
        {
            typedef bf::vector<> data_type;
            typedef bf::vector<> data_store_type;

            // the result sequence type is a cons of the last argument in the vector.
            typedef bf::vector<> type;

            typedef bf::vector<> atype;

            static type sources(std::vector<base::DataSourceBase::shared_ptr>::const_iterator args, int argnbr = 0)
            {
                return type();
            }

            static atype assignable(std::vector<base::DataSourceBase::shared_ptr>::const_iterator args, int argnbr = 0)
            {
                return atype();
            }

            /**
             * Returns the data contained in the data source
             * as a Fusion Sequence.
             * @param seq A Fusion Sequence of DataSource<T> types.
             * @return A sequence of type T holding the values of the DataSource<T>.
             */
            static data_type data(const type& seq) {
                return data_type();
            }

            static void update(const type&seq) {
                return;
            }

            static void set(const data_type& in, const atype& seq) {
                return;
            }

            static void load(const data_store_type& in, const atype& seq) {
                return;
            }

            static data_store_type store(const data_type& in ) {
                return data_store_type();
            }

            /**
             * Copies a sequence of DataSource<T>::shared_ptr according to the
             * copy/clone semantics of data sources.
             * @param seq A Fusion Sequence of DataSource<T>::shared_ptr
             * @param alreadyCloned the copy/clone map
             * @return A Fusion Sequence of DataSource<T>::shared_ptr containing the copies.
             */
            static type copy(const type& seq, std::map<
                              const base::DataSourceBase*,
                              base::DataSourceBase*>& alreadyCloned) {
                return type();
            }
            static const types::TypeInfo* GetTypeInfo(int i) {
                return 0;
            }
            static std::string GetType(int i) {
                return "na";
            }
        };
    }
}

#endif /* ORO_CREATESEQUENCE_HPP_ */
