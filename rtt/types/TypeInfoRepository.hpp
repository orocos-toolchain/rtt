/***************************************************************************
  tag: Peter Soetens  Mon Jan 19 14:11:26 CET 2004  TypeInfoRepository.hpp

                        TypeInfoRepository.hpp -  description
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

#ifndef ORO_CORELIB_TYPEINFOREPOSITORY_HPP
#define ORO_CORELIB_TYPEINFOREPOSITORY_HPP

#include <map>
#include <vector>
#include <string>
#include <boost/shared_ptr.hpp>
#include <boost/function.hpp>
#include "rtt/types/TypeInfo.hpp"
#include "rtt/types/TypeInfoGenerator.hpp"

namespace RTT
{ namespace types {

    /**
     * This class contains all known types to Orocos.
     * @see TemplateTypeInfo to add your own classes to Orocos.
     */
    class RTT_API TypeInfoRepository
    {
        TypeInfoRepository();
        typedef std::map<std::string, TypeInfo*> map_t;
        map_t data;

        typedef std::vector<TransportPlugin*> Transports;
        Transports transports;
        mutable os::Mutex type_lock;
        
        boost::function<bool (const std::string &)> loadTypeKitForName;
        
        TypeInfo* typeInternal( const std::string& name ) const;
    public:
        ~TypeInfoRepository();
        typedef boost::shared_ptr<TypeInfoRepository> shared_ptr;
        static shared_ptr Instance();
        static void Release();
        
        void setAutoLoader(const boost::function<bool (const std::string &)> &loader);
        
        /**
         * Retrieve a type with a given \a name.
         * @param name The type name as specified by the
         * TypeInfo object, or it's dotted equivalent.
         */
        TypeInfo* type( const std::string& name ) const;

        /**
         * Add a type to the Orocos type repository.
         * @param ti A type info object that becomes owned by
         * the TypeInfoRepository (so use new()). It's name must be unique.
         * @return false if \a ti was already added or if
         * ti->getTypeName() already exists in this repository.
         */
        bool addType( TypeInfo* ti );

        /**
         * Add a type by using a type info generator. This class
         * is responsible for installing the TypeInfo object,
         * registering the name and installing the necessary factories.
         */
        bool addType( TypeInfoGenerator* tig );

        /**
         * List all types registered with their original
         * type name. See getDottedTypes for the typenames
         * written in the 'package.type' notation.
         */
        std::vector<std::string> getTypes() const;

        /**
         * List all registered types in the package.type
         * notation.
         */
        std::vector<std::string> getDottedTypes() const;

        /**
         * Convert any type name returned by getTypes()
         * to the dotted type notation.
         */
        std::string toDot( const std::string& type ) const;

        /**
         * Return the type info structure of a given type by its type_id.
         */
        TypeInfo* getTypeById(TypeInfo::TypeId type_id) const;

        /**
         * Return the type info structure of a given type name.
         */
        TypeInfo* getTypeById(const char * type_id_name) const;

        /**
         * Return the type info structure of a given type T.
         */
        template<class T>
        TypeInfo* getTypeInfo() const {
            return getTypeById( &typeid(T) );
        }

        /**
         * Call this function to add a new (network) transport
         * for Orocos types.
         */
        void registerTransport( TransportPlugin* tr );

        /**
         * Dump all known types, along with transports and their types, to
         * the log.
         */
        void logTypeInfo() const;

    };

}}

#endif
