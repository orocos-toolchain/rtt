#ifndef ORO_DATASOURCE_ARGS_EVENT_HPP
#define ORO_DATASOURCE_ARGS_EVENT_HPP

#include "DataSource.hpp"
#include "LocalEvent.hpp"
#include "TemplateMemberFactory.hpp"

namespace RTT
{
    namespace detail
    {
        /**
         * A event which gets its arguments from a data source and
         * is an action object.
         */
        template<class SignatureT, class FunctorT = detail::FunctorDataSource<boost::function<SignatureT> > >
        class DataSourceArgsEvent
            : public ActionInterface
        {
            typename FunctorT::shared_ptr mmeth;
        public:
            typedef boost::function_traits<SignatureT> traits;
            typedef SignatureT Signature;

            typedef typename boost::function_traits<Signature>::result_type result_type;
            typedef ActionInterface Base;

            DataSourceArgsEvent(boost::function<Signature> meth)
                : mmeth( new FunctorT(meth) )
            {
            }

            DataSourceArgsEvent( typename FunctorT::shared_ptr ds)
                : mmeth( ds )
            {
            }

            DataSourceArgsEvent<Signature,FunctorT>* create() const
            {
                return clone();
            }

            template<class Arg1T>
            DataSourceArgsEvent<Signature,FunctorT>* create(DataSource<Arg1T>* a1) const
            {
                DataSourceArgsEvent<Signature,FunctorT>* r =  this->clone();
                r->mmeth->setArguments(a1);
                return r;
            }

            template<class Arg1T, class Arg2T>
            DataSourceArgsEvent<Signature,FunctorT>* create(DataSource<Arg1T>* a1, DataSource<Arg2T>* a2) const
            {
                DataSourceArgsEvent<Signature,FunctorT>* r =  this->clone();
                r->mmeth->setArguments(a1, a2);
                return r;
            }

            template<class Arg1T, class Arg2T, class Arg3T>
            DataSourceArgsEvent<Signature,FunctorT>* create(DataSource<Arg1T>* a1, DataSource<Arg2T>* a2, DataSource<Arg3T>* a3) const
            {
                DataSourceArgsEvent<Signature,FunctorT>* r =  this->clone();
                r->mmeth->setArguments(a1, a2, a3);
                return r;
            }

            template<class Arg1T, class Arg2T, class Arg3T, class Arg4T>
            DataSourceArgsEvent<Signature,FunctorT>* create(DataSource<Arg1T>* a1, DataSource<Arg2T>* a2, DataSource<Arg3T>* a3, DataSource<Arg4T>* a4) const
            {
                DataSourceArgsEvent<Signature,FunctorT>* r =  this->clone();
                r->mmeth->setArguments(a1, a2, a3, a4);
                return r;
            }

            template<class Arg1T, class Arg2T, class Arg3T, class Arg4T, class Arg5T>
            DataSourceArgsEvent<Signature,FunctorT>* create(DataSource<Arg1T>* a1, DataSource<Arg2T>* a2, DataSource<Arg3T>* a3, DataSource<Arg4T>* a4, DataSource<Arg5T>* a5) const
            {
                DataSourceArgsEvent<Signature,FunctorT>* r =  this->clone();
                r->mmeth->setArguments(a1, a2, a3, a4, a5);
                return r;
            }

            template<class Arg1T, class Arg2T, class Arg3T, class Arg4T, class Arg5T, class Arg6T>
            DataSourceArgsEvent<Signature,FunctorT>* create(DataSource<Arg1T>* a1, DataSource<Arg2T>* a2, DataSource<Arg3T>* a3, DataSource<Arg4T>* a4, DataSource<Arg5T>* a5, DataSource<Arg6T>* a6) const
            {
                DataSourceArgsEvent<Signature,FunctorT>* r =  this->clone();
                r->mmeth->setArguments(a1, a2, a3, a4, a5, a6);
                return r;
            }


            bool execute() {
                mmeth->get();
                return true;
            }

            void readArguments() {
                // the arguments are read in mmeth.get() above.
            }

            virtual DataSourceArgsEvent<Signature,FunctorT>* clone() const {
                return new DataSourceArgsEvent( typename FunctorT::shared_ptr(mmeth->clone()) );
            }

            virtual ActionInterface* copy( std::map<const DataSourceBase*, DataSourceBase*>& alreadyCloned ) const
            {
                return new DataSourceArgsEvent<Signature,FunctorT>( typename FunctorT::shared_ptr(mmeth->copy(alreadyCloned)) );
            }

            boost::function<Signature> getEventFunction() const {
                return mmeth.ff.gen;
            }
        };
    }
}


#endif
