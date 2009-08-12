/***************************************************************************
  tag: FMTC  do nov 2 13:06:09 CET 2006  RemoteCommand.hpp

                        RemoteCommand.hpp -  description
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


#ifndef ORO_REMOTE_COMMAND_HPP
#define ORO_REMOTE_COMMAND_HPP

#include <boost/function.hpp>
#include <string>
#include "../base/DispatchInterface.hpp"
#include "CommandProcessor.hpp"
#include "CommandFunctors.hpp"
#include "../base/CommandBase.hpp"
#include "DataSourceStorage.hpp"
#include "Invoker.hpp"

namespace RTT
{
    namespace detail
    {
        /**
         * A Command which is dispatched remotely to a CommandProcessor.
         * @param CommandT The function signature of the command. For example,
         * bool( int, Frame, double)
         *
         */
        template<class CommandT>
        class RemoteCommandImpl
            : public CommandBase<CommandT>,
              protected detail::DataSourceStorage<CommandT>
        {
        protected:
            CommandC mcom;
        public:
            /**
             * The defaults are reset by the constructor.
             */
            RemoteCommandImpl()
                : mcom()
            {}

            /**
             * Call this operator if the RemoteCommand takes no arguments.
             *
             * @return true if ready and succesfully sent.
             */
            bool invoke() {
                return mcom.execute();
            }

            template<class T1>
            bool invoke( T1 a1 ) {
                this->store( a1 );
                return mcom.execute();
            }

            template<class T1, class T2>
            bool invoke( T1 a1, T2 a2 ) {
                this->store( a1, a2 );
                return mcom.execute();
            }

            template<class T1, class T2, class T3>
            bool invoke( T1 a1, T2 a2, T3 a3 ) {
                this->store( a1, a2, a3 );
                return mcom.execute();
            }

            template<class T1, class T2, class T3, class T4>
            bool invoke( T1 a1, T2 a2, T3 a3, T4 a4 ) {
                this->store( a1, a2, a3, a4 );
                return mcom.execute();
            }
        };


        /**
         * A Command which is dispatched remotely to a CommandProcessor.
         * @param CommandT The function signature of the command. For example,
         * bool( int, Frame, double)
         *
         */
        template<class CommandT>
        class RemoteCommand
            : public Invoker<CommandT,RemoteCommandImpl<CommandT> >
        {
        public:
            typedef CommandT Signature;

            /**
             * Create a RemoteCommand object which executes a remote command
             *
             * @param name The name of this command.
             * @param com The OperationFactory for commands.
             */
            RemoteCommand(CommandFactory* of, std::string name)
            {
                // create the command.
                this->mcom = CommandC(of, name);
                // add the arguments to the command.
                this->initArgs( this->mcom );
            }

            virtual void readArguments() {}

            virtual bool ready() const {
                return this->mcom.ready();
            }

            virtual bool dispatch() {
                return this->mcom.execute();
            }

            virtual bool execute() {
                return this->mcom.execute();
            }

            virtual bool done() const {
                return this->mcom.done();
            }

            virtual void reset() {
                return this->mcom.reset();
            }

            virtual bool sent() const {
                return this->mcom.sent();
            }

            virtual bool accepted() const {
                return this->mcom.accepted();
            }

            virtual bool executed() const {
                return this->mcom.executed();
            }

            virtual bool valid() const {
                return this->mcom.valid();
            }

            virtual DispatchInterface::Status status() const {
                return this->mcom.status();
            }

            virtual ConditionInterface* createCondition() const
            {
                return new detail::ConditionFunctor<bool(void)>( boost::bind<bool>( boost::mem_fn(&RemoteCommand::done), this) );
            }

            /**
             * Creates a clone of this RemoteCommand object.
             * Use this method to get a new command object
             * which has its own state information.
             *
             * @return
             */
            virtual DispatchInterface* clone() const {
                return new RemoteCommand(*this);
            }

            virtual CommandBase<CommandT>* cloneI() const {
                return new RemoteCommand(*this);
            }
        };
    }
}
#endif
