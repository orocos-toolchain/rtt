/***************************************************************************
  tag: Peter Soetens  Thu Oct 10 16:16:57 CEST 2002  NameServer.hpp 

                        NameServer.hpp -  description
                           -------------------
    begin                : Thu October 10 2002
    copyright            : (C) 2002 Peter Soetens
    email                : peter.soetens@mech.kuleuven.ac.be
 
 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
 
 
#ifndef NAMESERVER_HPP
#define NAMESERVER_HPP

#include "os/fosi.h"
#include <boost/call_traits.hpp>

#include <iterator>
#include <string>
#include <map>

namespace ORO_CoreLib
{


    /**
     * @brief A nameserver for Orocos classes.
     *
     * There will be a seperate nameserver for every class, it should be statically
     * declared in the private section of the class and you should register your object
     * in the constructor of your nameserved class.<br>
     * <tt>
     * reasoning
     *  -> static ... type specific
     *
     * advantages:
     *  -> with this system you can ask a specific class for component "X" (
     *   with X a string) and you will get immediately the right type
     *   (or 0, which means the object of that type isn't registered)
     *
     * the possibilities of the nameserver are :
     *  -> coupling a name to a typed pointer
     *   Let's say you want object "Servoloop2" of type Servoloop (the type
     *   may be the type of the instance itself or an ancestor class of the
     *   instance). The code is very simple :
     *    Servoloop::nameserver.getObjectByName("Servoloop2")
     *
     *  -> getting all names registered in the nameservers
     *   with the getnBegin() and getnEnd() will get you a name_iterator suitable for iterating
     *   over all registered names
     * </tt>
     *
     * In case two objects are registered with the same name, the first one is kept and the
     * second one rejected.<br>
     * 
     *  @param ValueType  The type of objects you want to have nameserved( usually a pointer type ).
     *  @param NameType The type of the name you want to used (string by default, but you could use ints)
     *  @param Rep The container you wish to use for actually storing the name value pairs
     *   it has to support the [] operator with a NameType
     *   and it needs iterators and begin() and end().
     */
    template <class ValueType, class NameType = std::string, class Rep = std::map<NameType, ValueType> >
    class NameServer
    {
            /**
             * The iterator for iterating over the internal representation
             */
            typedef typename Rep::iterator iterator;
            /**
             * The const_iterator for iterating over the internal representation
             */
            typedef typename Rep::const_iterator const_iterator;

        public:
            /**
             * Construct an empty NameServer.
             */
            NameServer() : objects()
            {}

            /**
             * Destruct a nameserver.
             */
            ~NameServer()
            {}

            /**
             * Determine if a given name is registered
             *
             * @param s the name to check
             *
             * @returns true if <s> is registered, false otherwise
             */
            bool isNameRegistered( typename boost::call_traits<NameType>::param_type s ) const
            {
                return ( objects.find( s ) != objects.end() );
            }

            /**
             * Determine if a given object is registered
             *
             * @param o the object to check
             *
             * @returns true if <o> is registered, false otherwise
             */
            bool isObjectRegistered( typename boost::call_traits<ValueType>::param_type o ) const
            {
                for ( const_iterator i = objects.begin(); i != objects.end(); ++i )
                {
                    if ( ( *i ).second == o )
                        return true;
                }

                return false;
            }

            /**
             * Get the object registered for a name.
             *
             * @param s the name of the object you need
             * 
             * @returns the object the name is registered with,
             *  if the name isn't registered, it will return 0
             */
            ValueType getObject( typename boost::call_traits<NameType>::param_type s ) const
            {
                if ( isNameRegistered( s ) )
                {
                    const_iterator itc = objects.find( s );
                    return ( *itc ).second;
                }

                else
                {
                    return 0;
                }
            }

            /**
             * Get the object registered earlier by that name.
             *
             * @param s the name of the object you need.
             * @deprecated by Get()
             * 
             * @returns the object the name is registered with,
             *  if the name isn't registered, it will return 0
             */
        ValueType getObjectByName( typename::boost::call_traits<NameType>::param_type s ) const
        {
            return getObject(s);
        }
            /**
             * Get the name registered for a object.
             *
             * @param s the object of the name you need
             * 
             * @returns the name the object is registered with,
             *  if the object isn't registered, it will return 0
             */
            typename boost::call_traits<NameType>::const_reference 
                getName( const typename boost::call_traits<ValueType>::param_type s ) const
            {
                for ( const_iterator i = objects.begin(); i != objects.end(); ++i )
                {
                    if ( ( *i ).second == s )
                        return ( *i ).first;
                }

                return NoName;
            }

            /**
             * Get the name registered for a object.
             *
             * @param s the object of the name you need
             * @deprecated by getName()
             * 
             * @returns the name the object is registered with,
             *  if the object isn't registered, it will return 0
             */
            typename boost::call_traits<NameType>::const_reference 
                getNameByObject( const typename boost::call_traits<ValueType>::param_type s ) const
        {
            return getName( s );
        }
            /**
             * Register an object with a name
             *
             * @param obj the instance you wish to register with a name
             * @param name stop. think. think of your object as a cuddly little bear,
             *  now look at it hopping around in the garden. Call the little cute bear
             *  over. What is the first word that comes into your mind to call the little
             *  cute object bear ? That's what you put here.
             */
            void registerObject( typename boost::call_traits<ValueType>::const_reference obj, typename boost::call_traits<NameType>::param_type name )
            {
                objects[ name ] = obj;
            }

            /**
             * Remove an object from the nameserver registrations
             *
             * @param obj the object you want removed
             * 
             * @post obj will not be registered anymore
             */
            void unregisterObject( typename boost::call_traits<ValueType>::const_reference obj )
            {
                iterator i(objects.begin());

                while ( i != objects.end() )
                {
                    for ( i = objects.begin();
                            i != objects.end();
                            ++i )
                    {
                        if ( ( *i ).second == obj )
                            break;
                    }

                    if ( i != objects.end() )
                        objects.erase( i );
                }
            }

            /**
             * Remove a name from the nameserver registrations
             *
             * @param name the name of the object you want remove
             * 
             */
            void unregisterName( typename boost::call_traits<NameType>::param_type name )
            {
                iterator i = objects.find( name );

                if ( i != objects.end() )
                    objects.erase( i );
            }

            /**
             * An Iterator to iterate over the registered objects
             *
             * On dereference it will give you a ValueType (an object)
             */
#if __GNUC__ == 2
        class value_iterator : public bidirectional_iterator<ValueType, int>
#else
        class value_iterator : public std::iterator<ValueType, int>
#endif
            {
                protected:
                    iterator i;

                public:
                    value_iterator( iterator _i ) : i( _i )
                    {}

                    typename boost::call_traits<ValueType>::value_type operator*()
                    {
                        return ( ( *i ).second );
                    }

                    value_iterator operator++()
                    {
                        ++i;
                        return *this;
                    }

                    value_iterator operator--()
                    {
                        --i;
                        return *this;
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
             * An Iterator to iterate over the registered names
             *
             * on dereference it will give you a NameType (a name)
             */
#if __GNUC__ == 2
        class name_iterator : public bidirectional_iterator<NameType, int>
#else
        class name_iterator : public std::iterator<NameType, int>
#endif
            {

                protected:
                    iterator i;

                public:
                    name_iterator( iterator _i ) : i( _i )
                    {}

                    typename boost::call_traits<NameType>::value_type operator*()
                    {
                        return ( ( *i ).first );
                    }

                    name_iterator operator++()
                    {
                        ++i;
                        return *this;
                    }

                    name_iterator operator--()
                    {
                        --i;
                        return *this;
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
             * Get an iterator to the beginning of the names list.
             */
            name_iterator getNameBegin() { return name_iterator( objects.begin() ); }

            /**
             * Get an iterator to the end of the names list.
             */
            name_iterator getNameEnd()   { return name_iterator( objects.end() ); }

            /**
             * Get an iterator to the beginning of the objects list.
             */
            value_iterator getValueBegin() { return value_iterator( objects.begin() ); }
            
            /**
             * Get an iterator to the end of the objects list.
             */
            value_iterator getValueEnd()   { return value_iterator( objects.end() ); }

        private:
            Rep objects;
            static NameType NoName;
    };

    template<class T, class V, class W>
    V NameServer<T, V, W>::NoName;
    
}

#endif // NAMESERVER_HPP
