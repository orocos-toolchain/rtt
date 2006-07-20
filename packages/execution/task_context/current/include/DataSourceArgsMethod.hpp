#ifndef ORO_DATASOURCE_ARGS_METHOD_HPP
#define ORO_DATASOURCE_ARGS_METHOD_HPP

#include "DataSource.hpp"
#include "LocalMethod.hpp"
#include "TemplateMemberFactory.hpp"

namespace RTT
{
    namespace detail
    {
        /**
         * A method which gets its arguments from a data source and
         * is a datasource itself.
         */
        template<class SignatureT, class FunctorT = detail::FunctorDataSource<boost::function<SignatureT> > >
        class DataSourceArgsMethod
            : public DataSource< typename boost::function_traits<SignatureT>::result_type >
        {
            typename FunctorT::shared_ptr mmeth;
        public:
            typedef boost::function_traits<SignatureT> traits;
            typedef SignatureT Signature;

            typedef typename boost::function_traits<Signature>::result_type result_type;
            typedef DataSource<result_type> Base;

            DataSourceArgsMethod(boost::function<Signature> meth)
                : mmeth( new FunctorT(meth) )
            {
            }

            DataSourceArgsMethod( typename FunctorT::shared_ptr ds)
                : mmeth( ds )
            {
            }

            DataSourceArgsMethod<Signature,FunctorT>* create() const
            {
                return clone();
            }

            template<class Arg1T>
            DataSourceArgsMethod<Signature,FunctorT>* create(DataSource<Arg1T>* a1) const
            {
                DataSourceArgsMethod<Signature,FunctorT>* r =  this->clone();
                r->mmeth->setArguments(a1);
                return r;
            }

            template<class Arg1T, class Arg2T>
            DataSourceArgsMethod<Signature,FunctorT>* create(DataSource<Arg1T>* a1, DataSource<Arg2T>* a2) const
            {
                DataSourceArgsMethod<Signature,FunctorT>* r =  this->clone();
                r->mmeth->setArguments(a1, a2);
                return r;
            }

            template<class Arg1T, class Arg2T, class Arg3T>
            DataSourceArgsMethod<Signature,FunctorT>* create(DataSource<Arg1T>* a1, DataSource<Arg2T>* a2, DataSource<Arg3T>* a3) const
            {
                DataSourceArgsMethod<Signature,FunctorT>* r =  this->clone();
                r->mmeth->setArguments(a1, a2, a3);
                return r;
            }

            template<class Arg1T, class Arg2T, class Arg3T, class Arg4T>
            DataSourceArgsMethod<Signature,FunctorT>* create(DataSource<Arg1T>* a1, DataSource<Arg2T>* a2, DataSource<Arg3T>* a3, DataSource<Arg4T>* a4) const
            {
                DataSourceArgsMethod<Signature,FunctorT>* r =  this->clone();
                r->mmeth->setArguments(a1, a2, a3, a4);
                return r;
            }

            result_type operator()() {
                return mmeth->get();
            }

            virtual result_type get() const {
                return mmeth->get();
            }

            virtual result_type value() const {
                return mmeth->value();
            }

            virtual DataSourceArgsMethod<Signature,FunctorT>* clone() const {
                return new DataSourceArgsMethod( typename FunctorT::shared_ptr(mmeth->clone()) );
            }

            virtual DataSource<result_type>* copy( std::map<const DataSourceBase*, DataSourceBase*>& alreadyCloned ) const
            {
                return new DataSourceArgsMethod<Signature,FunctorT>( typename FunctorT::shared_ptr(mmeth->copy(alreadyCloned)) );
            }

            boost::function<Signature> getMethodFunction() const {
                return mmeth.ff.gen;
            }
        };
    }
}


#endif
