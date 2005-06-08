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

#ifdef ORO_PRAGMA_INTERFACE
ORO_PRAGMA_INTERFACE
#endif

#include <iterator>
#include <string>
#include <vector>
#include <map>
#include <utility>
#include <corelib/NameServer.hpp>
#include <boost/type_traits.hpp>
#include <corelib/DataObjectInterfaces.hpp>
#include "DataObjectReporting.hpp"
#include <corelib/PropertyIntrospection.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/mpl/list.hpp>

namespace ORO_ControlKernel
{
    namespace detail {
        /**
         * A place holder to terminate type lists.
         */
        typedef boost::mpl::na nil_type;
    }
}

namespace ORO_CoreLib
{
    // this is quite optional, since we will most likely never want to report
    // 'nothing'
    void decomposeProperty(ORO_CoreLib::PropertyIntrospection* pi, 
                           ORO_CoreLib::Property<ORO_ControlKernel::detail::nil_type> const& nt);

}

namespace ORO_ControlKernel
{
    using ORO_CoreLib::NameServer;
    using ORO_CoreLib::DataObject;
    using ORO_CoreLib::DataObjectLocked;
    using ORO_CoreLib::DataObjectPrioritySet;
    using ORO_CoreLib::DataObjectPriorityGet;
    using ORO_CoreLib::DataObjectLockFree;
    using ORO_CoreLib::DataObjectInterface;
    using ORO_CoreLib::CommandInterface;

    namespace detail {

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
        : public ReportingClient
    {
        static ORO_CoreLib::NameServer< typename DataObjectInterface< typename _DataObjectType::DataType>::shared_ptr > ns;
        std::string prefix;
        /**
         * A server where we reg our dataobjs for reporting.
         */
        boost::shared_ptr<DataObjectReporting> repserver;
    public :
        typedef _DataObjectType DataObjectType;
        typedef _DataObjectType* DataObjectType_ptr;
        typedef typename DataObjectInterface< typename _DataObjectType::DataType>::shared_ptr NameServerType ;
        
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
         * The _name must have the form : ControlKernelName::DOSName, for example MyKernel::Inputs
         * @param _prefix The 'scope'/prefix to automatically add to
         *        each lookup. This allows different DOS'es to work
         *        in the same namespace, although being in different kernels.
         */
        DataObjectServer(const std::string& _name, const std::string& _prefix )//= _name ) 
            : prefix( _prefix ), repserver( DataObjectReporting::nameserver.getObject( _name ) ) {
            if ( repserver.get() == 0 ) {
                new DataObjectReporting( _name );
                repserver = DataObjectReporting::nameserver.getObject( _name );
            }
            repserver->addClient( this );
        }

        virtual ~DataObjectServer() {
            repserver->removeClient( this );
        }

        virtual void reportAll(std::vector<CommandInterface*>& comms,  PropertyBag& bag ) const 
        {
            // Export all members of this server.
            // this is only done once on startup. The reporting extension
            // assumes they are updated with the comms Commands.
            //PropertyBagIntrospector inspector( bag );
            //this->inspectReports( &inspector );
            typename NameServer< NameServerType >::name_iterator it1( ns.getNameBegin() );
            typename NameServer< NameServerType >::name_iterator it2( ns.getNameEnd() );

            for ( ; it1 != it2; ++it1)
                {
                    // MemberType is *not* a pointer !
                    // the object is copied into the Property if the prefix is correct and it's not yet in the bag.
                    // because other servers try to copy it also into the bag.
                    //std::cerr <<"["<<prefix << "] Should I export : "<< (*it1);
                    if ( (*it1).find( prefix ) == 0 && bag.find( std::string( (*it1), prefix.length() ) ) == 0 )
                        {
                            //std::cerr << " yes."<<endl;
                            MemberType val;
                            ns.getObject( *it1 )->Get(val);
                            Property<MemberType>* item =  new Property<MemberType>( std::string( (*it1), prefix.length() ),
                                                                                    std::string( "" ), val );
                            comms.push_back( item->refreshCommand( ns.getObject( *it1 ).get() ));
                            bag.add( item );
                        } //else std::cerr << " no."<<endl;
                }
        }

        virtual std::pair<PropertyBase*,CommandInterface*> createItem( const std::string name ) const 
        {
            typename NameServer< NameServerType >::name_iterator it1( ns.getNameBegin() );
            typename NameServer< NameServerType >::name_iterator it2( ns.getNameEnd() );

            for ( ; it1 != it2; ++it1)
                {
                    if ( (*it1).find( prefix ) == 0 && name.find( std::string( (*it1), prefix.length() ) ) == 0 )
                        {
                            MemberType val;
                            ns.getObject( *it1 )->Get(val);
                            PropertyBase* item =  new Property<MemberType>( std::string( (*it1), prefix.length() ),
                                                                                    std::string( "" ), val );
                            // return the refresh command which updates orig with the DObj.
                            CommandInterface* c =  item->refreshCommand( ns.getObject( *it1 ).get() );
                            return std::make_pair( item, c );
                        }
                }
            return std::pair<PropertyBase*, CommandInterface*>(0,0);
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
         * @param m Returns the resulting DataObject (or null).
         * @param name The name (without prefix) of the DataObject
         * which must be Get.
         * @return true if it could be get, false otherwise.
         * @deprecated by shared_ptr version.
         */
        bool Get( const std::string& name, DataObjectInterface<MemberType>*& m ) const
        {
            NameServerType res;
            if ( (res = ns.getObject(prefix + name) ) )
                {
                    m = res.get();
                    return true;
                }
            m = 0;
            return false;
        }

        /**
         * @brief Get a pointer to a DataObject.
         *
         * @param m Returns the resulting DataObject (or null).
         * @param name The name (without prefix) of the DataObject
         * which must be Get.
         * @return true if it could be get, false otherwise.
         */
        bool Get( const std::string& name, typename DataObjectInterface<MemberType>::shared_ptr& m ) const
        {
            NameServerType res;
            if ( (res = ns.getObject(prefix + name) ) )
                {
                    m = res;
                    return true;
                }
            m = 0;
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
            if ( ns.registerObject(m, prefix + m->getName() ) == true )
                (*repserver)[m->getName()] = m;
        }

        /**
         * @brief Deregister a DataObject.
         * @param m The DataObject to remove
         */
        void deReg( DataObjectInterface<MemberType>* m )
        {
            ns.unregisterObject( m );
            repserver->erase( m->getName() );
        }
    };

    template< class _DataObjectType >
    NameServer< typename DataObjectInterface<typename _DataObjectType::DataType>::shared_ptr > DataObjectServer<_DataObjectType>::ns;

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


        template
        <
            typename T1  = nil_type, typename T2  = nil_type, typename T3  = nil_type,
            typename T4  = nil_type, typename T5  = nil_type, typename T6  = nil_type,
            typename T7  = nil_type, typename T8  = nil_type, typename T9  = nil_type,
            typename T10 = nil_type, typename T11 = nil_type, typename T12 = nil_type,
            typename T13 = nil_type, typename T14 = nil_type, typename T15 = nil_type,
            typename T16 = nil_type, typename T17 = nil_type, typename T18 = nil_type
        > 
        struct NameList
        {
        private:
            typedef typename NameList
            <
                T2 , T3 , T4 , 
                T5 , T6 , T7 , 
                T8 , T9 , T10, 
                T11, T12, T13,
                T14, T15, T16, 
                T17, T18
            >
            ::type tail_type;

        public:
            typedef NameList<T1, tail_type> type;
        };

        template<>
        struct NameList<>
        {
            typedef nil_type type;
        };


    template<typename First > class NameSubClass;

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
        : public DataObjectServer< DataObjectInterface< typename First::DataType> >,
          public NameSubClass<Rest>
    {
        typedef DataObjectServer< DataObjectInterface<typename First::DataType> > ServerType;
        using ServerType::Get;
        using ServerType::Set;
        using ServerType::has;
        using ServerType::reg;
        using ServerType::deReg;

        using NameSubClass<Rest>::Get;
        using NameSubClass<Rest>::Set;
        using NameSubClass<Rest>::has;
        using NameSubClass<Rest>::reg;
        using NameSubClass<Rest>::deReg;

        template< typename pair_type, typename index_type>
        NameSubClass(const std::string& name, const std::string& prefix, const pair_type& t, index_type index) 
            : ServerType(name,prefix), 
              NameSubClass<Rest>(name, prefix, t, index + 1 )
        {
            this->load(t, index);
        }

        virtual ~NameSubClass() 
        {
        }

        template< typename pair_type, typename index_type>
        void load(const pair_type& t, index_type index)
        {
            for ( typename pair_type::first_type it = t.first;
                 it != t.second;
                 ++it)
                {
                    // if the index is equal to our index
                    if ( it->first == index )
                        {
                            // Check for duplicate entries :
                            if ( ! ServerType::has( it->second ) ) {
                                First*  item = new First( it->second );
                                ServerType::reg( item ); // item now owned by server !
                            }
                        }
                }
        }

        template< typename pair_type, typename index_type>
        void recursiveReload(const pair_type& t, index_type index)
        {
            NameSubClass<Rest>::recursiveReload(t, index+1);
            load(t, index);
        }

        virtual void inspectReports(PropertyIntrospection* i) const
        {
            // Delegate to the subclass's implementation.
            ServerType::inspectReports(i);
            NameSubClass<Rest>::inspectReports(i);
        }

    };

    /**
     * This class holds one nameserved element of a DataObject.
     * Evenmore, it offers the DataObject Interface for a nameserved
     * element. 
     *
     */
    template<typename First>
    struct NameSubClass< NameList< First > >
        : public DataObjectServer< DataObjectInterface< typename First::DataType> > 
    {

        typedef DataObjectServer< DataObjectInterface<typename First::DataType> > ServerType;

        using ServerType::Get;
        using ServerType::Set;
        using ServerType::has;
        using ServerType::reg;
        using ServerType::deReg;

        template< typename pair_type, typename index_type>
        NameSubClass(const std::string& name, const std::string& prefix, const pair_type& t, index_type index) 
            : ServerType(name, prefix)
        {
            this->load(t, index);
        }

        virtual ~NameSubClass() 
        {
        }
        template< typename pair_type, typename index_type>
        void load(const pair_type& t, index_type index)
        {
            for ( typename pair_type::first_type it = t.first;
                 it != t.second;
                 ++it)
                {
                    // if the index is equal to our index
                    if ( it->first == index )
                        {
                            // Check for duplicate entries :
                            if ( ! ServerType::has( it->second ) ) {
                                First*  item = new First( it->second );
                                ServerType::reg( item ); // item now owned by server !
                            }
                        }
                }
        }

        template< typename pair_type, typename index_type>
        void recursiveReload(const pair_type& t, index_type index)
        {
            load(t, index);
        }
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

        bool has( const std::string& name, nil_type m = nil_type() ) const
        {
            return false;
        }
        void reg( First<nil_type>* ) {return ;}
        void deReg( First<nil_type>* ) {return ;}

        void inspectReports( PropertyIntrospection* introspector ) const {}

        template< typename pair_type, typename index_type>
        void load(const pair_type& t, index_type index) {}

        template< typename pair_type, typename index_type>
        void recursiveReload(const pair_type& t, index_type index) {}
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


    }

    /**
     * A common typelist implementation using the boost MPL.
     */
        template
        <
            typename T1  = detail::nil_type, typename T2  = detail::nil_type, typename T3  = detail::nil_type,
            typename T4  = detail::nil_type, typename T5  = detail::nil_type, typename T6  = detail::nil_type,
            typename T7  = detail::nil_type, typename T8  = detail::nil_type, typename T9  = detail::nil_type,
            typename T10 = detail::nil_type, typename T11 = detail::nil_type, typename T12 = detail::nil_type,
            typename T13 = detail::nil_type, typename T14 = detail::nil_type, typename T15 = detail::nil_type,
            typename T16 = detail::nil_type, typename T17 = detail::nil_type, typename T18 = detail::nil_type
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
            ::type tail_type;

        public:
            typedef detail::NameList<T1, tail_type> type;
        };

        template<>
        struct ServedTypes<> : public std::multimap<int, std::string>
        {
            typedef detail::nil_type type;
        };

    namespace detail
    {

        /** 
         * This class is the non templated base of a NameServedDataObject.
         */
        struct NameServedDataObjectBase {
            virtual ~NameServedDataObjectBase() {}
        };

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
            : public NameServedDataObjectBase,
              public _NameContainer::NamesTypes,
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
         * @param load Set to false if no DataObject creation should happen yet.
         *
         * @todo Create the NameServedDataObjectInterface
         */
        NameServedDataObject(const std::string& name, const std::string& prefix, bool load = true )
            :  _NameContainer::NamesTypes(),
              _NameContainer::tree(name, prefix, std::make_pair( this->begin(), load ? this->end() : this->begin() ), 0 )
        {
        }

        /**
         * @brief Instruct the nameserver to reload all DataObjects.
         * @param  b Specify an iterator range containing the
         * the 'build recipe' for which types to build.
         */
        template< class Iter >
        void reload(const Iter&  b, const Iter& e ) {
            // A reload is non destructive, it only adds new entries.
            this->_NameContainer::tree::recursiveReload( std::make_pair( b, e ), 0 );
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
         * @brief A class which serves as a DataObjectServer frontend/facade.
         *
         * Since DataObjectServers are hierarchies nested in NameSubClasses,
         * this is the single point of acces for the underlying DataObjectServer methods.
         * If it was not here, the compiler would complain about ambiguity with method
         * calls. This is the standard way of getting access to DataObjectServers .
         * 
         */
        template<class>
        struct NameFrontEnd;

        template<class Head, class Tail>
        struct NameFrontEnd< NameList<Head, Tail> >
            : public DataObjectServer< DataObjectInterface<typename Head::DataType> >,
              public NameFrontEnd<Tail>
        {
            typedef DataObjectServer< DataObjectInterface< typename Head::DataType> > ServerType;

            using ServerType::Get;
            using ServerType::Set;
            using ServerType::has;
            using ServerType::reg;
            using ServerType::deReg;

            using NameFrontEnd<Tail>::Get;
            using NameFrontEnd<Tail>::Set;
            using NameFrontEnd<Tail>::has;
            using NameFrontEnd<Tail>::reg;
            using NameFrontEnd<Tail>::deReg;

            template< typename pair_type, typename index_type>
            NameFrontEnd( const std::string& name, const std::string& prefix, const pair_type& t, index_type index ) :
                ServerType(name, prefix),
                NameFrontEnd<Tail>(name, prefix, t, index)
            {}

        virtual void inspectReports(PropertyIntrospection* i) const
        {
            // Delegate to the subclass's implementation.
            ServerType::inspectReports(i);
            NameFrontEnd<Tail>::inspectReports(i);
        }

        };

        template<class Head>
        struct NameFrontEnd< NameList<Head, nil_type> >
            : public DataObjectServer< DataObjectInterface< typename Head::DataType> >
        {
            typedef DataObjectServer< DataObjectInterface< typename Head::DataType> > ServerType;

            using ServerType::Get;
            using ServerType::Set;
            using ServerType::has;
            using ServerType::reg;
            using ServerType::deReg;
            template< typename pair_type, typename index_type>
            NameFrontEnd( const std::string& name, const std::string& prefix, const pair_type& t, index_type index) :
                ServerType(name, prefix)
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
            void inspectReports( PropertyIntrospection* introspector ) const {}
        };

        /**
         * A NameList operation that wraps each type T
         * in _list so that it becomes _Wrapper::Wrap<T>
         */
        template<class _Wrapper , class _list >
        struct Wrap;

        template<class _Wrapper>
        struct Wrap<_Wrapper, nil_type>
        {
            typedef typename _Wrapper::template Wrap<nil_type>::type type;
        };

        template<class _Wrapper, class Head, class Tail>
        struct Wrap<_Wrapper, NameList<Head, Tail> >
        {
            typedef NameList< typename _Wrapper::template Wrap<Head>::type, typename Wrap<_Wrapper,Tail>::type > type;
        };

        template<class _Wrapper, class Head >
        struct Wrap<_Wrapper, NameList<Head, nil_type> >
        {
            typedef NameList< typename _Wrapper::template Wrap<Head>::type, nil_type > type;
        };

        /**
         * @brief The containers below are used by the kernel to select the type of DataObject.
         */
        struct DataObjectC
        {
            /**
             * The template parameter is a user supplied NameList derived type, which
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
                 * NameList NamesTypes with a DataObject.
                 */
                struct Wrapper
                {
                    template<class T>
                    struct Wrap
                    {
                        typedef ORO_ControlKernel::DataObject<T> type;
                    };
                };

                typedef typename Wrap< Wrapper, typename NamesTypes::type >::type WrappedNamesTypes;
                typedef NameSubClass< typename Wrap< Wrapper, typename NamesTypes::type >::type > tree;
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
                        typedef ORO_ControlKernel::DataObjectLocked<T> type;
                    };
                };

                typedef typename Wrap< Wrapper, typename NamesTypes::type >::type WrappedNamesTypes;
                typedef NameSubClass<typename Wrap< Wrapper, typename NamesTypes::type >::type > tree;
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
                        typedef ORO_ControlKernel::DataObjectPrioritySet<T> type;
                    };
                };

                typedef typename Wrap< Wrapper, typename NamesTypes::type >::type WrappedNamesTypes;
                typedef NameSubClass<typename Wrap< Wrapper, typename NamesTypes::type >::type > tree;
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
                        typedef ORO_ControlKernel::DataObjectPriorityGet<T> type;
                    };
                };

                typedef typename Wrap< Wrapper, typename NamesTypes::type >::type WrappedNamesTypes;
                typedef NameSubClass<typename Wrap< Wrapper, typename NamesTypes::type >::type > tree;
            };
        };

        struct DataObjectLockFreeC
        {
            template< typename C>
            struct DataObject
            {
                typedef C NamesTypes; 
                template< typename D>
                struct DataObjectType { typedef ORO_ControlKernel::DataObjectLockFree<D> type; };
                struct Wrapper
                {
                    template<class T>
                    struct Wrap
                    {
                        typedef ORO_ControlKernel::DataObjectLockFree<T> type;
                    };
                };

                typedef typename Wrap< Wrapper, typename NamesTypes::type >::type WrappedNamesTypes;
                typedef NameSubClass<typename Wrap< Wrapper,typename NamesTypes::type >::type > tree;
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
                        typedef ORO_ControlKernel::DataObjectInterface<T> type;
                    };
                };

                typedef typename Wrap< Wrapper, typename NamesTypes::type >::type WrappedNamesTypes;
                typedef NameFrontEnd<typename Wrap< Wrapper,typename NamesTypes::type >::type > tree;
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
         * @param DataNames The datatypes (NameList) of the data exchanged.
         */
        template <class DataNames>
        struct NamesDOFactory
        {
            typedef NameServedDataObject< DataObjectC::DataObject< DataNames > > fast;
            typedef NameServedDataObject< DataObjectLockedC::DataObject< DataNames > > locked;
            typedef NameServedDataObject< DataObjectPrioritySetC::DataObject< DataNames > > priority_set;
            typedef NameServedDataObject< DataObjectPriorityGetC::DataObject< DataNames > > priority_get;
            typedef NameServedDataObject< DataObjectLockFreeC::DataObject< DataNames > > lockfree;
            typedef NameServedDataObject< DataObjectInterfaceC::DataObject< DataNames > > interface;
        }; 


    } // namespace detail

    struct NoSetPoint : public ServedTypes<> {};
    struct NoInput : public ServedTypes<> {};
    struct NoOutput : public ServedTypes<> {};
    struct NoCommand : public ServedTypes<> {};
    struct NoModel : public ServedTypes<> {};

}

#endif
