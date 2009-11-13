/***************************************************************************
  tag: FMTC  do nov 2 13:06:10 CET 2006  CommandRepository.hpp

                        CommandRepository.hpp -  description
                           -------------------
    begin                : do november 02 2006
    copyright            : (C) 2006 FMTC
    email                : peter.soetens@fmtc.be

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


#ifndef ORO_COMMAND_REPOSITORY_HPP
#define ORO_COMMAND_REPOSITORY_HPP

#include "../internal/DataSourceArgsCommand.hpp"
#include "../internal/OperationFactory.hpp"
#include "../internal/CommandC.hpp"
#include "../internal/LocalCommand.hpp"
#ifdef ORO_REMOTING
#include "../internal/RemoteCommand.hpp"
#endif
#include "../Logger.hpp"
#include <boost/static_assert.hpp>
#include <boost/type_traits/function_traits.hpp>

namespace RTT
{ namespace interface {

    /**
     * A command repository stores a number of commands from a Task which
     * can be used by other tasks or from scripts.
     * @ingroup Commands
     */
    class RTT_API CommandRepository
        : public internal::OperationFactory<base::DispatchInterface*>
    {
        template<class T>
        inline T* getpointer(T& t) {
            return &t;
        }
        template<class T>
        inline T* getpointer(T* t) {
            return t;
        }

        typedef std::map<std::string,base::DispatchInterface*> SimpleCommands;
        SimpleCommands simplecommands;
    public:
        typedef internal::CommandFactory Factory;

        ~CommandRepository();

        /**
         * Return the pointer to an added command for use in a Command object.
         * Store the result in a Command<\a Signature> object.
         *
         * @param name The name of the command to retrieve.
         * @param Signature The function signature of the command, for
         * example: getCommand<bool(int,double)>("name");
         *
         * @return A new pointer to a Command, or null if it does not exist.
         */
        template<class Signature>
        base::DispatchInterface* getCommand( std::string name )
        {
            Logger::In in("CommandRepository::getCommand");
            if ( simplecommands.count(name) ) {
                if ( dynamic_cast< base::CommandBase<Signature>* >(simplecommands[name]) )
                    return simplecommands[name]->clone();
                else
                    log(Error) << "Command '"<< name <<"' found, but has wrong Signature."<<endlog();
                return 0;
            }

#ifdef ORO_REMOTING
            if ( this->hasMember(name ) ) {
                return new internal::RemoteCommand<Signature>(this, name);
            }
#endif
            log(Warning) << "No such command: "<< name <<endlog();
            return 0;
        }

        /**
         * Clears the complete interface and frees all memory.
         */
        void clear();

        /**
         * Returns the names of all commands added to this interface.
         * @see getNames() to get a list of all commands available to scripting.
         */
        std::vector<std::string> getCommands() const;

        /**
         * Query for the existence of a Command in this interface.
         * @see hasMember() to verify if a command is available to scripting as well.
         */
        bool hasCommand(const std::string& name) const;

        /**
         * Removes a previously added command.
         */
        void removeCommand(const std::string& name);

        /**
         * Add a Command object to the command interface. The command is
         * added to the C++ interface and not added to the scripting interface.
         * @see getCommand to retrieve it.
         *
         * @param com A pointer to a Command object.
         *
         * @return true if the command could be added.
         */
        template<class CommandT>
        bool addCommand( CommandT* com )
        {
            Logger::In in("CommandRepository");
            if ( simplecommands.count( com->getName() ) ) {
                log(Error) << "Failed to addCommand: '"<< com->getName() <<"' already added." <<endlog();
                return false;
            }
            if (  com->getName().empty() || !com->ready() ) {
                log(Error) << "Failed to addCommand: '"<< com->getName() <<"' was not ready() or has no name." <<endlog();
                return false;
            }
            simplecommands[com->getName()] = com->getCommandImpl()->clone();
            this->add(com->getName(), 0);
            return true;
        }


        /**
         * Add a Command object, which takes no arguments, to the command interface.
         * The command is added to the C++ and to the scripting interface.
         * @param c A pointer to the existing command.
         * @param description A user readable description of what this command does.
         *
         * @return true if the command could be added.
         */
        template<class CommandT>
        bool addCommand( CommandT com, const char* description)
        {
            Logger::In in("CommandRepository");
            typedef typename boost::remove_pointer<CommandT>::type CommandVT;
            typedef typename boost::add_pointer<CommandVT>::type CommandPT;
            typedef typename CommandVT::Signature ComSig;
            BOOST_STATIC_ASSERT( boost::function_traits<typename CommandVT::Signature>::arity == 0 );

            CommandPT c = this->getpointer(com);
            internal::LocalCommand<ComSig>* lc = dynamic_cast<internal::LocalCommand<ComSig>*>( c->getCommandImpl() );
            // We can only add local commands.
            if ( !lc ) {
                log(Error) << "Failed to addCommand: '"<< c->getName() <<"' is not a local command." <<endlog();
                return false;
            }
            // First add the command to the normal interface.
            if ( this->addCommand( c ) == false )
                return false;
            // Next, add it to the Command from 'internal::DataSource' interface.
            this->add( c->getName(), new internal::OperationFactoryPart0<base::DispatchInterface*, internal::DataSourceArgsCommand<ComSig> >(
                  internal::DataSourceArgsCommand<ComSig>(lc->getCommandFunction(),
                                        lc->getConditionFunction(),
                                        lc->getCommandProcessor(), lc->isInverted() ), description) );
            return true;
        }

        /**
         * Add a Command object, which takes one argument, to the
         * command interface. The command is added to the C++ and to the
         * scripting interface.
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
            Logger::In in("CommandRepository");
            typedef typename boost::remove_pointer<CommandT>::type CommandVT;
            typedef typename boost::add_pointer<CommandVT>::type CommandPT;
            typedef typename CommandVT::Signature ComSig;
            BOOST_STATIC_ASSERT( boost::function_traits<typename CommandVT::Signature>::arity == 1 );

            CommandPT c = this->getpointer(com);
            internal::LocalCommand<ComSig>* lc = dynamic_cast<internal::LocalCommand<ComSig>*>( c->getCommandImpl() );
            if ( !lc ) {
                log(Error) << "Failed to addCommand: '"<< c->getName() <<"' is not a local command." <<endlog();
                return false;
            }
            if ( this->addCommand( c ) == false )
                return false;
            this->add( c->getName(), new internal::OperationFactoryPart1<base::DispatchInterface*, internal::DataSourceArgsCommand<ComSig> >(
                  internal::DataSourceArgsCommand<ComSig>(lc->getCommandFunction(),
                                        lc->getConditionFunction(),
                                        lc->getCommandProcessor(), lc->isInverted() ),
                  description, arg1, arg1_description) );
            return true;
        }

        /**
         * Add a Command object, which takes two arguments, to the
         * command interface.
         * The command is added to the C++ and to the scripting interface.
         *
         * @param c A pointer to the Command object.
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
            Logger::In in("CommandRepository");
            typedef typename boost::remove_pointer<CommandT>::type CommandVT;
            typedef typename boost::add_pointer<CommandVT>::type CommandPT;
            typedef typename CommandVT::Signature ComSig;
            BOOST_STATIC_ASSERT( boost::function_traits<typename CommandVT::Signature>::arity == 2 );

            CommandPT c = this->getpointer(com);
            internal::LocalCommand<ComSig>* lc = dynamic_cast<internal::LocalCommand<ComSig>*>( c->getCommandImpl() );
            if ( !lc ) {
                log(Error) << "Failed to addCommand: '"<< c->getName() <<"' is not a local command." <<endlog();
                return false;
            }
            if ( this->addCommand( c ) == false )
                return false;
            this->add( c->getName(), new internal::OperationFactoryPart2<base::DispatchInterface*, internal::DataSourceArgsCommand<ComSig> >(
                  internal::DataSourceArgsCommand<ComSig>(lc->getCommandFunction(),
                                        lc->getConditionFunction(),
                                        lc->getCommandProcessor(), lc->isInverted() ),
                  description, arg1, arg1_description,
                  arg2, arg2_description) );
            return true;
        }


        /**
         * Add a Command object, which takes three arguments, to the
         * command interface.
         * The command is added to the C++ and to the scripting interface.
         *
         * @param c A pointer to the Command object.
         * @param description A user readable description of what this command does.
         * @param arg1 A single word name of the first argument.
         * @param arg1_description A user readable description of the first argument.
         * @param arg2 A single word name of the second argument.
         * @param arg2_description A user readable description of the second argument.
         * @param arg3 A single word name of the third argument.
         * @param arg3_description A user readable description of the third argument.
         *
         * @return true if the command could be added.
         */
        template<class CommandT>
        bool addCommand( CommandT com, const char* description,
                         const char* arg1, const char* arg1_description,
                         const char* arg2, const char* arg2_description,
                         const char* arg3, const char* arg3_description)
        {
            Logger::In in("CommandRepository");
            typedef typename boost::remove_pointer<CommandT>::type CommandVT;
            typedef typename boost::add_pointer<CommandVT>::type CommandPT;
            typedef typename CommandVT::Signature ComSig;
            BOOST_STATIC_ASSERT( boost::function_traits<typename CommandVT::Signature>::arity == 3 );

            CommandPT c = this->getpointer(com);
            internal::LocalCommand<ComSig>* lc = dynamic_cast<internal::LocalCommand<ComSig>*>( c->getCommandImpl() );
            if ( !lc ) {
                log(Error) << "Failed to addCommand: '"<< c->getName() <<"' is not a local command." <<endlog();
                return false;
            }
            if ( this->addCommand( c ) == false )
                return false;
            this->add( c->getName(), new internal::OperationFactoryPart3<base::DispatchInterface*, internal::DataSourceArgsCommand<ComSig> >(
                  internal::DataSourceArgsCommand<ComSig>(lc->getCommandFunction(),
                                               lc->getConditionFunction(),
                                               lc->getCommandProcessor(), lc->isInverted() ),
                  description, arg1, arg1_description,
                  arg2, arg2_description,
                  arg3, arg3_description) );
            return true;
        }

        /**
         * Add a Command object, which takes four arguments, to the
         * command interface.
         * The command is added to the C++ and to the scripting interface.
         *
         * @param c A pointer to the Command object.
         * @param description A user readable description of what this command does.
         * @param arg1 A single word name of the first argument.
         * @param arg1_description A user readable description of the first argument.
         * @param arg2 A single word name of the second argument.
         * @param arg2_description A user readable description of the second argument.
         * @param arg3 A single word name of the third argument.
         * @param arg3_description A user readable description of the third argument.
         * @param arg4 A single word name of the fourth argument.
         * @param arg4_description A user readable description of the fourth argument.
         *
         * @return true if the command could be added.
         */
        template<class CommandT>
        bool addCommand( CommandT com, const char* description,
                         const char* arg1, const char* arg1_description,
                         const char* arg2, const char* arg2_description,
                         const char* arg3, const char* arg3_description,
                         const char* arg4, const char* arg4_description)
        {
            Logger::In in("CommandRepository");
            typedef typename boost::remove_pointer<CommandT>::type CommandVT;
            typedef typename boost::add_pointer<CommandVT>::type CommandPT;
            typedef typename CommandVT::Signature ComSig;
            BOOST_STATIC_ASSERT( boost::function_traits<typename CommandVT::Signature>::arity == 4 );

            CommandPT c = this->getpointer(com);
            internal::LocalCommand<ComSig>* lc = dynamic_cast<internal::LocalCommand<ComSig>*>( c->getCommandImpl() );
            if ( !lc ) {
                log(Error) << "Failed to addCommand: '"<< c->getName() <<"' is not a local command." <<endlog();
                return false;
            }
            if ( this->addCommand( c ) == false )
                return false;
            this->add( c->getName(), new internal::OperationFactoryPart4<base::DispatchInterface*, internal::DataSourceArgsCommand<ComSig> >(
                  internal::DataSourceArgsCommand<ComSig>(lc->getCommandFunction(),
                                               lc->getConditionFunction(),
                                               lc->getCommandProcessor(), lc->isInverted() ),
                  description, arg1, arg1_description,
                  arg2, arg2_description,
                  arg3, arg3_description,
                  arg4, arg4_description) );
            return true;
        }

        /**
         * For internal use only. Retrieve a previously added Command.
         *
         * @param name The name of the Command
         * @param args A vector of command arguments.
         *
         * @return A dispatchable object which is a new Command object.
         */
        base::DispatchInterface* getCommand( std::string name,
                                          const std::vector<base::DataSourceBase::shared_ptr>& args) const
        {
            return this->produce(name, args);
        }

        /**
         * For internal use only. The pointer of the object of which a member function
         * must be invoked is stored in a internal::DataSource such that the pointer can change
         * during program execution. Required in scripting for state machines.
         */
        template<class CommandT,class CompT>
        bool addCommandDS( internal::DataSource< boost::weak_ptr<CompT> >* wp, CommandT c, const char* description)
        {
            using namespace detail;
            typedef typename CommandT::Signature ComSig;
            if ( this->hasMember(c.getName() ) )
                return false;
            typedef scripting::FunctorDS0<ComSig> CommandF;
            typedef internal::DataSourceArgsCommand<ComSig,
                                  CommandF> DSComm;
            this->add( c.getName(), new internal::OperationFactoryPart0<base::DispatchInterface*, DSComm>(
                        DSComm( CommandF(wp, c.getCommandFunction()),
                                CommandF(wp, c.getConditionFunction()),
                                c.getCommandProcessor(), c.isInverted() ),
                        description) );
            return true;
        }

        /**
         * For internal use only. The pointer of the object of which a member function
         * must be invoked is stored in a internal::DataSource such that the pointer can change
         * during program execution. Required in scripting for state machines.
         */
        template<class CommandT, class CompT>
        bool addCommandDS( internal::DataSource<boost::weak_ptr<CompT> >* wp, CommandT c, const char* description,
                         const char* arg1, const char* arg1_description)
        {
            using namespace detail;
            typedef typename CommandT::Signature ComSig;
            typedef scripting::FunctorDS1<ComSig> CommandF;
            typedef internal::DataSourceArgsCommand<ComSig,
                                  CommandF> DSComm;
            if ( this->hasMember(c.getName() ) )
                return false;
            this->add( c.getName(), new internal::OperationFactoryPart1<base::DispatchInterface*, DSComm, typename DSComm::traits::arg2_type>(
                        DSComm( CommandF(wp, c.getCommandFunction()),
                                CommandF(wp, c.getConditionFunction()),
                                c.getCommandProcessor(), c.isInverted() ),
                        description, arg1, arg1_description) );
            return true;
        }

        /**
         * For internal use Only. Retrieve the completion condition of a previously added Command.
         *
         * @param name The name of the Command
         * @param args A vector of command arguments
         *
         * @return A condition which evaluates the command's completion.
         */
        base::ConditionInterface* getCondition( std::string name,
                                         const std::vector<base::DataSourceBase::shared_ptr>& args) const
        {
            base::DispatchInterface* di = this->produce(name, args);
            base::ConditionInterface* ret = di->createCondition();
            delete di;
            return ret;
        }

        /**
         * Create a internal::CommandC container object, which can be used
         * to access an added Command.
         *
         * @param name The name of the Command
         *
         * @return A new internal::CommandC object.
         */
        internal::CommandC create(std::string name) {
            return internal::CommandC( this, name );
        }


    };

}}


#endif
