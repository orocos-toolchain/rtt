/***************************************************************************
  tag: Peter Soetens  Thu Oct 10 16:16:57 CEST 2002  NameServer.hpp

                        NameServer.hpp -  description
                           -------------------
    begin                : Thu October 10 2002
    copyright            : (C) 2002 Peter Soetens
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

#ifndef ORO_NAMESERVER_HPP
#define ORO_NAMESERVER_HPP

#include <iterator>
#include <string>
#include <map>
// #include "rtt/rtt-config.h"
// #ifdef OROPKG_CORELIB_REPORTING
// #include "../../Logger.hpp"
// #endif

namespace RTT
{ namespace dev {

    /**
     * @brief A nameserver for Orocos Device classes.
     *
     * This class allows to globally set up device driver objects and find them back
     * in the same process by querying this class. Every Orocos device driver type has a
     * Device::nameserver type which allows retrieval of the object:
     @verbatim
     // At application startup code:
     Device* init_device = new Device("device_name");
     // ...
     // In other places:
     Device* myDevice = Device::nameserver.getObject("device_name");
     @endverbatim
     * In case two objects are registered with the same name, the first one is kept and the
     * second one rejected.
     *
     *  @param _ValueType  The type of objects you want to have nameserved( usually a pointer type ).
     */
    template < class _ValueType >
    class NameServer
    {
    public:
        typedef _ValueType ValueType;
        typedef std::string NameType;
        typedef std::map<NameType, ValueType> Rep;
        /**
         * The iterator for iterating over the internal representation
         */
        typedef typename Rep::iterator iterator;
        /**
         * The const_iterator for iterating over the internal representation
         */
        typedef typename Rep::const_iterator const_iterator;

        /**
         * @brief Construct an empty NameServer.
         */
        NameServer() : objects()
        {}

        /**
         * @brief Destruct a nameserver.
         */
        ~NameServer()
        {}

        /**
         * @brief Determine if a given name is registered
         *
         * @param s the name to check
         *
         * @return true if \a s is registered, false otherwise
         */
        bool isNameRegistered( const NameType& s ) const
        {
            return ( objects.find( s ) != objects.end() );
        }

        /**
         * @brief Determine if a given object is registered
         *
         * @param o the object to check
         *
         * @return true if \a o is registered, false otherwise
         */
        bool isObjectRegistered( const ValueType o ) const
        {
            for ( const_iterator i = objects.begin(); i != objects.end(); ++i )
                {
                    if ( ( *i ).second == o )
                        return true;
                }

            return false;
        }

        /**
         * @brief Get the object registered for a name.
         *
         * @param s the name of the object you need
         *
         * @return the object the name is registered with,
         *  if the name isn't registered, it will return 0
         */
        ValueType getObject( const NameType& s ) const
        {
            const_iterator itc = objects.find( s );
            if ( itc == objects.end() )
                return ValueType();
            return( *itc ).second;
        }

        /**
         * @brief Get the name registered for a object.
         *
         * @param s the object of the name you need
         *
         * @return the name the object is registered with,
         *  if the object isn't registered, it will return 0
         */
        const NameType&
        getName( const ValueType s ) const
        {
            for ( const_iterator i = objects.begin(); i != objects.end(); ++i )
                {
                    if ( ( *i ).second == s )
                        return ( *i ).first;
                }

            return NoName;
        }

        /**
         * @brief Register an object with a name. If an object with such
         * a name already exists, the present one will not be overwritten,
         * and this call is a no-op.
         *
         * @param obj  The instance you wish to register with a name
         * @param name The name of the object
         */
        bool registerObject( const ValueType obj, const NameType& name )
        {
            if ( isNameRegistered( name ) )
                return false;
// #ifdef OROPKG_CORELIB_REPORTING
//             Logger::log() << Logger::Debug << "NameServer : Adding " << name << Logger::endl;
// #endif
            objects.insert(std::make_pair(name,obj));
            return true;
        }

        /**
         * @brief Remove an object from the nameserver registrations.
         *
         * @param obj the object you want removed
         *
         * @post obj will not be registered anymore
         */
        void unregisterObject( const ValueType obj )
        {
            iterator i(objects.begin());

            /**
             * @todo might be improved by using equal_range
             */
            for(;;)
                {
                    for ( i = objects.begin();
                          i != objects.end();
                          ++i )
                        {
                            if ( ( *i ).second == obj )
                                break;
                        }

                    if ( i == objects.end() ) return;
// #ifdef OROPKG_CORELIB_REPORTING
//                         Logger::log() << Logger::Debug << "NameServer : Removing " << (*i).first << Logger::endl;
// #endif
                       objects.erase( i );
                }
        }

        /**
         * @brief Remove a name from the nameserver registrations.
         *
         * @param name the name of the object you want remove
         *
         */
        void unregisterName( const NameType& name )
        {
            iterator i( objects.find( name ) );

            if ( i != objects.end() ) {
                objects.erase( i );
// #ifdef OROPKG_CORELIB_REPORTING
//                 Logger::log() << Logger::Debug << "NameServer : Removing " << name << Logger::endl;
// #endif
                    }
        }

        /**
         * @brief An Iterator to iterate over the registered objects.
         *
         * On dereference it will give you a ValueType (an object)
         */
#if __GNUC__ == 2
        class value_iterator : public bidirectional_iterator<ValueType, int>
#else
        class value_iterator : public std::iterator<std::input_iterator_tag, ValueType>
#endif
        {
        protected:
        	typename NameServer<_ValueType>::iterator i;

        public:
            value_iterator( iterator _i ) : i( _i )
            {}

            ValueType operator*()
            {
                return ( ( *i ).second );
            }

            value_iterator& operator++()
            {
                ++i;
                return *this;
            }

            value_iterator& operator--()
            {
                --i;
                return *this;
            }

            value_iterator operator++( int )
            {
                value_iterator ret;
                operator++();
                return ret;
            }

            value_iterator operator--( int )
            {
                value_iterator ret;
                operator--();
                return ret;
            }

            bool operator==( value_iterator other )
            {
                return ( i == other.i );
            }

            bool operator!=( value_iterator other )
            {
                return ( i != other.i );
            }

            int operator- ( value_iterator other )
            {
                return ( i -other.i );
            }
        };

        /**
         * @brief An Iterator to iterate over the registered names.
         *
         * On dereference it will give you a NameType (a name).
         */
#if __GNUC__ == 2
        class name_iterator : public bidirectional_iterator<NameType, int>
#else
        class name_iterator : public std::iterator< std::input_iterator_tag , NameType>
#endif
        {

        protected:
        	typename NameServer<_ValueType>::iterator i;

        public:

            name_iterator( iterator _i ) : i( _i )
            {}

            NameType operator*()
            {
                return ( ( *i ).first );
            }

            name_iterator operator++( int )
            {
                name_iterator ret( i );
                operator++();
                return ret;
            }

            name_iterator& operator++()
            {
                ++i;
                return *this;
            }

            name_iterator& operator--()
            {
                --i;
                return *this;
            }

            name_iterator operator--( int )
            {
                name_iterator ret( i );
                operator--();
                return ret;
            }

            bool operator==( name_iterator other )
            {
                return ( i == other.i );
            }

            bool operator!=( name_iterator other )
            {
                return ( i != other.i );
            }

            int operator- ( name_iterator other )
            {
                return ( i -other.i );
            }
        };

        /**
         * @brief Get an iterator to the beginning of the names list.
         */
        name_iterator getNameBegin() { return name_iterator( objects.begin() ); }

        /**
         * @brief Get an iterator to the end of the names list.
         */
        name_iterator getNameEnd() { return name_iterator( objects.end() ); }

        /**
         * @brief Get an iterator to the beginning of the objects list.
         */
        value_iterator getValueBegin() { return value_iterator( objects.begin() ); }

        /**
         * @brief Get an iterator to the end of the objects list.
         */
        value_iterator getValueEnd() { return value_iterator( objects.end() ); }

    private:
        Rep objects;
        static NameType NoName;
    };

    template<class T>
    std::string NameServer<T>::NoName;

}}

#endif // NAMESERVER_HPP
