/***************************************************************************
  tag: FMTC  do nov 2 13:06:08 CET 2006  MethodRepository.hpp 

                        MethodRepository.hpp -  description
                           -------------------
    begin                : do november 02 2006
    copyright            : (C) 2006 FMTC
    email                : peter.soetens@fmtc.be
 
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
 
 
#ifndef ORO_METHOD_REPOSITORY_HPP
#define ORO_METHOD_REPOSITORY_HPP

#include "OperationFactory.hpp"
#include "LocalMethod.hpp"
#include "DataSourceArgsMethod.hpp"
#include "MethodC.hpp"
#ifdef ORO_REMOTING
#include "RemoteMethod.hpp"
#endif
#include <boost/shared_ptr.hpp>
#include <boost/static_assert.hpp>
#include <boost/type_traits/function_traits.hpp>

namespace RTT
{
    /**
     * This class allows storage and retrieval of Method objects.
     * 
     * @ingroup Methods
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
        typedef std::map<std::string,boost::shared_ptr<ActionInterface> > SimpleMethods;
        SimpleMethods simplemethods;
    public:
        typedef MethodFactory Factory;

        ~MethodRepository();

        /** 
         * Clear all added methods from the repository, saving memory space.
         */
        void clear();

        /**
         * Returns the names of all methods added to this interface.
         * @see getNames() to get a list of all methods available to scripting.
         */
        std::vector<std::string> getMethods() const;

        /**
         * Query for the existence of a Method in this interface.
         * @see hasMember() to verify if a method is available to scripting as well.
         */
        bool hasMethod(const std::string& name) const;

        /** 
         * Add a Method object to the method interface. This version
         * of addMethod does not add the Method object to the scripting
         * interface and only to the C++ interface.
         * 
         * @param meth The Method object to add
         * 
         * @return true if it could be added, false otherwise.
         */
        template<class MethodT>
        bool addMethod( MethodT* meth )
        {
            Logger::In in("MethodRepository");
            if ( simplemethods.count( meth->getName() ) ) {
                log(Error) << "Failed to addMethod: '"<< meth->getName() <<"' already added." <<endlog();
                return false;
            }
            if ( meth->getName().empty() || !meth->ready() ) {
                log(Error) << "Failed to addMethod: '"<< meth->getName() <<"' was not ready() or has no name." <<endlog();
                return false;
            }
            simplemethods[meth->getName()] = meth->getMethodImpl();
            return true;
        }

        /** 
         * Get a previously added method for 
         * use in a C++ Method object. Store the result of this
         * function in a Method<\a Signature> object.
         * 
         * @param name The name of the method to retrieve.
         * @param Signature The function signature of the method, for
         * example: getMethod<int(double)>("name");
         * 
         * @return true if it could be found, false otherwise.
         */
        template<class Signature>
        boost::shared_ptr<ActionInterface> getMethod( std::string name )
        {
            Logger::In in("MethodRepository::getMethod");
            if ( simplemethods.count(name) ) {
                if ( boost::dynamic_pointer_cast< detail::MethodBase<Signature> >(simplemethods[name]) )
                    return simplemethods[name];
                else
                    log(Error) << "Method '"<< name <<"' found, but has wrong Signature."<<endlog();
                return boost::shared_ptr<ActionInterface>();
            }

#ifdef ORO_REMOTING
            if ( this->hasMember(name ) ) {
                return boost::shared_ptr<ActionInterface>(new detail::RemoteMethod<Signature>(this, name));
            }
#endif
            log(Warning) << "No such method: "<< name <<endlog();
            return boost::shared_ptr<ActionInterface>();
        }

        /**
         * Removes a previously added method.
         */
        void removeMethod( const std::string& name );

        /** 
         * Add a local method object to the interface. This version
         * of addMethod adds a Method object to the C++ interface and
         * to the scripting interface of this component. 
         * The Method object must refer to a local method function.
         * 
         * @param meth The method object to add.
         * @param description A useful description.
         * 
         * @return true if it could be added.
         */
        template<class MethodT>
        bool addMethod( MethodT meth, const char* description) 
        {
            typedef typename boost::remove_pointer<MethodT>::type MethodVT;
            typedef typename boost::add_pointer<MethodVT>::type MethodPT;
            BOOST_STATIC_ASSERT( boost::function_traits<typename MethodVT::Signature>::arity == 0 );

            MethodPT c = this->getpointer(meth);
            typedef typename MethodVT::Signature Sig;
            const detail::LocalMethod<Sig>* lm = dynamic_cast< const detail::LocalMethod<Sig>* >( c->getMethodImpl().get() );
            if ( !lm ) {
                log(Error) << "Failed to addMethod: '"<< c->getName() <<"' is not a local method." <<endlog();
                return false;
            }
            if ( this->addMethod( c ) == false )
                return false;
            this->add( c->getName(), new detail::OperationFactoryPart0<DataSourceBase*, detail::DataSourceArgsMethod<Sig> >( 
                  detail::DataSourceArgsMethod<Sig>( lm->getMethodFunction()), description) );
            return true;
        }

        /** 
         * Add a local method object to the interface. This version
         * of addMethod adds a Method object to the C++ interface and
         * to the scripting interface of this component. 
         * The Method object must refer to a local method function.
         * 
         * @param meth The method object to add.
         * @param description A useful description.
         * @param arg1 The name of the first argument.
         * @param arg1_description The description of the first argument.
         * 
         * @return true if it could be added.
         */
        template<class MethodT>
        bool addMethod( MethodT meth, const char* description,
                         const char* arg1, const char* arg1_description)
        {
            typedef typename boost::remove_pointer<MethodT>::type MethodVT;
            typedef typename boost::add_pointer<MethodVT>::type MethodPT;
            BOOST_STATIC_ASSERT( boost::function_traits<typename MethodVT::Signature>::arity == 1 );

            MethodPT c = this->getpointer(meth);
            typedef typename MethodVT::Signature Sig;
            const detail::LocalMethod<Sig>* lm = dynamic_cast< const detail::LocalMethod<Sig>* >( c->getMethodImpl().get() );
            if ( !lm ) {
                log(Error) << "Failed to addMethod: '"<< c->getName() <<"' is not a local method." <<endlog();
                return false;
            }
            if ( this->addMethod( c ) == false )
                return false;
            this->add( c->getName(), new detail::OperationFactoryPart1<DataSourceBase*, detail::DataSourceArgsMethod<Sig> >( 
                  detail::DataSourceArgsMethod<Sig>(lm->getMethodFunction()), 
                  description, arg1, arg1_description) );
            return true;
        }

        /** 
         * Add a local method object to the interface. This version
         * of addMethod adds a Method object to the C++ interface and
         * to the scripting interface of this component. 
         * The Method object must refer to a local method function.
         * 
         * @param meth The method object to add.
         * @param description A useful description.
         * @param arg1 The name of the first argument.
         * @param arg1_description The description of the first argument.
         * @param arg2 The name of the second argument.
         * @param arg2_description The description of the second argument.
         * 
         * @return true if it could be added.
         */
        template<class MethodT>
        bool addMethod( MethodT meth, const char* description,
                        const char* arg1, const char* arg1_description,
                        const char* arg2, const char* arg2_description)
        {
            typedef typename boost::remove_pointer<MethodT>::type MethodVT;
            typedef typename boost::add_pointer<MethodVT>::type MethodPT;
            BOOST_STATIC_ASSERT( boost::function_traits<typename MethodVT::Signature>::arity == 2 );

            MethodPT c = this->getpointer(meth);
            typedef typename MethodVT::Signature Sig;
            const detail::LocalMethod<Sig>* lm = dynamic_cast< const detail::LocalMethod<Sig>* >( c->getMethodImpl().get() );
            if ( !lm ) {
                log(Error) << "Failed to addMethod: '"<< c->getName() <<"' is not a local method." <<endlog();
                return false;
            }
            if ( this->addMethod( c ) == false )
                return false;
            this->add( c->getName(), new detail::OperationFactoryPart2<DataSourceBase*, detail::DataSourceArgsMethod<Sig> >( 
                  detail::DataSourceArgsMethod<Sig>(lm->getMethodFunction()), 
                  description, 
                  arg1, arg1_description,
                  arg2, arg2_description) );
            return true;
        }

        /** 
         * Add a local method object to the interface. This version
         * of addMethod adds a Method object to the C++ interface and
         * to the scripting interface of this component. 
         * The Method object must refer to a local method function.
         * 
         * @param meth The method object to add.
         * @param description A useful description.
         * @param arg1 The name of the first argument.
         * @param arg1_description The description of the first argument.
         * @param arg2 The name of the second argument.
         * @param arg2_description The description of the second argument.
         * @param arg3 The name of the third argument.
         * @param arg3_description The description of the third argument.
         * 
         * @return true if it could be added.
         */
        template<class MethodT>
        bool addMethod( MethodT meth, const char* description,
                        const char* arg1, const char* arg1_description,
                        const char* arg2, const char* arg2_description,
                        const char* arg3, const char* arg3_description)
        {
            typedef typename boost::remove_pointer<MethodT>::type MethodVT;
            typedef typename boost::add_pointer<MethodVT>::type MethodPT;
            BOOST_STATIC_ASSERT( boost::function_traits<typename MethodVT::Signature>::arity == 3 );

            MethodPT c = this->getpointer(meth);
            typedef typename MethodVT::Signature Sig;
            const detail::LocalMethod<Sig>* lm = dynamic_cast< const detail::LocalMethod<Sig>* >( c->getMethodImpl().get() );
            if ( !lm ) {
                log(Error) << "Failed to addMethod: '"<< c->getName() <<"' is not a local method." <<endlog();
                return false;
            }
            if ( this->addMethod( c ) == false )
                return false;
            this->add( c->getName(), new detail::OperationFactoryPart3<DataSourceBase*, detail::DataSourceArgsMethod<Sig> >( 
                  detail::DataSourceArgsMethod<Sig>(lm->getMethodFunction()), 
                  description, 
                  arg1, arg1_description,
                  arg2, arg2_description,
                  arg3, arg3_description) );
            return true;
        }

        /** 
         * Add a local method object to the interface. This version
         * of addMethod adds a Method object to the C++ interface and
         * to the scripting interface of this component. 
         * The Method object must refer to a local method function.
         * 
         * @param meth The method object to add.
         * @param description A useful description.
         * @param arg1 The name of the first argument.
         * @param arg1_description The description of the first argument.
         * @param arg2 The name of the second argument.
         * @param arg2_description The description of the second argument.
         * @param arg3 The name of the third argument.
         * @param arg3_description The description of the third argument.
         * @param arg4 The name of the fourth argument.
         * @param arg4_description The description of the fourth argument.
         * 
         * @return true if it could be added.
         */
        template<class MethodT>
        bool addMethod( MethodT meth, const char* description,
                        const char* arg1, const char* arg1_description,
                        const char* arg2, const char* arg2_description,
                        const char* arg3, const char* arg3_description,
                        const char* arg4, const char* arg4_description)
        {
            typedef typename boost::remove_pointer<MethodT>::type MethodVT;
            typedef typename boost::add_pointer<MethodVT>::type MethodPT;
            BOOST_STATIC_ASSERT( boost::function_traits<typename MethodVT::Signature>::arity == 4 );

            MethodPT c = this->getpointer(meth);
            typedef typename MethodVT::Signature Sig;
            const detail::LocalMethod<Sig>* lm = dynamic_cast< const detail::LocalMethod<Sig>* >( c->getMethodImpl().get() );
            if ( !lm ) {
                log(Error) << "Failed to addMethod: '"<< c->getName() <<"' is not a local method." <<endlog();
                return false;
            }
            if ( this->addMethod( c ) == false )
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

        /**
         * For internal use only. The pointer of the object of which a member function
         * must be invoked is stored in a DataSource such that the pointer can change
         * during program execution. Required in scripting for state machines.
         */
        template<class MethodT,class CompT>
        bool addMethodDS( DataSource< boost::weak_ptr<CompT> >* wp, MethodT c, const char* description) 
        {
            using namespace detail;
            typedef typename MethodT::Signature Sig;
            if ( this->hasMember(c.getName() ) )
                return false;
            const detail::LocalMethod<Sig>* lm = dynamic_cast< const detail::LocalMethod<Sig>* >( c.getMethodImpl().get() );
            if ( !lm )
                return false;
            typedef FunctorDataSourceDS0<CompT, boost::function<Sig> > FunctorT;
            typedef detail::DataSourceArgsMethod<Sig, FunctorT> DSMeth;
            
            this->add( c.getName(), new detail::OperationFactoryPart0<DataSourceBase*, DSMeth>( 
                        DSMeth( typename FunctorT::shared_ptr(new FunctorT(wp, lm->getMethodFunction()))),
                        description));
            return true;
        }

        /**
         * For internal use only. The pointer of the object of which a member function
         * must be invoked is stored in a DataSource such that the pointer can change
         * during program execution. Required in scripting for state machines.
         */
        template<class MethodT,class CompT>
        bool addMethodDS( DataSource< boost::weak_ptr<CompT> >* wp, MethodT c, const char* description, 
                          const char* a1, const char* d1) 
        {
            using namespace detail;
            typedef typename MethodT::Signature Sig;
            if ( this->hasMember(c.getName() ) )
                return false;
            const detail::LocalMethod<Sig>* lm = dynamic_cast< const detail::LocalMethod<Sig>* >( c.getMethodImpl().get() );
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
         * For internal use only. Get a previously added method as a DataSource.
         * This function is inferior to getMethod(std::string name)
         * 
         * @param name The name of the method
         * @param args The arguments of the method as Data Sources.
         * 
         * @return A DataSource which, when evaluated, invokes the method.
         */
        DataSourceBase* getMethod( std::string name,
                                   const std::vector<DataSourceBase::shared_ptr>& args) const
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

        /**
         * Reset the implementation of a method.
         */
        bool resetMethod(std::string name, ActionInterface::shared_ptr impl)
        {
            if (!hasMethod(name))
                return false;
            simplemethods[name] = impl;
            return true;
        }
    };
}


#endif
