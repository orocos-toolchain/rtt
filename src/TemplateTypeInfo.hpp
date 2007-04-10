/***************************************************************************
  tag: Peter Soetens  Mon Jan 19 14:11:26 CET 2004  Types.hpp 

                        Types.hpp -  description
                           -------------------
    begin                : Mon January 19 2004
    copyright            : (C) 2004 Peter Soetens
    email                : peter.soetens@mech.kuleuven.ac.be
 
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
 
#ifndef ORO_TEMPLATE_TYPEINFO_HPP
#define ORO_TEMPLATE_TYPEINFO_HPP

#include "Types.hpp"
#include "Property.hpp"
#include "Attribute.hpp"
#include "Logger.hpp"
#include <ostream>
#include "FunctorFactory.hpp"
#include "DataSourceArgsMethod.hpp"

#include <boost/type_traits/function_traits.hpp>
#include <boost/function_types/function_type.hpp>
#include <boost/function_types/function_type_signature.hpp>

#include "rtt-config.h"
#ifdef OROPKG_CORBA
#include "corba/ExpressionProxy.hpp"
#endif

namespace RTT
{
    namespace detail {
        template<typename T, bool b_value>
        struct TypeStreamSelector;
        template<typename T>
        struct TypeStreamSelector<T,true>
        {
            static std::ostream& write(std::ostream& os, T t)
            {
#ifdef OS_HAVE_STREAMS
                os << t;
#endif
                return os;
            }
            static std::istream& read(std::istream& os, T& t)
            {
#ifdef OS_HAVE_STREAMS
                os >> t;
#endif
                return os;
            }
        };            
        template<typename T>
        struct TypeStreamSelector<T,false>
        {
            static std::ostream& write(std::ostream& os, T)
            {
                return os;
            }
            static std::istream& read(std::istream& os, T& )
            {
                return os;
            }
        };
    }

    /**
     * This helper class allows user types to be added to Orocos.
     * It provides 'default' implementations for each conversion type.
     * For user defined types, this is very likely not satisfactory and
     * the user needs to override the methods of this class in a subclass
     * or provide specialised global functions.
     * @param T The user class type.
     * @param ParamT The type used to pass the value as a parameter. This
     * defaults to T, but if T allocates upon copy (like std::string or std::vector),
     * ParamT tells Orocos how to pass it, probably by const& or &.
     * @param use_ostream When set to \a true, the class will use operator<<(std::ostream&, T)
     * to write out the type to a stream. When set to \a false, the class will use this function
     * and write '( \a type \a name )' to a stream instead. Defaults to \a false. Set to \a true
     * if your class \a T has the above mentioned function.
     * @see TypeInfoRepository.
     * @see composeType
     * @see decomposeType
     * @see operator<<
     *
     * @section compose composeTypeImpl() and decomposeTypeImpl()
     * The decomposeTypeImpl function is a function which converts user data
	 * into property bags of native C++ primitive types. The primitive types are then 
     * typically used to write the data to screen or to disk. The inverse operation
     * is done by composeTypeImpl, which reassembles a Property from its composite parts.
	 *
	 * Every new user type used as a property will require these two
	 * functions if it must be 'written to a file'. The subclass must implement these
     * functions by using composeTypeImpl() and decomposeTypeImpl().
     * such that class \a T is decomposed in primitive property types.
	 *
     * Example for type MyClass :
     * \code 
     *  struct MyClass {
     *      bool var1;
     *      double var2;
     *  };
     *
     *  bool decomposeTypeImpl(const MyClass &c, PropertyBag& result)
     *  {
     *      // Decode c into primitive properties Var1 and Var2 which are in a PropertyBag of the
     *      // bag-type "MyClass" with the same name as the Property 'c'.
     *	    result.setType("MyClass");
     *
     *      // Put var1 and var2 in the bag
     *      result.add( new Property<bool>("Var1","Description", c.var1 );
     *      result.add( new Property<double("Var2","Description", c.var2 );
     *	    
     *      return true; // done !
     *  } 
     *  bool composeTypeImpl(const PropertyBag& bag, MyClass &result)
     *  {
     *      // Read the variables from the bag and write them to result.
     *	    if ( bag.getType() != "MyClass") {
     *         std::cerr << "Wrong type encountered when composing "<< this->getTypeName()<<std::endl;
     *         return false;
     *      }
     *      if ( bag.find("Var1") == 0 || bag.find("Var2") == 0 ) { 
     *         std::cerr << "Missing properties when composing "<< this->getTypeName()<<std::endl;
     *         return false;
     *      }
     *      // Read var1 and var2 from the bag
     *      result.var1 = bag.getProperty<bool>("Var1")->get();
     *      result.var2 = bag.getProperty<bool>("Var2")->get();
     *	    
     *      return true; // done !
     *  } 
     * \endcode 
     */
    template<typename T, bool use_ostream = false> //, typename ParamT = T>
    class TemplateTypeInfo
        : public TypeInfo
    {
        const std::string tname;
    public:
        using TypeInfo::buildConstant;
        using TypeInfo::buildVariable;

        /**
         * The given \a T parameter is the type for reading DataSources.
         */
        typedef T UserType;
        /**
         * When Properties of \a T are constructed, they are non-const, non-reference.
         */
        typedef typename Property<T>::DataSourceType PropertyType;

        /**
         * Setup Type Information for type \a name.
         * This causes a switch from 'unknown' type to basic
         * type information for type T.
         * @param name the 'Orocos' type name.
         * 
         */
        TemplateTypeInfo(std::string name) 
            : tname(name)
        {
            // Install the type info object for T.
            if ( detail::DataSourceTypeInfo<T>::value_type_info::TypeInfoObject != 0) {
                Logger::log() << Logger::Warning << "Overriding TypeInfo for '" 
                              << detail::DataSourceTypeInfo<T>::value_type_info::TypeInfoObject->getTypeName()
                              << "'." << Logger::endl;
                delete detail::DataSourceTypeInfo<T>::value_type_info::TypeInfoObject;
            }
            detail::DataSourceTypeInfo<T>::value_type_info::TypeInfoObject = this;

        }

        AttributeBase* buildConstant(std::string name, DataSourceBase::shared_ptr dsb) const
        {
            typename DataSource<PropertyType>::shared_ptr res = AdaptDataSource<PropertyType>()(dsb);
            if ( res ) {
                res->get();
                Logger::log() << Logger::Info << "Building "<<tname<<" Constant '"<<name<<"' with value "<< dsb->getTypeInfo()->toString(dsb) <<Logger::endl;
                return new Constant<PropertyType>( name, res->value() );
            }
            else
                return 0;
        }

        AttributeBase* buildVariable(std::string name) const
        {
            // A variable starts its life as unbounded.
            Logger::log() << Logger::Debug << "Building variable '"<<name <<"' of type " << tname <<Logger::endl;
            return new Attribute<T>( name, new detail::UnboundDataSource<ValueDataSource<T> >() );
        }

        AttributeBase* buildAttribute( std::string name, DataSourceBase::shared_ptr in) const
        {
            typename AssignableDataSource<PropertyType>::shared_ptr ds;
            if ( !in )
                ds = new ValueDataSource<PropertyType>();
            else
                ds = AssignableDataSource<PropertyType>::narrow( in.get() );
            if (!ds)
                return 0;
            // A variable starts its life as unbounded.
            Logger::log() << Logger::Debug << "Building Attribute '"<< name <<"' of type " << tname <<Logger::endl;
            return new Attribute<PropertyType>( name, ds.get() );
        }

        AttributeBase* buildAlias(std::string name, DataSourceBase::shared_ptr in ) const
        {
            DataSource<T>* ds = AdaptDataSource<T>()( in );
            if ( ! ds )
                return 0;
            return new Alias<T>( name, ds );
        }

        virtual const std::string& getTypeName() const { return tname; }

        virtual PropertyBase* buildProperty(const std::string& name, const std::string& desc, DataSourceBase::shared_ptr source = 0) const {
            if (source) {
                AssignableDataSource<PropertyType>* ad
                    = AdaptAssignableDataSource<PropertyType>()( source );
                return new Property<PropertyType>(name, desc, ad );
            }
            return new Property<PropertyType>(name, desc);
        }

        virtual DataSourceBase::shared_ptr buildValue() const {
            return new ValueDataSource<PropertyType>();
        }

        virtual std::ostream& write( std::ostream& os, DataSourceBase::shared_ptr in ) const {
            DataSource<T>* d = AdaptDataSource<T>()( in );
            if ( d && use_ostream )
                detail::TypeStreamSelector<T, use_ostream>::write( os, d->value() );
            else {
#ifdef OS_HAVE_STREAMS
                std::string output = std::string("(")+ in->getTypeName() +")";
                os << output;
#endif
            }
            return os;
            //return os << "("<< tname <<")"
        }

        virtual std::istream& read( std::istream& os, DataSourceBase::shared_ptr out ) const {
            AssignableDataSource<T>* d = AdaptAssignableDataSource<T>()( out );
            if ( d && use_ostream ) {
                detail::TypeStreamSelector<T, use_ostream>::read( os, d->set() );
                d->updated(); // because use of set().
            }
            else {
#ifdef OS_HAVE_STREAMS
                // NOP
#endif
            }
            return os;
        }

        virtual bool decomposeType( DataSourceBase::shared_ptr source, PropertyBag& targetbag ) const {
            // Extract typed values
            typename DataSource<PropertyType>::shared_ptr ds = AdaptDataSource<PropertyType>()( source.get() );
            if ( !ds )
                return false; // happens in the case of 'unknown type'
            // Try user's function.
            return decomposeTypeImpl( ds->get(), targetbag );
        }

        /**
         * User, implement this function. Add the structural elements of source to targetbag.
         */
        virtual bool decomposeTypeImpl( typename AssignableDataSource<T>::const_reference_t source, PropertyBag& targetbag ) const {
            return false;
        }

        virtual bool composeType( DataSourceBase::shared_ptr source, DataSourceBase::shared_ptr result) const {
            // First, try a plain update.
            if ( result->update( source.get() ) )
                return true;

            // Did not work, maybe the type needs to be composed.
            const DataSource<PropertyBag>* pb = dynamic_cast< const DataSource<PropertyBag>* > (source.get() );
            if ( !pb )
                return false;
            typename AssignableDataSource<PropertyType>::shared_ptr ads = AdaptAssignableDataSource<PropertyType>()( result.get() );
            if ( !ads )
                return false;

            if ( composeTypeImpl( pb->value(), ads->set() ) )
                ads->updated();
            else {
                Logger::log() <<Logger::Debug<<"Failed to compose from "<< source->getTypeName() <<Logger::endl;
                return false;
            }
            Logger::log() <<Logger::Debug<<"Successfuly composed type from "<< source->getTypeName() <<Logger::endl;
            return true;
        }

        /**
         * User, implement this function. Extract the structural elements in source to result.
         */
        virtual bool composeTypeImpl(const PropertyBag& source,  typename AssignableDataSource<T>::reference_t result) const {
            return false;
        }

        virtual CORBA::Any* createAny(DataSourceBase::shared_ptr source) const {
#ifdef OROPKG_CORBA
            DataSource<T>* d = AdaptDataSource<T>()( source );
            if ( d )
                return AnyConversion<PropertyType>::createAny( d->value() );
#endif
            return 0;
        }

        virtual bool update(const CORBA::Any& any, DataSourceBase::shared_ptr target) const {
#ifdef OROPKG_CORBA
            //This line causes a compile error in DataSourceAdaptor.hpp (where the bug is)
            //Only narrow.
//             AssignableDataSource<T>* ad = AdaptAssignableDataSource<T>()( target );
            AssignableDataSource<T>* ad = AssignableDataSource<T>::narrow( target.get() );
            if ( ad ) {
                PropertyType value;
                if (AnyConversion<PropertyType>::update(any, value ) ) {
                    ad->set( value );
                    return true;
                }
            }
#endif
            return false;
        }

        virtual DataSourceBase* buildCorbaProxy( Corba::Expression* e ) const {
            DataSourceBase* result = 0;
#ifdef OROPKG_CORBA
            // first try as assignable DS, if not possible, try as normal DS.
            result = Corba::ExpressionProxy::NarrowAssignableDataSource<PropertyType>( e );
            if (!result )
                result = Corba::ExpressionProxy::NarrowDataSource<PropertyType>( e );
#endif
            return result;;
        }
    };

    template< class T>
    struct AlwaysAssignChecker
        : public std::binary_function<T, T, bool>
    {
        bool operator()(const T&, const T& ) const
        {
            return true;
        }
    };

    template< class T>
    struct SizeAssignChecker
        : public std::binary_function<T, T, bool>
    {
        bool operator()(const T& v1, const T& v2) const
        {
            // v1 may be assigned to v2 if it has sufficient capacity.
            return v1.capacity() < v2.size();
        }
    };

    // check the validity of an index
    template< class T>
    struct ArrayIndexChecker
        : public std::binary_function< T, int, bool>
    {
        bool operator()(const T& v, int i ) const
        {
            return i > -1 && i < (int)(v.size());
        }
    };

#if 0
  // check the validity of an index
    template< class T>
    struct MultiVectorIndexChecker
        : public std::binary_function< T, int, bool>
    {
        bool operator()(const T& v, int i ) const
        {
            return i > -1 && i < T::size;
        }
    };
#endif

  // check the validity of a fixed range index
    template< class T, int Range>
    struct RangeIndexChecker
        : public std::binary_function< T, int, bool>
    {
        bool operator()(const T& v, int i ) const
        {
            return i > -1 && i < Range;
        }
    };

    /**
     * Template for containers.
     */
    template<typename T, typename IndexType, typename SetType, typename IPred, typename APred, bool has_ostream>
    class TemplateContainerTypeInfo
        : public TemplateTypeInfo<T,has_ostream>
    {
        typedef typename boost::remove_const<typename boost::remove_reference<T>::type>::type _T;
    public:
        using TypeInfo::buildConstant;
        using TypeInfo::buildVariable;

        TemplateContainerTypeInfo(std::string name)
            : TemplateTypeInfo<T, has_ostream>(name) {}

        AttributeBase* buildVariable(std::string name) const
        {
            // no sizehint, but _do_ check IPred.
            return new Attribute<T>( name, new detail::UnboundDataSource<IndexedValueDataSource<T, IndexType, SetType, IPred, AlwaysAssignChecker<_T> > >() );
        }

        DataSourceBase::shared_ptr buildValue() const
        {
            Logger::log() <<Logger::Debug << "Building Container of "<< this->getTypeName()<<Logger::endl;
            return new IndexedValueDataSource<T, IndexType, SetType, IPred, AlwaysAssignChecker<_T> >();
        }

        AttributeBase* buildVariable(std::string name,int size) const
        {
            // if a sizehint is given, create a TaskIndexContainerVariable instead,
            // which checks capacities.
            _T t_init(size, SetType());

            return new Attribute<T>( name, new detail::UnboundDataSource<IndexedValueDataSource<T, IndexType, SetType, IPred, APred> >( t_init ) );
        }
    };

    /**
     * Template for indexable types.
     */
    template<typename T, typename IndexType, typename SetType, typename IPred, typename APred, bool has_ostream>
    class TemplateIndexTypeInfo
        : public TemplateTypeInfo<T,has_ostream>
    {
        typedef typename boost::remove_const<typename boost::remove_reference<T>::type>::type _T;
    public:
        using TypeInfo::buildConstant;
        using TypeInfo::buildVariable;

        TemplateIndexTypeInfo(std::string name)
            : TemplateTypeInfo<T, has_ostream>(name) {}

        AttributeBase* buildVariable(std::string name) const
        {
            // no sizehint, but _do_ check IPred.
            return new Attribute<T>( name, new detail::UnboundDataSource<IndexedValueDataSource<T, IndexType, SetType, IPred, AlwaysAssignChecker<T> > >() );
        }

        DataSourceBase::shared_ptr buildValue() const
        {
            Logger::log() <<Logger::Debug << "Building Indexable value of "<< this->getTypeName()<<Logger::endl;
            return new IndexedValueDataSource<T, IndexType, SetType, IPred, AlwaysAssignChecker<T> >();
        }

    };

    namespace detail
    {

        template<class Signature, int>
        struct TemplateConstructor;

        template<class S>
        struct TemplateConstructor<S,1>
            : public TypeBuilder,
              public FunctorFactoryPart1<DataSourceBase*, DataSourceArgsMethod<S> >
        {
            template<class FInit>
            TemplateConstructor( FInit f)
                : FunctorFactoryPart1<DataSourceBase*, DataSourceArgsMethod<S> >(f )
            {}

            virtual DataSourceBase::shared_ptr build(const std::vector<DataSourceBase::shared_ptr>& args) const {
                try {
                    return this->produce( args );
                } catch ( ... ) {
                }
                return DataSourceBase::shared_ptr();
            }
            
        };

        template<class S>
        struct TemplateConstructor<S,2>
            : public TypeBuilder,
              public FunctorFactoryPart2<DataSourceBase*, DataSourceArgsMethod<S> >
        {
            template<class FInit>
            TemplateConstructor( FInit f)
                : FunctorFactoryPart2<DataSourceBase*, DataSourceArgsMethod<S> >(f )
            {}

            virtual DataSourceBase::shared_ptr build(const std::vector<DataSourceBase::shared_ptr>& args) const {
                try {
                    return this->produce( args );
                } catch ( ... ) {
                }
                return DataSourceBase::shared_ptr();
            }
            
        };

        template<class S>
        struct TemplateConstructor<S,3>
            : public TypeBuilder,
              public FunctorFactoryPart3<DataSourceBase*, DataSourceArgsMethod<S> >
        {
            template<class FInit>
            TemplateConstructor( FInit f)
                : FunctorFactoryPart3<DataSourceBase*, DataSourceArgsMethod<S> >(f )
            {}

            virtual DataSourceBase::shared_ptr build(const std::vector<DataSourceBase::shared_ptr>& args) const {
                try {
                    return this->produce( args );
                } catch ( ... ) {
                }
                return DataSourceBase::shared_ptr();
            }
            
        };

        template<class S>
        struct TemplateConstructor<S,4>
            : public TypeBuilder,
              public FunctorFactoryPart4<DataSourceBase*, DataSourceArgsMethod<S> >
        {
            template<class FInit>
            TemplateConstructor( FInit f)
                : FunctorFactoryPart4<DataSourceBase*, DataSourceArgsMethod<S> >(f )
            {}

            virtual DataSourceBase::shared_ptr build(const std::vector<DataSourceBase::shared_ptr>& args) const {
                try {
                    return this->produce( args );
                } catch ( ... ) {
                }
                return DataSourceBase::shared_ptr();
            }
            
        };

        template<class S>
        struct TemplateConstructor<S,6>
            : public TypeBuilder,
              public FunctorFactoryPart6<DataSourceBase*, DataSourceArgsMethod<S> >
        {
            template<class FInit>
            TemplateConstructor( FInit f)
                : FunctorFactoryPart6<DataSourceBase*, DataSourceArgsMethod<S> >(f )
            {}

            virtual DataSourceBase::shared_ptr build(const std::vector<DataSourceBase::shared_ptr>& args) const {
                try {
                    return this->produce( args );
                } catch ( ... ) {
                }
                return DataSourceBase::shared_ptr();
            }
            
        };
    }

    /**
     * Create a new Constructor.
     * 
     * @param foo A pointer to the 'C' function which creates an object.
     * 
     * @return a Constructor object suitable for the type system.
     */
    template<class Function>
    TypeBuilder* newConstructor( Function* foo ) {
        return new detail::TemplateConstructor<Function, boost::function_traits<Function>::arity>(foo);
    }

    /**
     * Create a new Constructor.
     * 
     * @param obj A function object which has operator().
     * 
     * @return a Constructor object suitable for the type system.
     */
    template<class Object>
    TypeBuilder* newConstructor( Object obj ) {
        return new detail::TemplateConstructor<typename Object::Signature, boost::function_traits<typename Object::Signature>::arity>(obj);
    }
}

#endif
