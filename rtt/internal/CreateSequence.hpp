#ifndef ORO_CREATESEQUENCE_HPP_
#define ORO_CREATESEQUENCE_HPP_

#include <boost/fusion/include/cons.hpp>
#include <boost/fusion/include/front.hpp>

#include <vector>
#include <boost/mpl/front.hpp>
#include <boost/mpl/pop_front.hpp>
// The fusion <--> MPL link header
#include <boost/fusion/mpl.hpp>

#include "DataSource.hpp"
#include "DataSourceAdaptor.hpp"

namespace RTT
{
    namespace internal
    {
        namespace bf = boost::fusion;
        namespace mpl = boost::mpl;

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
         */
        template<class List>
        struct create_sequence: public create_sequence_impl<List, mpl::size<
                List>::value>
        {
        };

        template<class List, int size>
        struct create_sequence_impl
        {
            typedef typename mpl::front<List>::type arg_type;

            typedef typename create_sequence<
                    typename mpl::pop_front<List>::type>::data_type arg_tail_type;

            /**
             * The type of a single element of the vector.
             */
            typedef typename DataSource<arg_type>::shared_ptr element_type;

            /**
             * The type of the tail (List - head) of our sequence. It is recursively formulated
             * in terms of create_sequence.
             */
            typedef typename create_sequence<
                    typename mpl::pop_front<List>::type>::type tail_type;

            /**
             * The joint DataSource<T>::shared_ptr type of head and tail, again a fusion view.
             */
            typedef bf::cons<element_type, tail_type> type;

            /**
             * The joint T data type of head and tail.
             */
            typedef bf::cons<arg_type, arg_tail_type> data_type;

            /**
             * Converts a std::vector of DataSourceBase types into a boost::fusion Sequence
             * of the types given in List. Will throw if an element of the vector could not
             * be dynamic casted to the respective element type of List.
             * @param args A vector of data sources
             * @param argnbr Leave as default. Used internally to count recursive calls.
             * @return a Fusion Sequence of DataSource<T>::shared_ptr objects
             */
            type operator()(std::vector<base::DataSourceBase*> args, int argnbr = 1 )
            {
                base::DataSourceBase* front = args.front();

                DataSource<arg_type>* a =
                    AdaptDataSource<arg_type>()( DataSourceTypeInfo<arg_type>::getTypeInfo()->convert(front) );
                if ( ! a )
                    ORO_THROW_OR_RETURN(wrong_types_of_args_exception( argnbr, DataSource<arg_type>::GetType(), front->getType() ), type());

                args.erase(args.begin());
                return bf::cons<element_type, tail_type>(
                        element_type(front),
                        tail_type()(
                                args, ++argnbr));
            }

            /**
             * Returns the data contained in the data sources
             * as a Fusion Sequence.
             * @param seq A Fusion Sequence of DataSource<T> types.
             * @return A sequence of type T holding the values of the DataSource<T>.
             */
            static data_type data(const type& seq) {
                return data_type( bf::front(seq)->get(), tail_type::data( bf::pop_front(seq) ) );
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
                return type( bf::front(seq)->copy(alreadyCloned), tail_type::data( bf::pop_front(seq) ) );
            }
        };

        template<class List>
        struct create_sequence_impl<List, 1> // mpl list of one
        {
            typedef typename mpl::front<List>::type arg_type;
            typedef bf::cons<arg_type> data_type;

            // the result sequence type is a cons of the last argument in the vector.
            typedef bf::cons<typename DataSource<arg_type>::shared_ptr> type;

            type operator()(const std::vector<double>& args, int argnbr = 1)
            {
                base::DataSourceBase* front = args.front();

                DataSource<arg_type>* a =
                    AdaptDataSource<arg_type>()( DataSourceTypeInfo<arg_type>::getTypeInfo()->convert(front) );
                if ( ! a )
                    ORO_THROW_OR_RETURN(wrong_types_of_args_exception( argnbr, DataSource<arg_type>::GetType(), front->getType() ), type());

                return type(a);
            }

            /**
             * Returns the data contained in the data source
             * as a Fusion Sequence.
             * @param seq A Fusion Sequence of DataSource<T> types.
             * @return A sequence of type T holding the values of the DataSource<T>.
             */
            static data_type data(const type& seq) {
                return data_type( bf::front(seq)->get() );
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
        };

    }
}

#endif /* ORO_CREATESEQUENCE_HPP_ */
