/***************************************************************************
  tag: Peter Soetens  Mon Jan 19 14:11:26 CET 2004  DataServer.hpp 

                        DataServer.hpp -  description
                           -------------------
    begin                : Mon January 19 2004
    copyright            : (C) 2004 Peter Soetens
    email                : peter.soetens@mech.kuleuven.ac.be
 
 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
 
 
#ifndef DATASERVER_HPP
#define DATASERVER_HPP

/**
 * @file DataServer.hpp
 *
 * @brief Infrastructure for NameServed DataObjects.
 *
 * The code in this file is hard to understand for non template-experts.
 * You will need to understand templates and the libraries presented
 * on www.boost.org ( more specifically type_traits and mpl ). This 
 * file uses template meta-programming to construct classes at compile time,
 * on the basis of a template list given by the user. By giving semantical
 * properties to each element of the given list, the wishes of the user can be derived and a
 * class can be constructed conforming to it.
 */


#include <iterator>
#include <string>
#include <vector>
#include <map>
#include <utility>
#include <corelib/NameServer.hpp>
#include <boost/type_traits.hpp>
#include "DataObjectInterfaces.hpp"
#include "DataObjectReporting.hpp"

namespace ORO_ControlKernel
{

    /**
     * @brief An Interface for registering DataObject members 
     * into a public namespace. One nameserver will be
     * created for each _DataObjectType.
     *
     * It is intended to be a class using a 'normal'
     * DataObject, so that the DataObject gets a
     * NameServer. The DataObjectServer can do
     * standard prefixing of object names, so that an
     * automatic scope is added for each acces through this
     * DataObjectServer instance.
     *
     * @param _DataObjectType The Type of the DataObject.
     * For example DataObject< Geometry::Frame >
     */
    template< class _DataObjectType >
    class DataObjectServer
        : public DataObjectReporting
    {
        static ORO_CoreLib::NameServer< _DataObjectType* > ns;
        std::string prefix;

    public :
        typedef _DataObjectType DataObjectType;
        typedef _DataObjectType* DataObjectType_ptr;
        
        /**
         * The MemberType is the DataType of the DataObject.
         */
        typedef typename _DataObjectType::DataType MemberType;
        typedef MemberType* MemberType_ptr;

        /**
         * Create a 'stub' like object which grants access
         * to the global DataObject NameServer. This 'stub' can be given
         * a prefix to add a scope-like qualifier to the name, thus
         * avoiding name clashes. The use of the prefix is fully
         * transparent and does not need to be known to the user.
         *
         * @param _name The unique name of this DataObjectServer.
         * The _name must have the form : ControlKernelName::DOSName
         * @param _prefix The 'scope'/prefix to automatically add to
         *        each lookup. This allows different DOS'es to work
         *        in the same namespace, although being in different kernels.
         */
        DataObjectServer(const std::string& _name, const std::string& _prefix = _name ) 
            : DataObjectReporting( _name ), prefix( _prefix ) {}

        virtual ~DataObjectServer() {}

        /**
         * @brief Change the prefix of this server.
         *
         * @param _prefix The new prefix.
         */
        virtual void changePrefix(const std::string& _prefix) { prefix = _prefix; }

        virtual void exportReports( PropertyBag& bag ) const
        {
            // this is only done once on startup. The reporting extension
            // assumes they are updated with the updateReports() method.
            //PropertyBagIntrospector inspector( bag );
            //this->inspectReports( &inspector );
            typename NameServer< _DataObjectType* >::name_iterator it1( ns.getNameBegin() );
            typename NameServer< _DataObjectType* >::name_iterator it2( ns.getNameEnd() );

            for ( ; it1 != it2; ++it1)
                {
                    // MemberType is *not* a pointer !
                    // the object is copied into the Property
                    if ( (*it1).find( prefix ) == 0 )
                        {
                            MemberType val;
                            ns.getObject( *it1 )->Get(val);
                            bag.add( new Property<MemberType>( std::string( (*it1), prefix.length() ),
                                                               std::string( "" ), val ) );
                        }
                }
        }

        /**
         * Sends all data in this server stub to an introspector.
         */
        virtual void inspectReports( PropertyIntrospection* introspector ) const
        {
            // iterate over all ns elements, having 'prefix'
            // add them to the bag.
            // maybe it is better to use the 'native' container
            // iterators instead of the name_iterators
            typename NameServer< _DataObjectType* >::name_iterator it1( ns.getNameBegin() );
            typename NameServer< _DataObjectType* >::name_iterator it2( ns.getNameEnd() );

            // How to give a 'bag' name == prefix to the introspector ??
            for ( ; it1 != it2; ++it1)
                {
                    // MemberType is *not* a pointer !
                    // the object is copied into the Property, then decomposed and then
                    // the property is destructed again.
                    // the name of the property is without prefix.
                    if ( (*it1).find( prefix ) == 0 )
                        {
                            MemberType val;
                            ns.getObject( *it1 )->Get(val);
                            decomposeProperty( introspector,
                                               Property<MemberType>( std::string( (*it1), prefix.length() ),
                                                                     std::string( "" ), val ) );
                        }
                }
        }

        /**
         * Update the all elements in the given bag with new reports
         * of data in this server.
         */
        void refreshReports( PropertyBag& bag ) const
        {
            // The bag is filled with placeholders, ready to get the
            // updates.
            // try to update all ns elements having 'prefix'
            PropertyBag::iterator it1(bag.getProperties().begin()), it2(bag.getProperties().end() );
            for ( ; it1 != it2; ++it1 )
                {
                    // @todo : We could automagically store every data element
                    // in a property object. This would eliminate two copy operations !
                    // from DO to t below and from t to the Property.
                    // But since data is already stored in a DataObject, which controls access...
                    // it is hard to get around it. A Refval Property could eliminate
                    // one copy operation.
                    DataObjectType* dot;
                    if ( 0 != ( dot = ns.getObject( prefix + (*it1)->getName() ) ) )
                        {
                            MemberType t;
                            dot->Get(t);
                            // we are hosting this element.
                            // try to update the bag !
                            refreshProperty( bag,
                                             Property<MemberType>( (*it1)->getName(),
                                                                   "", t ) );
                        }
                }
        }

        virtual void cleanupReports( PropertyBag& bag ) const
        {
            flattenPropertyBag( bag );
            deleteProperties( bag );
        }
            
        /**
         * Get a member variable of a DataObject.
         * @param m Returns the resulting contents of a DataObject.
         * @param name The name (without prefix) of the DataObject
         * which must be Get.
         * @return true if it could be get, false otherwise.
         */
        bool Get(const std::string& name, MemberType& m) const
        {
            DataObjectType_ptr res;
            if ( (res = ns.getObject(prefix + name) ) )
                {
                    res->Get(m);
                    return true;
                }
            return false;
        }

        /**
         * @brief Get a pointer to a DataObject.
         *
         * @param m Returns the resulting DataObject.
         * @param name The name (without prefix) of the DataObject
         * which must be Get.
         * @return true if it could be get, false otherwise.
         */
        bool Get( const std::string& name, DataObjectType*& m ) const
        {
            DataObjectType_ptr res;
            if ( (res = ns.getObject(prefix + name) ) )
                {
                    m = res;
                    return true;
                }
            return false;
        }

        /**
         * @brief Set a member variable of a DataObject.
         * @param m The new value for the DataObject contents.
         * @param name The name (without prefix) of the DataObject
         * which must be Set.
         * @return true if it could be set, false otherwise.
         */
        bool Set( const std::string& name, const MemberType& m)
        {
            DataObjectType_ptr res;
            if (( res = ns.getObject(prefix + name)) ) {
                res->Set(m);
                return true;
            }
            return false;
        }

        /**
         * @brief Register a DataObject with a certain name.
         * @param m The DataObject to add.
         * @param name The name (without prefix) it should have.
         */
        void reg( DataObjectType_ptr m, const std::string& name)
        {
            ns.registerObject(m, prefix + name);
        }

        /**
         * @brief Deregister a DataObject.
         * @param m The DataObject to remove
         */
        void deReg( DataObjectType_ptr m )
        {
            ns.unregisterObject( m );
        }
    };

    template< class _DataObjectType >
    NameServer<_DataObjectType* > DataObjectServer<_DataObjectType>::ns;


    struct nil_type {};

    template<class T> 
    struct is_nil
    {
        static const bool value = false;
        typedef T type;
    };

    template<>
    struct is_nil<nil_type>
    {
        static const bool value = true;
        typedef nil_type type;
    };

    template< class T> struct has_nil
    {
        static const bool value = false;
        typedef T type;
    };

    template< template<class T> class V >
    struct has_nil< V<nil_type> >
    {
        static const bool value = true;
        typedef V<nil_type> type;
    };

    template<typename First, typename Rest=nil_type> // only last NameList element has Rest==nil_type
    struct NameList
    {
        typedef First first;
        typedef Rest rest;
    };

    template<typename First > class NameSubClass;

    template< typename T >
    struct MemberFromBase
    {
        MemberFromBase() : T() {}
        MemberFromBase( const T& _t ) : mfb(_t) {}
        T mfb;
    };

    /**
     * This class holds one nameserved element of a DataObject.
     * Evenmore, it offers the DataObject Interface for a nameserved
     * element.
     *
     * It forms a list of subclasses each holding their DataObject
     * in a DataObjectServer.
     */
    template<typename First, typename Rest>
    struct NameSubClass< NameList<First,Rest> >
        : public DataObjectServer<First>, public NameSubClass<Rest>
    {
        using DataObjectServer<First>::Get;
        using DataObjectServer<First>::Set;

        using NameSubClass<Rest>::Get;
        using NameSubClass<Rest>::Set;

        template< typename pair_type, typename index_type>
        NameSubClass(const std::string& name, const std::string& prefix, const pair_type& t, index_type index) 
            : DataObjectServer<First>(name,prefix), 
              NameSubClass<Rest>(name, prefix, t, index + 1 )
        {
            // t contains start and end iterator of the multimap
            for ( typename pair_type::first_type it = t.first;
                 it != t.second;
                 ++it)
                {
                    // if the index is equal to our index
                    if ( it->first == index )
                        {
                            First*  item = new First( it->second );
                            fv.push_back( item );
                            DataObjectServer<First>::reg( item, it->second );
                        }
                }
        }

        /**
         * The destructor cleans up all its DataObjectServer instances.
         */
        ~NameSubClass() 
        { 
            for ( typename std::vector<First*>::iterator it = fv.begin(); it != fv.end(); ++it)
                {
                    DataObjectServer<First>::deReg( *it ); 
                    delete *it;
                }
        }

        /**
         * Change the local name prefix.
         */
        void changePrefix(const std::string& prefix)
        {
            DataObjectServer<First>::changePrefix(prefix);
            NameSubClass<Rest>::changePrefix(prefix);
        }

        virtual void setName( const std::string& name )
        {
            DataObjectServer<First>::setName(name);
            NameSubClass<Rest>::setName(name);
        }

        virtual void refreshReports( PropertyBag& bag ) const
        {
            // Delegate to the subclass's implementation.
            DataObjectServer<First>::refreshReports(bag);
            NameSubClass<Rest>::refreshReports(bag);
        }

        void exportReports( PropertyBag& bag ) const
        {
            // Delegate to the subclass's implementation.
            DataObjectServer<First>::exportReports(bag);
            NameSubClass<Rest>::exportReports(bag);
        }

        virtual void inspectReports(PropertyIntrospection* i) const
        {
            // Delegate to the subclass's implementation.
            DataObjectServer<First>::inspectReports(i);
            NameSubClass<Rest>::inspectReports(i);
        }

        virtual void cleanupReports( PropertyBag& bag ) const
        {
            DataObjectServer<First>::cleanupReports(bag);
            NameSubClass<Rest>::cleanupReports(bag);
        }

    private:
        /**
         * The nameserved DataObject.
         */
        std::vector< First* > fv;
    };

    /**
     * This class holds one nameserved element of a DataObject.
     * Evenmore, it offers the DataObject Interface for a nameserved
     * element.
     *
     * @note This is only used when only the last element is nil_type.
     */
    template<typename First>
    struct NameSubClass< NameList< First, nil_type > >
        : public DataObjectServer<First> 
    {

        using DataObjectServer<First>::Get;
        using DataObjectServer<First>::Set;

        typedef DataObjectServer<First>    Server;
        typedef DataObjectServer<nil_type> NextServer;

        template< typename pair_type, typename index_type>
        NameSubClass(const std::string& name, const std::string& prefix, const pair_type& t, index_type index) 
            : DataObjectServer<First>(name, prefix)
        {
            // t contains start and end iterator of the multimap
            for ( typename pair_type::first_type it = t.first;
                 it != t.second;
                 ++it)
                {
                    // if the index is equal to our index
                    if ( it->first == index )
                        {
                            First*  item = new First();
                            fv.push_back( item );
                            DataObjectServer<First>::reg( item, it->second );
                        }
                }
        }

        ~NameSubClass()
        { 
            for ( typename std::vector<First*>::iterator it = fv.begin(); it != fv.end(); ++it)
                {
                    DataObjectServer<First>::deReg( *it ); 
                    delete *it;
                }
        }
    private:
        std::vector< First* > fv;
    };

    /**
     * NameSubClass specialisation.
     * When a nil_type is encountered in a container on all but last template slot.
     */
    template< template<class Cont> class First, typename Rest>
    struct NameSubClass< NameList< First<nil_type>, Rest > >
    {
        typedef DataObjectServer<nil_type> Server;
        typedef DataObjectServer<nil_type> NextServer;

        bool Get( First<nil_type>& ) {}
        bool Set( First<nil_type>& ) {}
        template< typename pair_type, typename index_type>
        NameSubClass(const std::string& name, const std::string& prefix, const pair_type& t, index_type index) {}
        void changePrefix(const std::string& prefix) { }
        void setName(const std::string& prefix) { }
        virtual void refreshReports( PropertyBag& bag ) const {}
        virtual void inspectReports( PropertyIntrospection* introspector ) const {}
        virtual void exportReports( PropertyBag& bag ) const  {}
        virtual void cleanupReports( PropertyBag& bag ) const {}
    };

    /**
     * NameSubClass specialisation.
     * When a plain nil_type is encountered on all but last template slot.
     */
//     template<typename First>
//     struct NameSubClass< NameList< typename has_nil<First>::type, nil_type> >
//     {
//         typedef DataObjectServer<nil_type> Server;
//         typedef DataObjectServer<nil_type> NextServer;

//         bool Get( First& ) {}
//         bool Set( First& ) {}
//         template< typename pair_type, typename index_type>
//         NameSubClass(const std::string& prefix, const pair_type& t, index_type index) {}
//         void changePrefix(const std::string& prefix) { }
// //         DataObjectServer<nil_type>* nextServer()
// //         {
// //             return 0;
// //         }
//     };

    template< typename _T0= nil_type,
              typename _T1= nil_type,
              typename _T2= nil_type,
              typename _T3= nil_type,
              typename _T4= nil_type,
              typename _T5= nil_type,
              typename _T6= nil_type,
              typename _T7= nil_type,
              typename _T8= nil_type,
              typename _T9= nil_type>
    struct ServedTypes : public std::multimap<int, std::string>
    {
        typedef _T0 T0;
        typedef _T1 T1;
        typedef _T2 T2;
        typedef _T3 T3;
        typedef _T4 T4;
        typedef _T5 T5;
        typedef _T6 T6;
        typedef _T7 T7;
        typedef _T8 T8;
        typedef _T9 T9;
    };

    template<typename _DataType = nil_type>
    struct UnServedType
    {
        typedef _DataType DataType;
    };

    namespace detail
    {

    /**
     * @brief The containers below are used by the kernel to select the type of DataObject.
     *
     * The template parameter is a user supplied ServedTypes derived type, which
     * conains the names and the types of each nameserved DataObject.
     *
     * Should these definitions remain in this file or move to another file ?
     */
    template< typename C>
    struct DataObjectContainer
    {
        typedef C NamesTypes; 
        template< typename D>
        struct DataObjectType { typedef DataObject<D> type; };
        typedef NameSubClass< NameList<DataObject<typename C::T0>, NameList<DataObject<typename C::T1>, NameList<DataObject<typename C::T2>, NameList<DataObject<typename C::T3>, NameList<DataObject<typename C::T4>,NameList<DataObject<typename C::T5>,NameList<DataObject<typename C::T6>, NameList<DataObject<typename C::T7>, NameList<DataObject<typename C::T8>, NameList< DataObject<typename C::T9> > > > > > > > > > > > tree;
    };

    template< typename C>
    struct DataObjectLockedContainer
    {
        typedef C NamesTypes; 
        template< typename D>
        struct DataObjectType { typedef DataObjectLocked<D> type; };
        typedef NameSubClass< NameList<DataObjectLocked<typename C::T0>, NameList<DataObjectLocked<typename C::T1>, NameList<DataObjectLocked<typename C::T2>, NameList<DataObjectLocked<typename C::T3>, NameList<DataObjectLocked<typename C::T4>,NameList<DataObjectLocked<typename C::T5>,NameList<DataObjectLocked<typename C::T6>, NameList<DataObjectLocked<typename C::T7>, NameList<DataObjectLocked<typename C::T8>, NameList< DataObjectLocked<typename C::T9> > > > > > > > > > > > tree;
    };

    template< typename C>
    struct DataObjectPrioritySetContainer
    {
        typedef C NamesTypes; 
        template< typename D>
        struct DataObjectType { typedef DataObjectPrioritySet<D> type; };
        typedef NameSubClass< NameList<DataObjectPrioritySet<typename C::T0>, NameList<DataObjectPrioritySet<typename C::T1>, NameList<DataObjectPrioritySet<typename C::T2>, NameList<DataObjectPrioritySet<typename C::T3>, NameList<DataObjectPrioritySet<typename C::T4>,NameList<DataObjectPrioritySet<typename C::T5>,NameList<DataObjectPrioritySet<typename C::T6>, NameList<DataObjectPrioritySet<typename C::T7>, NameList<DataObjectPrioritySet<typename C::T8>, NameList< DataObjectPrioritySet<typename C::T9> > > > > > > > > > > > tree;
    };

    template< typename C>
    struct DataObjectPriorityGetContainer
    {
        typedef C NamesTypes; 
        template< typename D>
        struct DataObjectType { typedef DataObjectPriorityGet<D> type; };
        typedef NameSubClass< NameList<DataObjectPriorityGet<typename C::T0>, NameList<DataObjectPriorityGet<typename C::T1>, NameList<DataObjectPriorityGet<typename C::T2>, NameList<DataObjectPriorityGet<typename C::T3>, NameList<DataObjectPriorityGet<typename C::T4>,NameList<DataObjectPriorityGet<typename C::T5>,NameList<DataObjectPriorityGet<typename C::T6>, NameList<DataObjectPriorityGet<typename C::T7>, NameList<DataObjectPriorityGet<typename C::T8>, NameList< DataObjectPriorityGet<typename C::T9> > > > > > > > > > > > tree;
    };

    template< typename C>
    struct DataObjectBufferContainer
    {
        typedef C NamesTypes; 
        template< typename D>
        struct DataObjectType { typedef DataObjectBuffer<D> type; };
        typedef NameSubClass< NameList<DataObjectBuffer<typename C::T0>, NameList<DataObjectBuffer<typename C::T1>, NameList<DataObjectBuffer<typename C::T2>, NameList<DataObjectBuffer<typename C::T3>, NameList<DataObjectBuffer<typename C::T4>,NameList<DataObjectBuffer<typename C::T5>,NameList<DataObjectBuffer<typename C::T6>, NameList<DataObjectBuffer<typename C::T7>, NameList<DataObjectBuffer<typename C::T8>, NameList< DataObjectBuffer<typename C::T9> > > > > > > > > > > > tree;
    };

        /**
         * @brief The templated nameserved dataobject. It can be of any DataType/DataObjectType.
         *
         * This class is the toplevel class that represents a dataobject of a given type
         * (simple, locked, priority_set,...) and which contains one not nameserved type and
         * multiple nameserved types.
         *
         * NameContainer is one of the specialised containers for holding type info of
         * the objects to be served.
         */
        template<typename _NameContainer>
        class NameServedDataObject
            : public _NameContainer::NamesTypes, 
          public _NameContainer::tree, 
          public _NameContainer::DataObjectType< typename _NameContainer::NamesTypes::DataType>::type // To provide the generic DataObjectInterface
    {
    public :
        using _NameContainer::tree::Get;
        using _NameContainer::tree::Set;
        typedef typename _NameContainer::DataObjectType< typename _NameContainer::NamesTypes::DataType>::type DefaultDataObject;
        using DefaultDataObject::Get;
        using DefaultDataObject::Set;

        /**
         * @brief Create a NameServedDataObject, with a prefixed name.
         *
         * This constructs the nameserver for data objects for use
         * within a kernel.
         * @param name The unique name of the DataObject.
         * @param prefix The scope of the nameserver, to avoid clashes with
         *        other nameservers (the user does not see this).
         */
        NameServedDataObject(const std::string& name, const std::string& prefix = name ) 
            : _NameContainer::NamesTypes(), 
            _NameContainer::tree(name, prefix, std::make_pair( begin(), end() ), 0 ),
            DefaultDataObject( name, prefix )
        {
        }

        void setName( const std::string& name)
        {
            _NameContainer::tree::setName(name);
        }

        void changePrefix(const std::string& prefix)
        {
            _NameContainer::tree::changePrefix(prefix);
        }

        /**
         * @brief This allows the user to retrieve a DataObject<T> of type T.
         *
         * He needs this to retrieve the DataObject type itself from the NameServer 
         * (instead of its contents or the type of its contents). It is a helper
         * class.
         */
        template< typename _DataT >
        struct DataObject
        {
            typedef typename _NameContainer::DataObjectType< _DataT >::type type;
        };

        typedef typename _NameContainer::NamesTypes::DataType DataType;

    };


        /**
         * @brief A helper class for nameskernels, which defines the type
         * for various kinds of dataobjects (locked, priority_set,...) 
         *  within nameservers.
         *
         * This is purely for aiding in shorter writing of some template
         * code. Its use is optional.
         *
         * @param DataNames The datatypes of the data exchanged.
         */
        template <class DataNames>
        struct NamesDOFactory
        {
            typedef NameServedDataObject< DataObjectContainer< DataNames > > fast;
            typedef NameServedDataObject< DataObjectLockedContainer< DataNames > > locked;
            typedef NameServedDataObject< DataObjectPrioritySetContainer< DataNames > > priority_set;
            typedef NameServedDataObject< DataObjectPriorityGetContainer< DataNames > > priority_get;
            typedef NameServedDataObject< DataObjectBufferContainer< DataNames > > buffer;
        }; 


    } // namespace detail

}

#endif
