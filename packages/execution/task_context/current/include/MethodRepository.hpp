#ifndef ORO_METHOD_REPOSITORY_HPP
#define ORO_METHOD_REPOSITORY_HPP

#include "OperationFactory.hpp"
#include "LocalMethod.hpp"
#include "DataSourceArgsMethod.hpp"
#include "MethodC.hpp"

namespace ORO_Execution
{
    /**
     * This class allows storage and retrieval of Method objects.
     * 
     */
    class MethodRepository
        : public OperationFactory<DataSourceBase*>
    {
        template<class T>
        inline T* getpointer(T& t) {
            return &t;
        }
        template<class T>
        inline T* getpointer(T* t) {
            return t;
        }
        
    protected:
        std::map<std::string,ORO_CoreLib::ActionInterface*> simplemethods;
    public:
        typedef MethodFactory Factory;

        ~MethodRepository()
        {
            clear();
        }

        /** 
         * Clear all added methods from the repository, saving memory space.
         */
        void clear() {
            while ( !simplemethods.empty() ) {
                delete simplemethods.begin()->second;
                simplemethods.erase( simplemethods.begin() );
            }
            OperationFactory<DataSourceBase*>::clear();
        }

        /** 
         * Add a Method object to the method interface.
         * 
         * @param meth The Method object to add
         * 
         * @return true if it could be added, false otherwise.
         */
        template<class MethodT>
        bool addMethod( MethodT* meth )
        {
            if ( simplemethods.count( meth->getName() ) )
                return false;
            simplemethods[meth->getName()] = meth->getMethodImpl()->clone();
            return true;
        }

        /** 
         * Get a new Method object from the method interface.
         * 
         * @param name The name of the method to retrieve.
         * @param Signature The function signature of the command, for
         * example: getMethod<int(double)>("name");
         * 
         * @return true if it could be found, false otherwise.
         */
        template<class Signature>
        ORO_CoreLib::ActionInterface* getMethod( std::string name )
        {
            if ( simplemethods.count(name) )
                return simplemethods[name]->clone();
            return 0;
        }
        

        template<class MethodT>
        bool addMethod( MethodT meth, const char* description) 
        {
            typedef typename boost::remove_pointer<MethodT>::type MethodVT;
            typedef typename boost::add_pointer<MethodVT>::type MethodPT;
            MethodPT c = this->getpointer(meth);
            typedef typename MethodVT::Signature Sig;
            if ( this->hasMember(c->getName() ) )
                return false;
            const detail::LocalMethod<Sig>* lm = dynamic_cast< const detail::LocalMethod<Sig>* >( c->getMethodImpl() );
            if ( !lm )
                return false;
            this->add( c->getName(), new detail::OperationFactoryPart0<DataSourceBase*, detail::DataSourceArgsMethod<Sig> >( 
                  detail::DataSourceArgsMethod<Sig>( lm->getMethodFunction()), description) );
            return true;
        }

        template<class MethodT>
        bool addMethod( MethodT meth, const char* description,
                         const char* arg1, const char* arg1_description)
        {
            typedef typename boost::remove_pointer<MethodT>::type MethodVT;
            typedef typename boost::add_pointer<MethodVT>::type MethodPT;
            MethodPT c = this->getpointer(meth);
            typedef typename MethodVT::Signature Sig;
            if ( this->hasMember(c->getName() ) )
                return false;
            const detail::LocalMethod<Sig>* lm = dynamic_cast< const detail::LocalMethod<Sig>* >( c->getMethodImpl() );
            if ( !lm )
                return false;
            this->add( c->getName(), new detail::OperationFactoryPart1<DataSourceBase*, detail::DataSourceArgsMethod<Sig> >( 
                  detail::DataSourceArgsMethod<Sig>(lm->getMethodFunction()), 
                  description, arg1, arg1_description) );
            return true;
        }

        template<class MethodT>
        bool addMethod( MethodT meth, const char* description,
                        const char* arg1, const char* arg1_description,
                        const char* arg2, const char* arg2_description)
        {
            typedef typename boost::remove_pointer<MethodT>::type MethodVT;
            typedef typename boost::add_pointer<MethodVT>::type MethodPT;
            MethodPT c = this->getpointer(meth);
            typedef typename MethodVT::Signature Sig;
            if ( this->hasMember(c->getName() ) )
                return false;
            const detail::LocalMethod<Sig>* lm = dynamic_cast< const detail::LocalMethod<Sig>* >( c->getMethodImpl() );
            if ( !lm )
                return false;
            this->add( c->getName(), new detail::OperationFactoryPart2<DataSourceBase*, detail::DataSourceArgsMethod<Sig> >( 
                  detail::DataSourceArgsMethod<Sig>(lm->getMethodFunction()), 
                  description, 
                  arg1, arg1_description,
                  arg2, arg2_description) );
            return true;
        }
        template<class MethodT>
        bool addMethod( MethodT meth, const char* description,
                        const char* arg1, const char* arg1_description,
                        const char* arg2, const char* arg2_description,
                        const char* arg3, const char* arg3_description)
        {
            typedef typename boost::remove_pointer<MethodT>::type MethodVT;
            typedef typename boost::add_pointer<MethodVT>::type MethodPT;
            MethodPT c = this->getpointer(meth);
            typedef typename MethodVT::Signature Sig;
            if ( this->hasMember(c->getName() ) )
                return false;
            const detail::LocalMethod<Sig>* lm = dynamic_cast< const detail::LocalMethod<Sig>* >( c->getMethodImpl() );
            if ( !lm )
                return false;
            this->add( c->getName(), new detail::OperationFactoryPart3<DataSourceBase*, detail::DataSourceArgsMethod<Sig> >( 
                  detail::DataSourceArgsMethod<Sig>(lm->getMethodFunction()), 
                  description, 
                  arg1, arg1_description,
                  arg2, arg2_description,
                  arg3, arg3_description) );
            return true;
        }

        template<class MethodT>
        bool addMethod( MethodT meth, const char* description,
                        const char* arg1, const char* arg1_description,
                        const char* arg2, const char* arg2_description,
                        const char* arg3, const char* arg3_description,
                        const char* arg4, const char* arg4_description)
        {
            typedef typename boost::remove_pointer<MethodT>::type MethodVT;
            typedef typename boost::add_pointer<MethodVT>::type MethodPT;
            MethodPT c = this->getpointer(meth);
            typedef typename MethodVT::Signature Sig;
            if ( this->hasMember(c->getName() ) )
                return false;
            const detail::LocalMethod<Sig>* lm = dynamic_cast< const detail::LocalMethod<Sig>* >( c->getMethodImpl() );
            if ( !lm )
                return false;
            this->add( c->getName(), new detail::OperationFactoryPart4<DataSourceBase*, detail::DataSourceArgsMethod<Sig> >( 
                  detail::DataSourceArgsMethod<Sig>(lm->getMethodFunction()), 
                  description, 
                  arg1, arg1_description,
                  arg2, arg2_description,
                  arg3, arg3_description,
                  arg4, arg4_description) );
            return true;
        }

        template<class MethodT,class CompT>
        bool addMethodDS( DataSource< boost::weak_ptr<CompT> >* wp, MethodT c, const char* description) 
        {
            using namespace detail;
            typedef typename MethodT::Signature Sig;
            if ( this->hasMember(c.getName() ) )
                return false;
            const detail::LocalMethod<Sig>* lm = dynamic_cast< const detail::LocalMethod<Sig>* >( c.getMethodImpl() );
            if ( !lm )
                return false;
            typedef FunctorDataSourceDS0<CompT, boost::function<Sig> > FunctorT;
            typedef detail::DataSourceArgsMethod<Sig, FunctorT> DSMeth;
            
            this->add( c.getName(), new detail::OperationFactoryPart0<DataSourceBase*, DSMeth>( 
                        DSMeth( typename FunctorT::shared_ptr(new FunctorT(wp, lm->getMethodFunction()))),
                        description));
            return true;
        }

        template<class MethodT,class CompT>
        bool addMethodDS( DataSource< boost::weak_ptr<CompT> >* wp, MethodT c, const char* description, 
                          const char* a1, const char* d1) 
        {
            using namespace detail;
            typedef typename MethodT::Signature Sig;
            if ( this->hasMember(c.getName() ) )
                return false;
            const detail::LocalMethod<Sig>* lm = dynamic_cast< const detail::LocalMethod<Sig>* >( c.getMethodImpl() );
            if ( !lm )
                return false;
            typedef typename MethodT::traits::arg2_type arg1_type; // second arg is 1st data arg.
            typedef FunctorDataSourceDS1<CompT, boost::function<Sig>, arg1_type > FunctorT;
            typedef detail::DataSourceArgsMethod<Sig, FunctorT> DSMeth;
            this->add( c.getName(), new detail::OperationFactoryPart1<DataSourceBase*, DSMeth, arg1_type >( 
                        DSMeth( typename FunctorT::shared_ptr(new FunctorT(wp, lm->getMethodFunction()))),
                        description, a1, d1));
            return true;
        }

        /** 
         * Get a previously added method as a DataSource.
         * This function is inferior to getMethod(std::string name)
         * 
         * @param name The name of the method
         * @param args The arguments of the method as Data Sources.
         * 
         * @return A DataSource which, when evaluated, invokes the method.
         */
        DataSourceBase* getMethod( std::string name,
                                   const std::vector<ORO_CoreLib::DataSourceBase::shared_ptr>& args) const
        {
            return this->produce(name, args);
        }

        /** 
         * Create a MethodC object, a template-less method invocation
         * object. This function is inferior to getMethod(std::string name).
         * 
         * @param name The name of the method
         * 
         * @return An object which can invoke a method.
         */
        MethodC create(std::string name) {
            return MethodC( this, name );
        }
    };
}


#endif
