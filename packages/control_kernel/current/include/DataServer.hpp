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

namespace ORO_ControlKernel
{
    /**
     * An Interface for registering DataObject members 
     * into a public namespace. One nameserver will be
     * created for each _MemberType.
     * It is intended to be a base class of a 'normal'
     * DataObject, so that the DataObject offers a
     * NameServer interface. The DataObjectServer can do
     * standard prefixing of object names, so that an
     * automatic scope is added for each acces through this
     * DataObjectServer instance.
     */
    template< class _DataObjectType >
    class DataObjectServer
    {
        static ORO_CoreLib::NameServer< _DataObjectType* > ns;
        std::string prefix;

    public :
        typedef _DataObjectType DataObjectType;
        typedef _DataObjectType* DataObjectType_ptr;
        typedef typename _DataObjectType::DataType _MemberType;
        typedef _MemberType MemberType;
        typedef _MemberType* MemberType_ptr;

        /**
         * Create a 'stub' like object which grants access
         * to the global DataObjectServer. This 'stub' can be given
         * a prefix to add a scope-like qualifier to the name, thus
         * avoiding name clashes. The use of the prefix is fully
         * transparent and does not need to be known to the user.
         *
         * @param _prefix The 'scope'/prefix to automatically add to
         *        each lookup.
         */
        DataObjectServer(const std::string _prefix = std::string() ) 
            : prefix(_prefix) {}

        /**
         * Get a member variable of a DataObject.
         * @return true if it could be get, false otherwise.
         */
        bool Get(const std::string& name, MemberType& m)
        {
            DataObjectType_ptr res;
            if ( (res = ns.getObject(prefix + name) ) )
                {
                    res->Get(m);
                    //m = *res;
                    return true;
                }
            return false;
        }

        /**
         * @brief Get a pointer to a DataObject.
         *
         * @return true if it could be get, false otherwise.
         */
        bool Get( const std::string& name, DataObjectType*& m )
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
         * @return true if it could be set, false otherwise.
         */
        bool Set( const std::string& name, const MemberType& m)
        {
            DataObjectType_ptr res;
            if (( res = ns.getObject(prefix + name)) ) {
                res->Set(m);
                //*res = m;
                return true;
            }
            return false;
        }

        /**
         * @brief Register a DataObject with a certain name.
         */
        void reg( DataObjectType_ptr m, const std::string& name)
        {
            ns.registerObject(m, prefix + name);
        }

        /**
         * @brief Deregister a DataObject.
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
        NameSubClass(const std::string& prefix, const pair_type& t, index_type index) 
            : DataObjectServer<First>(prefix), 
              NameSubClass<Rest>(prefix, t, index + 1 )
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

        template< typename pair_type, typename index_type>
        NameSubClass(const std::string& prefix, const pair_type& t, index_type index) 
            : DataObjectServer<First>(prefix)
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
        bool Get( First<nil_type>& ) {}
        bool Set( First<nil_type>& ) {}
        template< typename pair_type, typename index_type>
        NameSubClass(const std::string& prefix, const pair_type& t, index_type index) {}
    };

    /**
     * NameSubClass specialisation.
     * When a plain nil_type is encountered on all but last template slot.
     */
    template<typename First>
    struct NameSubClass< NameList< typename has_nil<First>::type, nil_type> >
    {
        bool Get( First& ) {}
        bool Set( First& ) {}
        template< typename pair_type, typename index_type>
        NameSubClass(const std::string& prefix, const pair_type& t, index_type index) {}
    };

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

    // Deprecated, erase when code is stable :
//     template< typename C>
//     struct NakedContainer
//     {
//         template< typename D>
//         struct DataObjectType { typedef DataObject<D> type; };
//         // without data objects (original):
//         typedef C NamesTypes;
//         typedef NameSubClass< NameList<typename C::T0, NameList<typename C::T1, NameList<typename C::T2, NameList<typename C::T3, NameList<typename C::T4,NameList<typename C::T5,NameList<typename C::T6, NameList<typename C::T7, NameList<typename C::T8, NameList< typename C::T9> > > > > > > > > > > tree;
//     };

        /**
         * @brief The templated nameserved dataobject. It can be of any DataType/DataObjectType.
         *
         * This class is the toplevel class that represents a dataobject of a given type
         * (simple, locked, priority_set,...) and which contains one not nameserved type and
         * multiple nameserved types.
         *
         *  Automatic registration.
         * BaseContainer contains all not nameserved objects in a standard DataObject.
         * NameContainer contains all names of the following types, retrievable by an std::iterator.
         */
    template<typename _NameContainer>
    class NameServedDataObject
        : public _NameContainer::NamesTypes, 
//          public MemberFromBase<typename _NameContainer::NamesTypes::reverse_iterator>,  // deprecated
          public _NameContainer::tree, 
          public _NameContainer::DataObjectType< typename _NameContainer::NamesTypes::DataType>::type // To provide the generic DataObjectInterface
    {
        //NameContainer nc;
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
         * @param prefix The scope of the nameserver, to avoid clashes with
         *        other nameservers (the user does not see this).
         */
        NameServedDataObject(const std::string prefix="") 
            : _NameContainer::NamesTypes(), 
              // MemberFromBase<typename _NameContainer::NamesTypes::reverse_iterator>( rend() ), // deprecated
              _NameContainer::tree( prefix, std::make_pair( begin(), end() ), 0 )
        {
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

//      void Get( DataType& pull) const;
//      const DataType& Get() const;
//      void Set( const DataType& push);
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
        }; 


    } // namespace detail

}

#endif
