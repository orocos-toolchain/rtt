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
#include "DataSourceAdaptor.hpp"
#include "Exceptions.hpp"
#include "../interface/FactoryExceptions.hpp"

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
        {
          enum {
              value=mpl::or_<
                      boost::is_convertible<T,::boost::shared_ptr<void> >,
                      boost::is_convertible<T,::boost::shared_ptr<const void> > >::value
          };
          typedef typename mpl::or_<
                      typename boost::is_convertible< T,::boost::shared_ptr<void> >,
                      typename boost::is_convertible< T,::boost::shared_ptr<const void> > > type;
        };

        /***
         * Helper class for extracting the bare pointer from a shared_ptr
         * data source. Used in create_sequence::data() to unwrap the shared_ptr;
         */
        template<class Seq, class Data, class Enable = void >
        struct GetPointerWrap {
            Data operator()(Seq s) { return Data(bf::front(s)->get()); /* front(s) is a DataSource<Data> */}
        }; // normal type

        template<class Seq, class Data>
        struct GetPointerWrap<Seq, Data, typename boost::enable_if< is_shared_ptr<typename mpl::front<Seq>::type::element_type::result_t> >::type> {
            Data operator()(Seq s) { return Data(bf::front(s)->get().get()); /* first get is on DS, second get is on shared_ptr.*/ }
        }; // shared_ptr type

        /**
         * Helper to only update data sources that hold references.
         */
        template<class T>
        struct UpdateHelper {
            static void update(typename DataSource<T>::shared_ptr) {}
        };

        template<class T>
        struct UpdateHelper<T&> {
            static void update(typename DataSource<T&>::shared_ptr s) { s->updated(); }
        };

        template<class List, int size>
        struct create_sequence_impl;

        /**
         * This class can create three kinds of Boost Fusion
         * Sequences.
         *
         * opeartor() creates a fusion sequence of DataSource<T>::shared_ptr from an mpl sequence
         * and a std::vector<DataSourceBase*>. Both must have same length.
         * The mpl sequence is typically obtained from the
         * function_types parameter_types traits class.
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
            typedef typename mpl::if_<typename boost::is_pointer<bare_type>::type, boost::shared_ptr<typename boost::remove_pointer<bare_type>::type>, bare_type>::type arg_type;

            /**
             * The type of a single element of the vector.
             */
            typedef typename DataSource<arg_type>::shared_ptr element_type;

            /**
             * The type of the tail (List - head) of our sequence. It is recursively formulated
             * in terms of create_sequence.
             */
            typedef typename tail::type tail_type;

            /**
             * The joint DataSource<T>::shared_ptr type of head and tail, again a fusion view.
             */
            typedef bf::cons<element_type, tail_type> type;

            typedef typename tail::data_type arg_tail_type;

            /**
             * The joint T data type of head and tail.
             */
            typedef bf::cons<bare_type, arg_tail_type> data_type;

            /**
             * Converts a std::vector of DataSourceBase types into a boost::fusion Sequence
             * of the types given in List. Will throw if an element of the vector could not
             * be dynamic casted to the respective element type of List.
             * @param args A vector of data sources
             * @param argnbr Leave as default. Used internally to count recursive calls.
             * @return a Fusion Sequence of DataSource<T>::shared_ptr objects
             */
            type operator()(std::vector<base::DataSourceBase::shared_ptr> args, int argnbr = 1 )
            {
                assert( args.size() == size);
                base::DataSourceBase::shared_ptr front = args.front();

                typename DataSource<arg_type>::shared_ptr a =
                    AdaptDataSource<arg_type>()( DataSourceTypeInfo<arg_type>::getTypeInfo()->convert(front) );
                if ( ! a ) {
                    cout << typeid(DataSource<arg_type>).name() << endl;
                    ORO_THROW_OR_RETURN(interface::wrong_types_of_args_exception( argnbr, DataSource<arg_type>::GetType(), front->getType() ), type());
                    //ORO_THROW_OR_RETURN(wrong_types_of_args_exception( argnbr, typeid(DataSource<arg_type>).name(), typeid(front).name() ), type());
                }

                args.erase(args.begin());
                return bf::cons<element_type, tail_type>(
                        element_type(a),
                        tail()(args, ++argnbr));
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
            static types::TypeInfo* GetTypeInfo(int i) {
                if ( i <= 0 || i > size)
                    return "na";
                if ( i = 1 ) {
                    return DataSource<arg_type>::GetTypeInfo();
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
                if ( i = 1 ) {
                    return DataSource<arg_type>::GetType();
                } else {
                    return tail::GetType(i-1);
                }
            }
};

        template<class List>
        struct create_sequence_impl<List, 1> // mpl list of one
        {
            typedef typename mpl::front<List>::type bare_type;
            typedef typename mpl::if_<typename boost::is_pointer<bare_type>::type, boost::shared_ptr<typename boost::remove_pointer<bare_type>::type>, bare_type>::type arg_type;

            typedef bf::cons<bare_type> data_type;

            // the result sequence type is a cons of the last argument in the vector.
            typedef bf::cons<typename DataSource<arg_type>::shared_ptr> type;

            type operator()(const std::vector<base::DataSourceBase::shared_ptr>& args, int argnbr = 1)
            {
                assert( args.size() == 1);
                base::DataSourceBase::shared_ptr front = args.front();

                typename DataSource<arg_type>::shared_ptr a =
                    AdaptDataSource<arg_type>()( DataSourceTypeInfo<arg_type>::getTypeInfo()->convert(front) );
                if ( ! a ) {
                    cout << typeid(DataSource<arg_type>).name() << endl;
                    ORO_THROW_OR_RETURN(interface::wrong_types_of_args_exception( argnbr, DataSource<arg_type>::GetType(), front->getType() ), type());
                }

                return type(a);
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

            static types::TypeInfo* GetTypeInfo(int i) {
                if ( i != 1)
                    return "na";
                return DataSource<arg_type>::GetTypeInfo();
            }
            static std::string GetType(int i) {
                if ( i != 1)
                    return "na";
                return DataSource<arg_type>::GetType();
            }
        };

        template<class List>
        struct create_sequence_impl<List, 0> // empty mpl list
        {
            typedef bf::vector<> data_type;

            // the result sequence type is a cons of the last argument in the vector.
            typedef bf::vector<> type;

            type operator()(const std::vector<base::DataSourceBase::shared_ptr>& args, int argnbr = 0)
            {
                assert( args.size() == 0);
                return type();
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
            static types::TypeInfo* GetTypeInfo(int i) {
                return "na";
            }
            static std::string GetType(int i) {
                return "na";
            }
        };
    }
}

#endif /* ORO_CREATESEQUENCE_HPP_ */
