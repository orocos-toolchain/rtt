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

#include "DataSource.hpp"
#include "Exceptions.hpp"
#include "../interface/FactoryExceptions.hpp"
#include "mystd.hpp"

#include <iostream>
using namespace std;

namespace RTT
{
    namespace internal
    {
        namespace bf = boost::fusion;
        namespace mpl = boost::mpl;


        template <class T> struct incomplete;

        /**
         * This function should have been in boost::type_traits itself (C++0x?).
         */
        template <typename T>
        struct is_shared_ptr
            : public mpl::false_
        {};
        template<typename T>
        struct is_shared_ptr<boost::shared_ptr<T> >
            : public mpl::true_
        {};

        /**
         * Helper class for extracting the bare pointer from a shared_ptr
         * data source. Used in create_sequence::data() to unwrap the shared_ptr;
         */
        template<class Seq, class Data, class Enable = void >
        struct GetPointerWrap {
            Data operator()(Seq s) { bf::front(s)->evaluate(); return Data(bf::front(s)->rvalue()); /* front(s) is a DataSource<Data> */}
        }; // normal type

        /**
         * In this case, Data is a pointer and the front element of Seq is a shared_ptr.
         */
        template<class Seq, class Data>
        struct GetPointerWrap<Seq, Data, typename boost::enable_if< boost::is_pointer<Data> >::type> {
        //struct GetPointerWrap<Seq, Data, typename boost::enable_if< is_shared_ptr<typename mpl::front<Seq>::type::element_type::result_t> >::type> {
            Data operator()(Seq s) { return Data(bf::front(s)->get().get()); /* first get is on DS, second get is on shared_ptr.*/ }
        }; // shared_ptr type

        /**
         * In this case, Data is a pure reference and the first element of Seq is an AssignableDataSource.
         */
        template<class Seq, class Data>
        struct GetPointerWrap<Seq, Data, typename boost::enable_if< is_pure_reference<Data> >::type> {
            Data operator()(Seq s) { return Data(bf::front(s)->set() ); /* Case of reference.*/ }
        }; // shared_ptr type

        /**
         * Helper class for avoiding assigning a bare pointer to a shared_ptr
         * data source. Used in create_sequence::set() to ignore the shared_ptr;
         */
        template<class Seq, class Data, class Enable = void >
        struct AssignHelper {
            static void set( Seq seq, Data in) { bf::front(seq)->set( bf::front(in) );
}
        }; // normal type

        template<class Seq, class Data>
        struct AssignHelper<Seq, Data, typename boost::enable_if< boost::is_pointer<typename mpl::front<Data>::type> >::type> {
            //        struct AssignHelper<Seq, Data, typename boost::enable_if< is_shared_ptr<typename mpl::front<Seq>::type::element_type::result_t> >::type> {
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
         * @note When the first list element is an object pointer T*, it will be wrapped in a
         * boost::shared_ptr<T> in order to support the operationDS semantics. The shared_ptr
         * will be unwrapped from T when calling data().
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

            typedef typename mpl::front<List>::type bare_type;

            /**
             * As documented, if the first element of List is a pointer, wrap it
             * in a boost::shared_ptr.
             */
            typedef typename mpl::if_<typename boost::is_pointer<bare_type>::type, boost::shared_ptr<typename boost::remove_pointer<bare_type>::type>, bare_type >::type arg_type;

            /**
             * The data source value type of an assignable data source is non-const, non-reference.
             */
            typedef typename remove_cr<arg_type>::type ds_arg_type;

            /**
             * The type of a single element of the vector.
             */
            typedef typename mpl::if_<typename is_pure_reference<bare_type>::type,
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

            /**
             * The joint T data type of head and tail.
             */
            typedef bf::cons<bare_type, arg_tail_type> data_type;

            /**
             * Converts a std::vector of DataSourceBase types into a boost::fusion Sequence of DataSources
             * of the types given in List. Will throw if an element of the vector could not
             * be dynamic casted to the respective element type of List.
             * @param args A vector of data sources
             * @param argnbr Leave as default. Used internally to count recursive calls.
             * @return a Fusion Sequence of DataSource<T>::shared_ptr objects
             */
            static type sources(std::vector<base::DataSourceBase::shared_ptr> args, int argnbr = 1 )
            {
                assert( args.size() == size);
                base::DataSourceBase::shared_ptr front = args.front();

                ds_type a =
                    boost::dynamic_pointer_cast< typename ds_type::element_type >( DataSourceTypeInfo<ds_arg_type>::getTypeInfo()->convert(front) );
                if ( ! a ) {
                    //cout << typeid(DataSource<arg_type>).name() << endl;
                    ORO_THROW_OR_RETURN(interface::wrong_types_of_args_exception( argnbr, DataSource<ds_arg_type>::GetType(), front->getType() ), type());
                    //ORO_THROW_OR_RETURN(wrong_types_of_args_exception( argnbr, typeid(DataSource<arg_type>).name(), typeid(front).name() ), type());
                }

                args.erase(args.begin());
                return bf::cons<ds_type, tail_type>(
                        ds_type(a),
                        tail::sources(args, ++argnbr));
            }

            /**
             * Converts a std::vector of DataSourceBase types into a boost::fusion Sequence of AssignableDataSources
             * of the types given in List. Will throw if an element of the vector could not
             * be dynamic casted to the respective element type of List.
             * @param args A vector of data sources
             * @param argnbr Leave as default. Used internally to count recursive calls.
             * @return a Fusion Sequence of DataSource<T>::shared_ptr objects
             */
            static atype assignable(std::vector<base::DataSourceBase::shared_ptr> args, int argnbr = 1 )
            {
                assert( args.size() == size);
                base::DataSourceBase::shared_ptr front = args.front();

                typename AssignableDataSource<ds_arg_type>::shared_ptr a =
                    boost::dynamic_pointer_cast< AssignableDataSource<ds_arg_type> >( front ); // note: no conversion done, must be same type.
                if ( ! a ) {
                    ORO_THROW_OR_RETURN(interface::wrong_types_of_args_exception( argnbr, DataSource<ds_arg_type>::GetType(), front->getType() ), type());
                }

                args.erase(args.begin());
                return atype(
                        ads_type(a),
                        tail::assignable(args, ++argnbr));
            }

            /**
             * Returns the data contained in the data sources
             * as a Fusion Sequence.
             * @param seq A Fusion Sequence of DataSource<T> types.
             * @return A sequence of type T holding the values of the DataSource<T>.
             */
            static data_type data(const type& seq) {
                // we want to return the bare_type, without any shared_ptr.
                return data_type( GetPointerWrap<type,bare_type>()(seq), tail::data( bf::pop_front(seq) ) );
            }

            /**
             * Sets the values of a sequence of AssignableDataSource<T>
             * data sources ot the values contained in \a in using set().
             * @param in The values to write.
             * @param seq The receiving assignable data sources.
             */
            static void set(const data_type& in, const atype& seq) {
                AssignHelper<atype, data_type>::set(seq, in);
                return tail::set( bf::pop_front(in), bf::pop_front(seq) );
            }

            /**
             * Calls the DataSourceBase::updated() function for each element
             * in sequence \a seq, in case the element is a non-const reference type.
             * @param seq A sequence of DataSource<T> objects
             */
            static void update(const type&seq) {
                UpdateHelper<arg_type>::update( bf::front(seq) );
                return tail::update( bf::pop_front(seq) );
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
                    return DataSourceTypeInfo<arg_type>::getType() + DataSourceTypeInfo<arg_type>::getQualifier();
                } else {
                    return tail::GetType(i-1);
                }
            }
};

        template<class List>
        struct create_sequence_impl<List, 1> // mpl list of one
        {
            typedef typename mpl::front<List>::type bare_type;
            typedef typename mpl::if_<typename boost::is_pointer<bare_type>::type, boost::shared_ptr<typename boost::remove_pointer<bare_type>::type>, bare_type >::type arg_type;
            typedef typename remove_cr<arg_type>::type ds_arg_type;
            typedef bf::cons<bare_type> data_type;

            /**
             * The type of a single element of the vector.
             */
            typedef typename mpl::if_<typename is_pure_reference<bare_type>::type,
                    typename AssignableDataSource< ds_arg_type >::shared_ptr,
                    typename DataSource<ds_arg_type>::shared_ptr>::type ds_type;
            typedef typename AssignableDataSource< ds_arg_type >::shared_ptr ads_type;


            // the result sequence type is a cons of the last argument in the vector.
            typedef bf::cons<ds_type> type;

            typedef bf::cons<ads_type> atype;

            static type sources(const std::vector<base::DataSourceBase::shared_ptr>& args, int argnbr = 1)
            {
                assert( args.size() == 1);
                base::DataSourceBase::shared_ptr front = args.front();

                ds_type a =
                    boost::dynamic_pointer_cast< typename ds_type::element_type >( DataSourceTypeInfo<ds_arg_type>::getTypeInfo()->convert(front) );
                if ( ! a ) {
                    //cout << typeid(DataSource<arg_type>).name() << endl;
                    ORO_THROW_OR_RETURN(interface::wrong_types_of_args_exception( argnbr, DataSource<ds_arg_type>::GetType(), front->getType() ), type());
                }

                return type(a);
            }

            static atype assignable(const std::vector<base::DataSourceBase::shared_ptr>& args, int argnbr = 1)
            {
                assert( args.size() == 1);
                base::DataSourceBase::shared_ptr front = args.front();

                typename AssignableDataSource<ds_arg_type>::shared_ptr a =
                    boost::dynamic_pointer_cast< AssignableDataSource<ds_arg_type> >( front );
                if ( ! a ) {
                    //cout << typeid(DataSource<arg_type>).name() << endl;
                    ORO_THROW_OR_RETURN(interface::wrong_types_of_args_exception( argnbr, DataSource<ds_arg_type>::GetType(), front->getType() ), type());
                }

                return atype(a);
            }

            /**
             * Returns the data contained in the data source
             * as a Fusion Sequence.
             * @param seq A Fusion Sequence of DataSource<T> types.
             * @return A sequence of type T holding the values of the DataSource<T>.
             */
            static data_type data(const type& seq) {
                return data_type( GetPointerWrap<type,bare_type>()(seq) );
            }

            static void update(const type&seq) {
                UpdateHelper<arg_type>::update( bf::front(seq) );
                return;
            }

            static void set(const data_type& in, const atype& seq) {
                AssignHelper<atype, data_type>::set(seq, in);
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
                return DataSource<ds_arg_type>::GetType();
            }
        };

        template<class List>
        struct create_sequence_impl<List, 0> // empty mpl list
        {
            typedef bf::vector<> data_type;

            // the result sequence type is a cons of the last argument in the vector.
            typedef bf::vector<> type;

            typedef bf::vector<> atype;

            static type sources(const std::vector<base::DataSourceBase::shared_ptr>& args, int argnbr = 0)
            {
                assert( args.size() == 0);
                return type();
            }

            static atype assignable(const std::vector<base::DataSourceBase::shared_ptr>& args, int argnbr = 0)
            {
                assert( args.size() == 0);
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
