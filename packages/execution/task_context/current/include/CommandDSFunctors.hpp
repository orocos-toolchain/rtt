#ifndef ORO_COMMAND_DS_FUNCTORS_HPP
#define ORO_COMMAND_DS_FUNCTORS_HPP

#include <boost/type_traits.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>

namespace ORO_Execution
{
    namespace detail {

        template<typename FunctionT>
        class FunctorDS0
        {
        public:
            typedef FunctionT Function;
            typedef typename boost::remove_pointer<typename FunctionT::arg1_type>::type CompT;
            FunctionT fun;
            typename DataSource<boost::weak_ptr<CompT> >::shared_ptr ds;
            typedef boost::weak_ptr<CompT> CompW;
            typedef boost::shared_ptr<CompT> CompS;

            FunctorDS0( DataSource<CompW>* c, FunctionT f )
                : fun( f ), ds(c)
            {}

            void setArguments( DataSourceBase* = 0, DataSourceBase* = 0, DataSourceBase* = 0, DataSourceBase* = 0  ) {}
            void readArguments(){}
      
            bool execute()
            {
                // the Component pointer is stored in a DataSource
                CompS c = ds->get().lock();
                if (c) {
                    CompT* ct = c.get();
                    return fun( ct );
                } else {
                    return false; // destroyed.
                }
            }

            bool evaluate()
            {
                // logical XOR :
                boost::shared_ptr<CompT> c = ds->get().lock();
                if (c){
                    CompT* ct = c.get();
                    return fun( ct );
                } else
                    return false;
            }

            FunctorDS0<FunctionT> copy( std::map<const ORO_CoreLib::DataSourceBase*, ORO_CoreLib::DataSourceBase*>& alreadyCloned ) const
            {
                return FunctorDS0( ds->copy(alreadyCloned), fun );
            }
        };

        /**
         * A functor that stores a function needing one argument,
         * and one DataSource to get the data from.
         */
        template<typename FunctionT>
        class FunctorDS1
        {
        public:
            typedef FunctionT Function;
            // arg1_type is the component type.
            typedef typename boost::remove_pointer<typename FunctionT::arg1_type>::type CompT;
            typedef typename FunctionT::arg2_type Arg2T;
            FunctionT fun;
            typename DataSource<Arg2T>::shared_ptr aa;
            typename DataSource<boost::weak_ptr<CompT> >::shared_ptr ds;

            FunctorDS1( DataSource<boost::weak_ptr<CompT> >* c, FunctionT f, DataSource<Arg2T>* a = 0)
                : fun( f ), aa( a ), ds(c)
            {
            }

            void setArguments( DataSource<Arg2T>* a, DataSourceBase* = 0, DataSourceBase* = 0, DataSourceBase* = 0  ) 
            {
                aa = a;
            }

            void readArguments()
            {
                aa->evaluate();
            }
      
            bool execute()
            {
                boost::shared_ptr<CompT> c =  ds->get().lock();
                if (c){
                    CompT* ct = c.get();
                    Arg2T a = aa->value();
                    bool r= fun( ct, a );
                    return r;
                } else
                    return false;
            }

            bool evaluate()
            {
                Arg2T a =  aa->value();
                boost::shared_ptr<CompT> c = ds->get().lock();
                // logical XOR :
                if (c) {
                    CompT* ct = c.get();
                    bool r= fun( ct, a );
                    aa->updated();
                    return r;
                } else
                    return false;
            }

            FunctorDS1<FunctionT> copy( std::map<const ORO_CoreLib::DataSourceBase*, ORO_CoreLib::DataSourceBase*>& alreadyCloned ) const
            {
                return FunctorDS1( ds->copy(alreadyCloned), fun, aa->copy( alreadyCloned ) );
            }
        };
        
    }
}

#endif
