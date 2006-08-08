#ifndef ORO_COMMAND_REPOSITORY_HPP
#define ORO_COMMAND_REPOSITORY_HPP

#include "DataSourceArgsCommand.hpp"
#include "OperationFactory.hpp"
#include "CommandC.hpp"
#include "LocalCommand.hpp"
//#define ORO_REMOTING
#ifdef ORO_REMOTING
#include "RemoteCommand.hpp"
#endif

namespace RTT
{
    /**
     * A command repository stores a number of commands from a Task which
     * can be used by other tasks or from scripts.
     */
    class CommandRepository
        : public OperationFactory<DispatchInterface*>
    {
        template<class T>
        inline T* getpointer(T& t) {
            return &t;
        }
        template<class T>
        inline T* getpointer(T* t) {
            return t;
        }
        
        std::map<std::string,DispatchInterface*> simplecommands;
    public:
        typedef CommandFactory Factory;

        ~CommandRepository()
        {
            clear();
        }

        /** 
         * Retrieve a previously added Command.
         * 
         * @param name The name of the Command
         * @param args A vector of command arguments.
         * 
         * @return A dispatchable object which is a new Command object.
         */
        DispatchInterface* getCommand( std::string name,
                                          const std::vector<DataSourceBase::shared_ptr>& args) const
        {
            return this->produce(name, args);
        }

        /** 
         * Return the pointer to an added simple Command.
         * 
         * @param name The name of the command to retrieve.
         * @param Signature The function signature of the command, for
         * example: getCommand<bool(int,double)>("name");
         * 
         * @return A new pointer to a simple Command, or null if it does not exist.
         */
        template<class Signature>
        DispatchInterface* getCommand( std::string name )
        {
            if ( simplecommands.count(name) )
                return simplecommands[name]->clone();
#ifdef ORO_REMOTING
            if ( this->hasMember(name ) ) {
                return new detail::RemoteCommand<Signature>(this, name);
            }
#endif
            return 0;
        }
        /** 
         * Retrieve the completion condition of a previously added Command.
         * 
         * @param name The name of the Command
         * @param args A vector of command arguments
         * 
         * @return A condition which evaluates the command's completion.
         */
        ConditionInterface* getCondition( std::string name,
                                         const std::vector<DataSourceBase::shared_ptr>& args) const
        {
            DispatchInterface* di = this->produce(name, args);
            ConditionInterface* ret = di->createCondition();
            delete di;
            return ret;
        }

        void clear() {
            while ( !simplecommands.empty() ) {
                delete simplecommands.begin()->second;
                simplecommands.erase( simplecommands.begin() );
            }
            OperationFactory<DispatchInterface*>::clear();
        }


        /** 
         * Create a CommandC container object, which can be used
         * to access an added Command.
         * 
         * @param name The name of the Command
         * 
         * @return A new CommandC object.
         */
        CommandC create(std::string name) {
            return CommandC( this, name );
        }

        /** 
         * Add a Command to the interface. You can retrieve it with getCommand.
         * 
         * @param com A pointer to a Command object.
         * 
         * @return true if the command could be added.
         */
        template<class CommandT>
        bool addCommand( CommandT* com )
        {
            if ( simplecommands.count( com->getName() ) )
                return false;
            simplecommands[com->getName()] = com->getCommandImpl()->clone();
            return true;
        }


        /** 
         * Add an existing Command, which takes no arguments, to the Operations interface.
         * 
         * @param c A pointer to the existing command.
         * @param description A user readable description of what this command does.
         * 
         * @return true if the command could be added.
         */
        template<class CommandT>
        bool addCommand( CommandT com, const char* description) 
        {
            typedef typename boost::remove_pointer<CommandT>::type CommandVT;
            typedef typename boost::add_pointer<CommandVT>::type CommandPT;
            typedef typename CommandVT::Signature ComSig;
            CommandPT c = this->getpointer(com);
            detail::LocalCommand<ComSig>* lc = dynamic_cast<detail::LocalCommand<ComSig>*>( c->getCommandImpl() );
            // We can only add local commands.
            if ( !lc )
                return false;
            // First add the command to the normal interface.
            if ( this->addCommand( c ) == false )
                return false;
            // Next, add it to the Command from 'DataSource' interface.
            this->add( c->getName(), new detail::OperationFactoryPart0<DispatchInterface*, detail::DataSourceArgsCommand<ComSig> >( 
                  detail::DataSourceArgsCommand<ComSig>(lc->getCommandFunction(),
                                        lc->getConditionFunction(), 
                                        lc->getCommandProcessor(), lc->isInverted() ), description) );
            return true;
        }

        /** 
         * Add an existing Command, which takes one argument, to the
         * Operations Interface.
         * 
         * @param c A pointer to the existing command.
         * @param description A user readable description of what this command does.
         * @param arg1 A single word name of the argument.
         * @param arg1_description A user readable description of the argument.
         * 
         * @return true if the command could be added.
         */
        template<class CommandT>
        bool addCommand( CommandT com, const char* description,
                         const char* arg1, const char* arg1_description)
        {
            typedef typename boost::remove_pointer<CommandT>::type CommandVT;
            typedef typename boost::add_pointer<CommandVT>::type CommandPT;
            typedef typename CommandVT::Signature ComSig;
            CommandPT c = this->getpointer(com);
            detail::LocalCommand<ComSig>* lc = dynamic_cast<detail::LocalCommand<ComSig>*>( c->getCommandImpl() );
            if ( !lc )
                return false;
            if ( this->addCommand( c ) == false )
                return false;
            this->add( c->getName(), new detail::OperationFactoryPart1<DispatchInterface*, detail::DataSourceArgsCommand<ComSig> >( 
                  detail::DataSourceArgsCommand<ComSig>(lc->getCommandFunction(),
                                        lc->getConditionFunction(), 
                                        lc->getCommandProcessor(), lc->isInverted() ), 
                  description, arg1, arg1_description) );
            return true;
        }

        /** 
         * Add an existing Command, which takes two arguments, to the
         * Operations Interface.
         * 
         * @param c A pointer to the existing command.
         * @param description A user readable description of what this command does.
         * @param arg1 A single word name of the first argument.
         * @param arg1_description A user readable description of the first argument.
         * @param arg2 A single word name of the second argument.
         * @param arg2_description A user readable description of the second argument.
         * 
         * @return true if the command could be added.
         */
        template<class CommandT>
        bool addCommand( CommandT com, const char* description,
                         const char* arg1, const char* arg1_description,
                         const char* arg2, const char* arg2_description)
        {
            typedef typename boost::remove_pointer<CommandT>::type CommandVT;
            typedef typename boost::add_pointer<CommandVT>::type CommandPT;
            typedef typename CommandVT::Signature ComSig;
            CommandPT c = this->getpointer(com);
            detail::LocalCommand<ComSig>* lc = dynamic_cast<detail::LocalCommand<ComSig>*>( c->getCommandImpl() );
            if ( !lc )
                return false;
            if ( this->addCommand( c ) == false )
                return false;
            this->add( c->getName(), new detail::OperationFactoryPart2<DispatchInterface*, detail::DataSourceArgsCommand<ComSig> >( 
                  detail::DataSourceArgsCommand<ComSig>(lc->getCommandFunction(),
                                        lc->getConditionFunction(), 
                                        lc->getCommandProcessor(), lc->isInverted() ), 
                  description, arg1, arg1_description,
                  arg2, arg2_description) );
            return true;
        }


        template<class CommandT>
        bool addCommand( CommandT com, const char* description,
                         const char* arg1, const char* arg1_description,
                         const char* arg2, const char* arg2_description,
                         const char* arg3, const char* arg3_description)
        {
            typedef typename boost::remove_pointer<CommandT>::type CommandVT;
            typedef typename boost::add_pointer<CommandVT>::type CommandPT;
            typedef typename CommandVT::Signature ComSig;
            CommandPT c = this->getpointer(com);
            detail::LocalCommand<ComSig>* lc = dynamic_cast<detail::LocalCommand<ComSig>*>( c->getCommandImpl() );
            if ( !lc )
                return false;
            if ( this->addCommand( c ) == false )
                return false;
            this->add( c->getName(), new detail::OperationFactoryPart3<DispatchInterface*, detail::DataSourceArgsCommand<ComSig> >( 
                  detail::DataSourceArgsCommand<ComSig>(lc->getCommandFunction(),
                                               lc->getConditionFunction(), 
                                               lc->getCommandProcessor(), lc->isInverted() ), 
                  description, arg1, arg1_description,
                  arg2, arg2_description,
                  arg3, arg3_description) );
            return true;
        }

        template<class CommandT>
        bool addCommand( CommandT com, const char* description,
                         const char* arg1, const char* arg1_description,
                         const char* arg2, const char* arg2_description,
                         const char* arg3, const char* arg3_description,
                         const char* arg4, const char* arg4_description)
        {
            typedef typename boost::remove_pointer<CommandT>::type CommandVT;
            typedef typename boost::add_pointer<CommandVT>::type CommandPT;
            typedef typename CommandVT::Signature ComSig;
            CommandPT c = this->getpointer(com);
            detail::LocalCommand<ComSig>* lc = dynamic_cast<detail::LocalCommand<ComSig>*>( c->getCommandImpl() );
            if ( !lc )
                return false;
            if ( this->addCommand( c ) == false )
                return false;
            this->add( c->getName(), new detail::OperationFactoryPart4<DispatchInterface*, detail::DataSourceArgsCommand<ComSig> >( 
                  detail::DataSourceArgsCommand<ComSig>(lc->getCommandFunction(),
                                               lc->getConditionFunction(), 
                                               lc->getCommandProcessor(), lc->isInverted() ), 
                  description, arg1, arg1_description,
                  arg2, arg2_description,
                  arg3, arg3_description,
                  arg4, arg4_description) );
            return true;
        }

        template<class CommandT,class CompT>
        bool addCommandDS( DataSource< boost::weak_ptr<CompT> >* wp, CommandT c, const char* description) 
        {
            using namespace detail;
            typedef typename CommandT::Signature ComSig;
            if ( this->hasMember(c.getName() ) )
                return false;
            typedef FunctorDS0<ComSig> CommandF;
            typedef detail::DataSourceArgsCommand<ComSig,
                                  CommandF> DSComm;
            this->add( c.getName(), new detail::OperationFactoryPart0<DispatchInterface*, DSComm>( 
                        DSComm( CommandF(wp, c.getCommandFunction()),
                                CommandF(wp, c.getConditionFunction()), 
                                c.getCommandProcessor(), c.isInverted() ),
                        description) );
            return true;
        }

        template<class CommandT, class CompT>
        bool addCommandDS( DataSource<boost::weak_ptr<CompT> >* wp, CommandT c, const char* description,
                         const char* arg1, const char* arg1_description)
        {
            using namespace detail;
            typedef typename CommandT::Signature ComSig;
            typedef FunctorDS1<ComSig> CommandF;
            typedef detail::DataSourceArgsCommand<ComSig,
                                  CommandF> DSComm;
            if ( this->hasMember(c.getName() ) )
                return false;
            this->add( c.getName(), new detail::OperationFactoryPart1<DispatchInterface*, DSComm, typename DSComm::traits::arg2_type>( 
                        DSComm( CommandF(wp, c.getCommandFunction()),
                                CommandF(wp, c.getConditionFunction()), 
                                c.getCommandProcessor(), c.isInverted() ),
                        description, arg1, arg1_description) );
            return true;
        }


    };

}


#endif
