/***************************************************************************
  tag: Peter Soetens  Tue Dec 21 22:43:08 CET 2004  MemberFactoryComposite.hpp 

                        MemberFactoryComposite.hpp -  description
                           -------------------
    begin                : Tue December 21 2004
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
 
 
#ifndef MEMBERFACTORYCOMPOSITE_HPP
#define MEMBERFACTORYCOMPOSITE_HPP

#include "MemberFactoryInterface.hpp"

namespace RTT
{
    /**
     * Used by the GlobalFactory to merge two
     * MemberFactories for one object.
     */
    class MemberFactoryComposite
        : public MemberFactoryInterface
    {
        const MemberFactoryInterface *one, *other;
    public:
        MemberFactoryComposite( const MemberFactoryInterface* f,
                                 const MemberFactoryInterface* s)
            : one(f), other(s)
        {}

        ~MemberFactoryComposite() {
            delete one;
            delete other;
        }

        bool hasMember(const std::string& method) const 
        {
            return one->hasMember(method) || other->hasMember(method);
        }

        std::string getResultType( const std::string& method ) const 
        {
            if ( one->hasMember( method ) )
                return one->getResultType(method);
            return other->getResultType(method);
        }

        std::vector<std::string> getNames() const
        {
            std::vector<std::string> res = one->getNames();
            std::vector<std::string> res2 = other->getNames();
            res.insert( res.end(), res2.begin(), res2.end() );
            return res;
        }

        std::string getDescription( const std::string& com ) const
        {
            if ( one->hasMember( com ) )
                return one->getDescription(com);
            return other->getDescription(com);
        }

        virtual int getArity(const std::string& com ) const {
            if ( one->hasMember( com ) )
                return one->getArity(com);
            return other->getArity(com);
        }


        PropertyBag
        getArgumentSpec( const std::string& member ) const
        {
            if ( one->hasMember( member ) )
                return one->getArgumentSpec(member);
            return other->getArgumentSpec(member);
        }

        std::vector< ArgumentDescription > getArgumentList( const std::string& member ) const
        {
            if ( one->hasMember( member ) )
                return one->getArgumentList(member);
            return other->getArgumentList(member);
        }

        DataSourceBase* create( const std::string& member,
                       const PropertyBag& args ) const
        {
            if ( one->hasMember( member ) )
                return one->create(member, args);
            return other->create(member, args);
        }

        DataSourceBase* create(
                      const std::string& member,
                      const std::vector<DataSourceBase*>& args ) const
        {
            if ( one->hasMember( member ) )
                return one->create(member, args);
            return other->create(member, args);
        }

        DataSourceBase* create(
                      const std::string& member,
                      const std::vector<DataSourceBase::shared_ptr>& args ) const
        {
            if ( one->hasMember( member ) )
                return one->create(member, args);
            return other->create(member, args);
        }
    };
}

#endif
