
#ifndef COMMANDFACTORYCOMPOSITE_HPP
#define COMMANDFACTORYCOMPOSITE_HPP


namespace ORO_Execution
{
    /**
     * Used by the GlobalFactory to merge two
     * CommandFactories for one object.
     */
    class CommandFactoryComposite
        : public CommandFactoryInterface
    {
        const CommandFactoryInterface* one, *other;
    public:
        CommandFactoryComposite( const CommandFactoryInterface* f,
                                 const CommandFactoryInterface* s)
            : one(f), other(s)
        {}

        ~CommandFactoryComposite() {
            delete one;
            delete other;
        }

        bool hasCommand(const std::string& com) const 
        {
            return one->hasCommand(com) || other->hasCommand(com);
        }

        std::vector<std::string> getCommandList() const
        {
            std::vector<std::string> res = one->getCommandList();
            std::vector<std::string> res2 = other->getCommandList();
            res.insert( res.end(), res2.begin(), res2.end() );
            return res;
        }

        std::string getDescription( const std::string& com ) const
        {
            if ( one->hasCommand( com ) )
                return one->getDescription(com);
            return other->getDescription(com);
        }

        ORO_CoreLib::PropertyBag
        getArgumentSpec( const std::string& command ) const
        {
            if ( one->hasCommand( command ) )
                return one->getArgumentSpec(command);
            return other->getArgumentSpec(command);
        }

        std::vector< ArgumentDescription > getArgumentList( const std::string& command ) const
        {
            if ( one->hasCommand( command ) )
                return one->getArgumentList(command);
            return other->getArgumentList(command);
        }

        ComCon create( const std::string& command,
                       const ORO_CoreLib::PropertyBag& args, bool asyn=true ) const
        {
            if ( one->hasCommand( command ) )
                return one->create(command, args, asyn);
            return other->create(command, args, asyn);
        }

        ComCon create(
                      const std::string& command,
                      const std::vector<DataSourceBase*>& args, bool asyn=true ) const
        {
            if ( one->hasCommand( command ) )
                return one->create(command, args, asyn);
            return other->create(command, args, asyn);
        }
    };
};

#endif
