/***************************************************************************
  tag: Peter Soetens  Mon Jan 19 14:11:26 CET 2004  DataServer.hpp 

                        DataServer.hpp -  description
                           -------------------
    begin                : Mon January 19 2004
    copyright            : (C) 2004 Peter Soetens
    email                : peter.soetens@mech.kuleuven.ac.be
 
 ***************************************************************************
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU Lesser General Public            *
 *   License as published by the Free Software Foundation; either          *
 *   version 2.1 of the License, or (at your option) any later version.    *
 *                                                                         *
 *   This library is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU     *
 *   Lesser General Public License for more details.                       *
 *                                                                         *
 *   You should have received a copy of the GNU Lesser General Public      *
 *   License along with this library; if not, write to the Free Software   *
 *   Foundation, Inc., 59 Temple Place,                                    *
 *   Suite 330, Boston, MA  02111-1307  USA                                *
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
#include <corelib/VectorComposition.hpp>
#include "Typelist.h"

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
        static ORO_CoreLib::NameServer< DataObjectInterface< typename _DataObjectType::DataType>* > ns;
        std::string prefix;

    public :
        typedef _DataObjectType DataObjectType;
        typedef _DataObjectType* DataObjectType_ptr;
        typedef DataObjectInterface< typename _DataObjectType::DataType>* NameServerType ;
        
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
        DataObjectServer(const std::string& _name, const std::string& _prefix )//= _name ) 
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
            typename NameServer< NameServerType >::name_iterator it1( ns.getNameBegin() );
            typename NameServer< NameServerType >::name_iterator it2( ns.getNameEnd() );

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
            typename NameServer< NameServerType >::name_iterator it1( ns.getNameBegin() );
            typename NameServer< NameServerType >::name_iterator it2( ns.getNameEnd() );

            // How to give a 'bag' name == prefix to the introspector ??
            for ( ; it1 != it2; ++it1)
                {
                    // MemberType is *not* a pointer !
                    // the object is copied into the Property, then introspected and then
                    // the property is destructed again.
                    // the name of the property is without prefix.
                    if ( (*it1).find( prefix ) == 0 )
                        {
                            MemberType val;
                            ns.getObject( *it1 )->Get(val);
//                             cout << "Decomposing "<< *it1<<endl;
                            introspector->introspect( Property<MemberType>( std::string( (*it1), prefix.length() ),
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
                    NameServerType dot;
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

//         template< class T>
//         bool has( const std::string& name, T t = T() ) const
//         {
//             return false;
//         }

        bool has( const std::string& name, MemberType m = MemberType() ) const
        {
            if ( ns.getObject(prefix + name) )
                return true;
            return false;
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
            NameServerType res;
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
        bool Get( const std::string& name, DataObjectInterface<MemberType>*& m ) const
        {
            NameServerType res;
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
            NameServerType res;
            if (( res = ns.getObject(prefix + name)) ) {
                res->Set(m);
                return true;
            }
            return false;
        }

        /**
         * @brief Register a DataObject with a certain name.
         * @param m The DataObject to add.
         */
        void reg( DataObjectInterface<MemberType>* m )
        {
            ns.registerObject(m, prefix + m->getName() );
        }

        /**
         * @brief Deregister a DataObject.
         * @param m The DataObject to remove
         */
        void deReg( DataObjectInterface<MemberType>* m )
        {
            ns.unregisterObject( m );
        }
    };

    template< class _DataObjectType >
    NameServer< DataObjectInterface<typename _DataObjectType::DataType>* > DataObjectServer<_DataObjectType>::ns;

    typedef Loki::NullType nil_type; // Moving to Loki.

//     template<class T> 
//     struct is_nil
//     {
//         static const bool value = false;
//         typedef T type;
//     };

//     template<>
//     struct is_nil<nil_type>
//     {
//         static const bool value = true;
//         typedef nil_type type;
//     };

//     template< class T> struct has_nil
//     {
//         static const bool value = false;
//         typedef T type;
//     };

//     template< template<class T> class V >
//     struct has_nil< V<nil_type> >
//     {
//         static const bool value = true;
//         typedef V<nil_type> type;
//     };


//     template<typename First, typename Rest=nil_type> // only last NameList element has Rest==nil_type
//     struct NameList
//     {
//         typedef First first;
//         typedef Rest rest;
//     };

    using Loki::Typelist; // moving to Loki.

    template<typename First > class NameSubClass;

    /**
     * Similar to CompositeExtension, it allows users
     * to compose data objects so that multiple components,
     * with different data objects, can be used in one kernel.
     */
    template< class OneDO, class OtherDO >
    class CompositeDataObject
    {
    public:
        typedef OneDO First;
        typedef OtherDO Second;

        /**
         * The Result contains all types of both Typelist.
         */
        typedef typename Loki::TL::Append<typename First::Result, typename Second::Result>::Result Result;
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
    struct NameSubClass< Typelist<First,Rest> >
        : public DataObjectServer<First>, public NameSubClass<Rest>
    {
        using DataObjectServer<First>::Get;
        using DataObjectServer<First>::Set;
        using DataObjectServer<First>::has;
        using DataObjectServer<First>::reg;
        using DataObjectServer<First>::deReg;

        using NameSubClass<Rest>::Get;
        using NameSubClass<Rest>::Set;
        using NameSubClass<Rest>::has;
        using NameSubClass<Rest>::reg;
        using NameSubClass<Rest>::deReg;

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
                            DataObjectServer<First>::reg( item );
                        }
                }
        }

        /**
         * The destructor cleans up all its DataObjectServer instances.
         */
        virtual ~NameSubClass() 
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

//         virtual void setName( const std::string& name )
//         {
//             DataObjectServer<First>::setName(name);
//             NameSubClass<Rest>::setName(name);
//         }

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
     */
    template<typename First>
    struct NameSubClass< Typelist< First, nil_type > >
        : public DataObjectServer<First> 
    {

        using DataObjectServer<First>::Get;
        using DataObjectServer<First>::Set;
        using DataObjectServer<First>::has;
        using DataObjectServer<First>::reg;
        using DataObjectServer<First>::deReg;

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
                            First*  item = new First( it->second );
                            fv.push_back( item );
                            DataObjectServer<First>::reg( item );
                        }
                }
        }

        virtual ~NameSubClass()
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
     * When a nil_type is given, there is no typelist in this case,
     * so we need to do some manual type faking...
     */
    template< template<class Cont> class First>
    struct NameSubClass< First<nil_type> >
            : public DataObjectServer< First<nil_type> > 
    {
        bool Get( First<nil_type>& ) {return false;}
        bool Set( First<nil_type>& ) {return false;}
        template< typename pair_type, typename index_type>
        NameSubClass(const std::string& name, const std::string& prefix, const pair_type& t, index_type index)
            : DataObjectServer<First<nil_type> >(name, prefix)
        {}
        void changePrefix(const std::string& prefix) { }
        bool has( const std::string& name, nil_type m = nil_type() ) const
        {
            return false;
        }
        void reg( First<nil_type>* ) {return ;}
        void deReg( First<nil_type>* ) {return ;}
            
        void refreshReports( PropertyBag& bag ) const {}
        void inspectReports( PropertyIntrospection* introspector ) const {}
        void exportReports( PropertyBag& bag ) const  {}
        void cleanupReports( PropertyBag& bag ) const {}
    };

    /**
     * NameSubClass specialisation.
     * When a plain nil_type is encountered on all but last template slot.
     */
//     template<typename First>
//     struct NameSubClass< Typelist< typename has_nil<First>::type, nil_type> >
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



    using namespace Loki::TL;

    /**
     * This class is almost the same as MakeTypelist from Loki. The copyright of this
     * code is in Typename.h.
     */
        template
        <
            typename T1  = nil_type, typename T2  = nil_type, typename T3  = nil_type,
            typename T4  = nil_type, typename T5  = nil_type, typename T6  = nil_type,
            typename T7  = nil_type, typename T8  = nil_type, typename T9  = nil_type,
            typename T10 = nil_type, typename T11 = nil_type, typename T12 = nil_type,
            typename T13 = nil_type, typename T14 = nil_type, typename T15 = nil_type,
            typename T16 = nil_type, typename T17 = nil_type, typename T18 = nil_type
        > 
        struct ServedTypes : public std::multimap<int, std::string>
        {
        private:
            typedef typename ServedTypes
            <
                T2 , T3 , T4 , 
                T5 , T6 , T7 , 
                T8 , T9 , T10, 
                T11, T12, T13,
                T14, T15, T16, 
                T17, T18
            >
            ::Result TailResult;

        public:
            typedef Typelist<T1, TailResult> Result;
        };

        template<>
        struct ServedTypes<> : public std::multimap<int, std::string>
        {
            typedef nil_type Result;
        };

    namespace detail
    {


        /**
         * @brief The templated nameserved dataobject. It can be of any DataType/DataObjectType.
         *
         * This class is the toplevel class that represents a dataobject of a given type
         * (simple, locked, priority_set,...) and which contains
         * multiple nameserved types.
         *
         * @param _NameContainer is one of the specialised containers for holding type info of
         * the objects to be served.
         */
        template<typename _NameContainer>
        class NameServedDataObject
            : public _NameContainer::NamesTypes,
              public _NameContainer::tree
    {
    public :
        typedef typename _NameContainer::NamesTypes NamesTypes;
        typedef typename _NameContainer::WrappedNamesTypes WrappedNamesTypes;

        using _NameContainer::tree::Get;
        using _NameContainer::tree::Set;
        using _NameContainer::tree::has;
        using _NameContainer::tree::reg;
        using _NameContainer::tree::deReg;

        /**
         * @brief Create a NameServedDataObject, with a prefixed name.
         *
         * This constructs the nameserver for data objects for use
         * within a kernel.
         * @param name The unique name of the DataObject.
         * @param prefix The scope of the nameserver, to avoid clashes with
         *        other nameservers (the user does not see this).
         */
        NameServedDataObject(const std::string& name, const std::string& prefix )// = name ) 
            :  _NameContainer::NamesTypes(),
              _NameContainer::tree(name, prefix, std::make_pair( this->begin(), this->end() ), 0 )
        {
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
            typedef typename _NameContainer::template DataObjectType< _DataT >::type type;
        };

        typedef nil_type DataType;
    };

        /**
         * @brief A class which serves as a DataObjectServer frontend/facade with CompositeDataObject.
         *
         * Since DataObjectServers are hierarchies nested in NameSubClasses,
         * this is the single point of acces for the underlying DataObjectServer methods.
         * If it was not here, the compiler would complain about ambiguity with method
         * calls. This could become the standard way of composing DataObjectServers 
         * when CompositeDataObject is not used.
         */
        template<class>
        struct NameFrontEnd;

        template<class Head, class Tail>
        struct NameFrontEnd< Typelist<Head, Tail> >
            : public DataObjectServer<Head>,
              public NameFrontEnd<Tail>
        {
            using DataObjectServer<Head>::Get;
            using DataObjectServer<Head>::Set;
            using DataObjectServer<Head>::has;
            using DataObjectServer<Head>::reg;
            using DataObjectServer<Head>::deReg;

            using NameFrontEnd<Tail>::Get;
            using NameFrontEnd<Tail>::Set;
            using NameFrontEnd<Tail>::has;
            using NameFrontEnd<Tail>::reg;
            using NameFrontEnd<Tail>::deReg;

            template< typename pair_type, typename index_type>
            NameFrontEnd( const std::string& name, const std::string& prefix, const pair_type& t, index_type index ) :
                DataObjectServer<Head>(name, prefix),
                NameFrontEnd<Tail>(name, prefix, t, index)
            {}

        };

        template<class Head>
        struct NameFrontEnd< Typelist<Head, nil_type> >
            : public DataObjectServer<Head>
        {
            using DataObjectServer<Head>::Get;
            using DataObjectServer<Head>::Set;
            using DataObjectServer<Head>::has;
            using DataObjectServer<Head>::reg;
            using DataObjectServer<Head>::deReg;
            template< typename pair_type, typename index_type>
            NameFrontEnd( const std::string& name, const std::string& prefix, const pair_type& t, index_type index) :
                DataObjectServer<Head>(name, prefix)
            {}
        };            
            
        // Container case
        template< template<class> class First>
        struct NameFrontEnd< First<nil_type> >
        {
            template< typename pair_type, typename index_type>
            NameFrontEnd( const std::string& name, const std::string& prefix, const pair_type& t, index_type index ) {}
            ~NameFrontEnd() {}
            bool Get( First<nil_type>& ) {return false;}
            bool Set( First<nil_type>& ) {return false;}
            void changePrefix(const std::string& prefix) { }
            bool has( const std::string& name, nil_type m = nil_type() ) const
            {
                return false;
            }
            void reg( First<nil_type>* ) {return ;}
            void deReg( First<nil_type>* ) {return ;}
            
            void refreshReports( PropertyBag& bag ) const {}
            void inspectReports( PropertyIntrospection* introspector ) const {}
            void exportReports( PropertyBag& bag ) const  {}
            void cleanupReports( PropertyBag& bag ) const {}
        };


        /**
         * @brief This specialisation detects the use of the CompositeDataObject
         * class for composing DataObjects.
         *
         * It delegates the two parts to
         * two NameServedDataObject subclasses. This allows recursive
         * Composition of DataObjects. This specialisation is needed to
         * remove the dupplicate type definitions when merging two Typelists
         * and to provide a 'FrontEnd' to the types.
         */
        template< template<class> class _NameContainer, class F, class S>
        struct NameServedDataObject< _NameContainer< CompositeDataObject<F,S> > >
            : public NameFrontEnd< typename Loki::TL::NoDuplicates< typename _NameContainer< CompositeDataObject<F,S> >::WrappedNamesTypes >::Result > // All Types get an Accessor method.
//               ,private NameServedDataObject< _NameContainer<F> >,
//               private NameServedDataObject< _NameContainer<S> >
        {
            NameServedDataObject< _NameContainer<F> > first_server;
            NameServedDataObject< _NameContainer<S> > second_server;

            typedef NameFrontEnd< typename Loki::TL::NoDuplicates< typename _NameContainer< CompositeDataObject<F,S> >::WrappedNamesTypes >::Result > FrontEnd;

            using FrontEnd::Get;
            using FrontEnd::Set;
            using FrontEnd::has;
            using FrontEnd::reg;
            using FrontEnd::deReg;
//             using NameServedDataObject< _NameContainer<F> >::Get;
//             using NameServedDataObject< _NameContainer<F> >::Set;

//             using NameServedDataObject< _NameContainer<S> >::Get;
//             using NameServedDataObject< _NameContainer<S> >::Set;

            NameServedDataObject(const std::string& name, const std::string& prefix ) //= name ) 
                : FrontEnd(name, prefix, 0, 0),
                  first_server(name, prefix),
                  second_server(name, prefix)
//                 : NameServedDataObject<_NameContainer<F> >(name, prefix),
//                   NameServedDataObject<_NameContainer<S> >(name, prefix)
            {}

            void changePrefix(const std::string& prefix)
            {
//                 NameServedDataObject< _NameContainer<F> >::changePrefix(prefix);
//                 NameServedDataObject< _NameContainer<S> >::changePrefix(prefix);
                first_server.changePrefix(prefix);
                second_server.changePrefix(prefix);
            }


//             virtual void setName( const std::string& name )
//             {
//                 first_server.setName(name);
//                 second_server.setName(name);
//             }

            virtual void refreshReports( PropertyBag& bag ) const
            {
                // Delegate to the subclass's implementation.
                first_server.refreshReports(bag);
                second_server.refreshReports(bag);
            }

            void exportReports( PropertyBag& bag ) const
            {
                // Delegate to the subclass's implementation.
                first_server.exportReports(bag);
                second_server.exportReports(bag);
            }

            virtual void inspectReports(PropertyIntrospection* i) const
            {
                // Delegate to the subclass's implementation.
                first_server.inspectReports(i);
                second_server.inspectReports(i);
            }

            virtual void cleanupReports( PropertyBag& bag ) const
            {
                first_server.cleanupReports(bag);
                second_server.cleanupReports(bag);
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
                typedef typename _NameContainer<F>::template DataObjectType< _DataT >::type type;
            };

            //typedef typename _NameContainer<F>::NamesTypes::DataType DataType;
            typedef nil_type DataType;
        };

        /**
         * A Typelist operation that wraps each type T
         * in _Typelist so that it becomes _Wrapper::Wrap<T>
         */
        template<class _Wrapper , class _Typelist >
        struct Wrap;

        template<class _Wrapper>
        struct Wrap<_Wrapper, nil_type>
        {
            typedef typename _Wrapper::template Wrap<nil_type>::Result Result;
        };

        template<class _Wrapper, class Head, class Tail>
        struct Wrap<_Wrapper, Typelist<Head, Tail> >
        {
            typedef Typelist< typename _Wrapper::template Wrap<Head>::Result, typename Wrap<_Wrapper,Tail>::Result > Result;
        };

        template<class _Wrapper, class Head >
        struct Wrap<_Wrapper, Typelist<Head, nil_type> >
        {
            typedef Typelist< typename _Wrapper::template Wrap<Head>::Result, nil_type > Result;
        };

        /**
         * @brief The containers below are used by the kernel to select the type of DataObject.
         */
        struct DataObjectC
        {
            /**
             * The template parameter is a user supplied Typelist derived type, which
             * conains the names and the types of each nameserved DataObject.
             *
             * @param C The class inheriting from ServedTypes and UnServedType.
             */
            template< typename C>
            struct DataObject
            {
                /**
                 * The List of all types.
                 */
                typedef C NamesTypes; 
                template< typename D>
                struct DataObjectType { typedef ORO_ControlKernel::DataObject<D> type; };

                /**
                 * This special construct wraps any type T in the
                 * Typelist NamesTypes with a DataObject.
                 */
                struct Wrapper
                {
                    template<class T>
                    struct Wrap
                    {
                        typedef ORO_ControlKernel::DataObject<T> Result;
                    };
                };

                typedef typename Wrap< Wrapper, typename NamesTypes::Result >::Result WrappedNamesTypes;
                typedef NameSubClass< typename Wrap< Wrapper, typename NamesTypes::Result >::Result > tree;
            };
        };

        struct DataObjectLockedC
        {
            template< typename C>
            struct DataObject
            {
                typedef C NamesTypes; 
                template< typename D>
                struct DataObjectType { typedef ORO_ControlKernel::DataObjectLocked<D> type; };
                struct Wrapper
                {
                    template<class T>
                    struct Wrap
                    {
                        typedef ORO_ControlKernel::DataObjectLocked<T> Result;
                    };
                };

                typedef typename Wrap< Wrapper, typename NamesTypes::Result >::Result WrappedNamesTypes;
                typedef NameSubClass<typename Wrap< Wrapper, typename NamesTypes::Result >::Result > tree;
            };
        };

        struct DataObjectPrioritySetC
        {
            template< typename C>
            struct DataObject
            {
                typedef C NamesTypes; 
                template< typename D>
                struct DataObjectType { typedef ORO_ControlKernel::DataObjectPrioritySet<D> type; };
                struct Wrapper
                {
                    template<class T>
                    struct Wrap
                    {
                        typedef ORO_ControlKernel::DataObjectPrioritySet<T> Result;
                    };
                };

                typedef typename Wrap< Wrapper, typename NamesTypes::Result >::Result WrappedNamesTypes;
                typedef NameSubClass<typename Wrap< Wrapper, typename NamesTypes::Result >::Result > tree;
            };
        };

        struct DataObjectPriorityGetC
        {
            template< typename C>
            struct DataObject
            {
                typedef C NamesTypes; 
                template< typename D>
                struct DataObjectType { typedef ORO_ControlKernel::DataObjectPriorityGet<D> type; };
                struct Wrapper
                {
                    template<class T>
                    struct Wrap
                    {
                        typedef ORO_ControlKernel::DataObjectPriorityGet<T> Result;
                    };
                };

                typedef typename Wrap< Wrapper, typename NamesTypes::Result >::Result WrappedNamesTypes;
                typedef NameSubClass<typename Wrap< Wrapper, typename NamesTypes::Result >::Result > tree;
            };
        };

        struct DataObjectBufferC
        {
            template< typename C>
            struct DataObject
            {
                typedef C NamesTypes; 
                template< typename D>
                struct DataObjectType { typedef ORO_ControlKernel::DataObjectBuffer<D> type; };
                struct Wrapper
                {
                    template<class T>
                    struct Wrap
                    {
                        typedef ORO_ControlKernel::DataObjectBuffer<T> Result;
                    };
                };

                typedef typename Wrap< Wrapper, typename NamesTypes::Result >::Result WrappedNamesTypes;
                typedef NameSubClass<typename Wrap< Wrapper,typename NamesTypes::Result >::Result > tree;
            };
        };

        struct DataObjectInterfaceC
        {
            template< typename C>
            struct DataObject
            {
                typedef C NamesTypes; 
                template< typename D>
                struct DataObjectType { typedef ORO_ControlKernel::DataObjectInterface<D> type; };
                struct Wrapper
                {
                    template<class T>
                    struct Wrap
                    {
                        typedef ORO_ControlKernel::DataObjectInterface<T> Result;
                    };
                };

                typedef typename Wrap< Wrapper, typename NamesTypes::Result >::Result WrappedNamesTypes;
                typedef NameFrontEnd<typename Wrap< Wrapper,typename NamesTypes::Result >::Result > tree;
            };
        };

        /**
         * @brief A helper class for nameskernels, which defines the type
         * for various kinds of dataobjects (locked, priority_set,...) 
         *  within nameservers.
         *
         * This is purely for aiding in shorter writing of some template
         * code. Its use is optional.
         *
         * @param DataNames The datatypes (TypeList) of the data exchanged.
         */
        template <class DataNames>
        struct NamesDOFactory
        {
            typedef NameServedDataObject< DataObjectC::DataObject< DataNames > > fast;
            typedef NameServedDataObject< DataObjectLockedC::DataObject< DataNames > > locked;
            typedef NameServedDataObject< DataObjectPrioritySetC::DataObject< DataNames > > priority_set;
            typedef NameServedDataObject< DataObjectPriorityGetC::DataObject< DataNames > > priority_get;
            typedef NameServedDataObject< DataObjectBufferC::DataObject< DataNames > > buffer;
            typedef NameServedDataObject< DataObjectInterfaceC::DataObject< DataNames > > interface;
        }; 


    } // namespace detail

}

#endif
