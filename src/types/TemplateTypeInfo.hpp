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
#include "../Property.hpp"
#include "../Attribute.hpp"
#include "../Logger.hpp"
#include "../InputPort.hpp"
#include "../OutputPort.hpp"
#include <ostream>
#include "../internal/FunctorFactory.hpp"
#include "../internal/DataSourceArgsMethod.hpp"

#include <boost/type_traits/function_traits.hpp>
#include "../boost/function_types/function_type.hpp"
#include "../boost/function_types/function_type_signature.hpp"

#include "../rtt-config.h"

namespace RTT
{
    namespace types {
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
    template<typename T, bool use_ostream = false>
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
            if ( internal::DataSourceTypeInfo<T>::TypeInfoObject != 0) {
                Logger::log() << Logger::Warning << "Overriding TypeInfo for '"
                              << internal::DataSourceTypeInfo<T>::TypeInfoObject->getTypeName()
                              << "'." << Logger::endl;
            }
            internal::DataSourceTypeInfo<T>::TypeInfoObject = this;

        }

        base::AttributeBase* buildConstant(std::string name, base::DataSourceBase::shared_ptr dsb) const
        {
            typename internal::DataSource<PropertyType>::shared_ptr res =
                internal::AdaptDataSource<PropertyType>()( internal::DataSourceTypeInfo<PropertyType>::getTypeInfo()->convert(dsb));
            if ( res ) {
                res->get();
                Logger::log() << Logger::Info << "Building "<<tname<<" Constant '"<<name<<"' with value "<< dsb->getTypeInfo()->toString(dsb) <<Logger::endl;
                return new Constant<PropertyType>( name, res->value() );
            }
            else
                return 0;
        }

        base::AttributeBase* buildVariable(std::string name) const
        {
            // A variable starts its life as unbounded.
            Logger::log() << Logger::Debug << "Building variable '"<<name <<"' of type " << tname <<Logger::endl;
            return new Attribute<T>( name, new internal::UnboundDataSource<internal::ValueDataSource<T> >() );
        }

        base::AttributeBase* buildAttribute( std::string name, base::DataSourceBase::shared_ptr in) const
        {
            typename internal::AssignableDataSource<PropertyType>::shared_ptr ds;
            if ( !in )
                ds = new internal::ValueDataSource<PropertyType>();
            else
                ds = internal::AssignableDataSource<PropertyType>::narrow( in.get() );
            if (!ds)
                return 0;
            // A variable starts its life as unbounded.
            Logger::log() << Logger::Debug << "Building Attribute '"<< name <<"' of type " << tname <<Logger::endl;
            return new Attribute<PropertyType>( name, ds.get() );
        }

        base::AttributeBase* buildAlias(std::string name, base::DataSourceBase::shared_ptr in ) const
        {
            typename internal::DataSource<T>::shared_ptr ds = internal::AdaptDataSource<T>()( internal::DataSourceTypeInfo<T>::getTypeInfo()->convert(in) );
            if ( ! ds )
                return 0;
            return new Alias<T>( name, ds );
        }

        virtual const std::string& getTypeName() const { return tname; }

        virtual base::PropertyBase* buildProperty(const std::string& name, const std::string& desc, base::DataSourceBase::shared_ptr source = 0) const {
            if (source) {
               typename internal::AssignableDataSource<PropertyType>::shared_ptr ad
                    = internal::AdaptAssignableDataSource<PropertyType>()( source );
                if (ad)
                    return new Property<PropertyType>(name, desc, ad );
                // else ?
            }
            return new Property<PropertyType>(name, desc);
        }

        virtual base::DataSourceBase::shared_ptr buildValue() const {
            return new internal::ValueDataSource<PropertyType>();
        }
        virtual base::DataSourceBase::shared_ptr buildReference(void* ptr) const {
            return new internal::ReferenceDataSource<PropertyType>(*static_cast<PropertyType*>(ptr));
        }

        virtual std::ostream& write( std::ostream& os, base::DataSourceBase::shared_ptr in ) const {
            typename internal::DataSource<T>::shared_ptr d = internal::AdaptDataSource<T>()( in );
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

        virtual std::istream& read( std::istream& os, base::DataSourceBase::shared_ptr out ) const {
            typename internal::AssignableDataSource<T>::shared_ptr d = internal::AdaptAssignableDataSource<T>()( out );
            if ( d && use_ostream ) {
                detail::TypeStreamSelector<T, use_ostream>::read( os, d->set() );
                d->updated(); // because use of set().
            }
            return os;
        }

        virtual bool decomposeType( base::DataSourceBase::shared_ptr source, PropertyBag& targetbag ) const {
            // Extract typed values
            typename internal::DataSource<PropertyType>::shared_ptr ds = internal::AdaptDataSource<PropertyType>()( source.get() );
            if ( !ds )
                return false; // happens in the case of 'unknown type'
            // Try user's function.
            return decomposeTypeImpl( ds->get(), targetbag );
        }

        /**
         * User, implement this function. Add the structural elements of source to targetbag.
         */
        virtual bool decomposeTypeImpl( typename internal::AssignableDataSource<T>::const_reference_t source, PropertyBag& targetbag ) const {
            return false;
        }

        virtual bool composeType( base::DataSourceBase::shared_ptr source, base::DataSourceBase::shared_ptr result) const {
            // First, try a plain update.
            if ( result->update( source.get() ) )
                return true;

            // Did not work, maybe the type needs to be composed.
            const internal::DataSource<PropertyBag>* pb = dynamic_cast< const internal::DataSource<PropertyBag>* > (source.get() );
            if ( !pb )
                return false;
            typename internal::AssignableDataSource<PropertyType>::shared_ptr ads = internal::AdaptAssignableDataSource<PropertyType>()( result.get() );
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
        virtual bool composeTypeImpl(const PropertyBag& source,  typename internal::AssignableDataSource<T>::reference_t result) const {
            return false;
        }
		
		std::string getTypeIdName() const { return typeid(T).name(); }

      
        base::InputPortInterface*  inputPort(std::string const& name) const { return new InputPort<T>(name); }  
        base::OutputPortInterface* outputPort(std::string const& name) const { return new OutputPort<T>(name); }  

        base::ChannelElementBase* buildDataStorage(ConnPolicy const& policy) const {
            return internal::ConnFactory::buildDataStorage<T>(policy);
        }

        base::ChannelElementBase* buildChannelOutput(base::InputPortInterface& port) const
        {
            return internal::ConnFactory::buildChannelOutput(
                    static_cast<RTT::InputPort<T>&>(port), new internal::SimpleConnID());
        }

        base::ChannelElementBase* buildChannelInput(base::OutputPortInterface& port) const
        {
            return internal::ConnFactory::buildChannelInput(
                    static_cast<RTT::OutputPort<T>&>(port), new internal::SimpleConnID(), 0 );
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
            // v2 may be assigned to v1 if it has sufficient capacity.
            return v1.capacity() >= v2.size();
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
        using TemplateTypeInfo<T,has_ostream>::buildConstant;
        using TemplateTypeInfo<T,has_ostream>::buildVariable;

        TemplateContainerTypeInfo(std::string name)
            : TemplateTypeInfo<T, has_ostream>(name) {}

        base::AttributeBase* buildVariable(std::string name) const
        {
            // no sizehint, but _do_ check IPred.
            return new Attribute<T>( name, new internal::UnboundDataSource<internal::IndexedValueDataSource<T, IndexType, SetType, IPred, AlwaysAssignChecker<_T> > >() );
        }

        base::DataSourceBase::shared_ptr buildValue() const
        {
            return new internal::IndexedValueDataSource<T, IndexType, SetType, IPred, AlwaysAssignChecker<_T> >();
        }

        base::AttributeBase* buildVariable(std::string name,int size) const
        {
            // if a sizehint is given, create a TaskIndexContainerVariable instead,
            // which checks capacities.
            _T t_init(size, SetType());

            return new Attribute<T>( name, new internal::UnboundDataSource<internal::IndexedValueDataSource<T, IndexType, SetType, IPred, APred> >( t_init ) );
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
        using TemplateTypeInfo<T,has_ostream>::buildConstant;
        using TemplateTypeInfo<T,has_ostream>::buildVariable;

        TemplateIndexTypeInfo(std::string name)
            : TemplateTypeInfo<T, has_ostream>(name) {}

        base::AttributeBase* buildVariable(std::string name) const
        {
            // no sizehint, but _do_ check IPred.
            return new Attribute<T>( name, new internal::UnboundDataSource<internal::IndexedValueDataSource<T, IndexType, SetType, IPred, AlwaysAssignChecker<T> > >() );
        }

        base::DataSourceBase::shared_ptr buildValue() const
        {
            Logger::log() <<Logger::Debug << "Building Indexable value of "<< this->getTypeName()<<Logger::endl;
            return new internal::IndexedValueDataSource<T, IndexType, SetType, IPred, AlwaysAssignChecker<T> >();
        }

    };

        /**
         * The constructor classes allow to define type constructors
         * or type conversions (convert type B from type A).
         * @see TypeInfo::addConstructor()
         */
        template<class Signature, int>
        struct TemplateConstructor;

        template<class S>
        struct TemplateConstructor<S,1>
            : public TypeBuilder,
              public internal::FunctorFactoryPart1<base::DataSourceBase*, internal::DataSourceArgsMethod<S> >
        {
            typedef typename boost::function_traits<S>::result_type result_type;
            typedef typename boost::function_traits<S>::arg1_type arg1_type;

            bool automatic;
            template<class FInit>
            TemplateConstructor( FInit f, bool autom)
                : internal::FunctorFactoryPart1<base::DataSourceBase*, internal::DataSourceArgsMethod<S> >(f ),
                automatic(autom)
            {}

            virtual base::DataSourceBase::shared_ptr build(const std::vector<base::DataSourceBase::shared_ptr>& args) const {
                // these checks are necessary because produce(args) calls convert, which could lead to endless loops.
                // detect same type converion.
                if ( args.size() == 1 && args[0]->getTypeInfo() == internal::DataSourceTypeInfo<result_type>::getTypeInfo() ) {
                    return args[0];
                }
                // detect invalid type conversion.
                if ( args.size() == 1 && args[0]->getTypeInfo() != internal::DataSourceTypeInfo<arg1_type>::getTypeInfo() ) {
                    return base::DataSourceBase::shared_ptr();
                }
                try {
                    return this->produce( args );
                } catch ( ... ) {
                }
                return base::DataSourceBase::shared_ptr();
            }

            virtual base::DataSourceBase::shared_ptr convert(base::DataSourceBase::shared_ptr arg) const {
                // these checks are necessary because produce(args) calls convert, which could lead to endless loops.
                // detect same type converion.
                if ( arg->getTypeInfo() == internal::DataSourceTypeInfo<result_type>::getTypeInfo() ) {
                    return arg;
                }
                // detect invalid type conversion.
                if ( arg->getTypeInfo() != internal::DataSourceTypeInfo<arg1_type>::getTypeInfo() ) {
                    return base::DataSourceBase::shared_ptr();
                }
                // from now on, it should always succeed.
                std::vector<base::DataSourceBase::shared_ptr> args;
                args.push_back(arg);
                base::DataSourceBase::shared_ptr ret = this->build(args);
                if (ret && !automatic)
                    log(Warning) << "Conversion from " << arg->getTypeName() << " to " << ret->getTypeName() <<endlog();
                return ret;
            }
        };

        template<class S>
        struct TemplateConstructor<S,2>
            : public TypeBuilder,
              public internal::FunctorFactoryPart2<base::DataSourceBase*, internal::DataSourceArgsMethod<S> >
        {
            template<class FInit>
            TemplateConstructor( FInit f, bool autom)
                : internal::FunctorFactoryPart2<base::DataSourceBase*, internal::DataSourceArgsMethod<S> >(f )
            {}

            virtual base::DataSourceBase::shared_ptr build(const std::vector<base::DataSourceBase::shared_ptr>& args) const {
                try {
                    return this->produce( args );
                } catch ( ... ) {
                }
                return base::DataSourceBase::shared_ptr();
            }

        };

        template<class S>
        struct TemplateConstructor<S,3>
            : public TypeBuilder,
              public internal::FunctorFactoryPart3<base::DataSourceBase*, internal::DataSourceArgsMethod<S> >
        {
            template<class FInit>
            TemplateConstructor( FInit f, bool autom)
                : internal::FunctorFactoryPart3<base::DataSourceBase*, internal::DataSourceArgsMethod<S> >(f )
            {}

            virtual base::DataSourceBase::shared_ptr build(const std::vector<base::DataSourceBase::shared_ptr>& args) const {
                try {
                    return this->produce( args );
                } catch ( ... ) {
                }
                return base::DataSourceBase::shared_ptr();
            }

        };

        template<class S>
        struct TemplateConstructor<S,4>
            : public TypeBuilder,
              public internal::FunctorFactoryPart4<base::DataSourceBase*, internal::DataSourceArgsMethod<S> >
        {
            template<class FInit>
            TemplateConstructor( FInit f, bool autom)
                : internal::FunctorFactoryPart4<base::DataSourceBase*, internal::DataSourceArgsMethod<S> >(f )
            {}

            virtual base::DataSourceBase::shared_ptr build(const std::vector<base::DataSourceBase::shared_ptr>& args) const {
                try {
                    return this->produce( args );
                } catch ( ... ) {
                }
                return base::DataSourceBase::shared_ptr();
            }

        };

        template<class S>
        struct TemplateConstructor<S,6>
            : public TypeBuilder,
              public internal::FunctorFactoryPart6<base::DataSourceBase*, internal::DataSourceArgsMethod<S> >
        {
            template<class FInit>
            TemplateConstructor( FInit f, bool autom)
                : internal::FunctorFactoryPart6<base::DataSourceBase*, internal::DataSourceArgsMethod<S> >(f )
            {}

            virtual base::DataSourceBase::shared_ptr build(const std::vector<base::DataSourceBase::shared_ptr>& args) const {
                try {
                    return this->produce( args );
                } catch ( ... ) {
                }
                return base::DataSourceBase::shared_ptr();
            }

        };

    /**
     * Create a new Constructor.
     *
     * @param foo A pointer to the 'C' function which creates an object.
     * @param automatic Set to true to allow automatic conversion (without warning) to this type.
     * @return a Constructor object suitable for the type system.
     */
    template<class Function>
    TypeBuilder* newConstructor( Function* foo, bool automatic = false ) {
        return new detail::TemplateConstructor<Function, boost::function_traits<Function>::arity>(foo, automatic);
    }

    /**
     * Create a new Constructor.
     *
     * @param obj A function object which has operator().
     * @param automatic Set to true to allow automatic conversion (without warning) to this type.
     *
     * @return a Constructor object suitable for the type system.
     */
    template<class Object>
    TypeBuilder* newConstructor( Object obj, bool automatic = false) {
        return new detail::TemplateConstructor<typename Object::Signature, boost::function_traits<typename Object::Signature>::arity>(obj, automatic);
    }
}}

#endif
