#ifndef MEMBERFACTORYCOMPOSITE_HPP
#define MEMBERFACTORYCOMPOSITE_HPP

#include "MemberFactoryInterface.hpp"

namespace ORO_Execution
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

        bool hasMember(const std::string& com) const 
        {
            return one->hasMember(com) || other->hasMember(com);
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

        ORO_CoreLib::PropertyBag
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
                       const ORO_CoreLib::PropertyBag& args ) const
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
    };
}

#endif
