#ifndef ORO_DLIB_COMMAND_HPP
#define ORO_DLIB_COMMAND_HPP

#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <boost/mem_fn.hpp>
#include <string>
#include "DispatchInterface.hpp"
#include "CommandFunctors.hpp"
#include "Invoker.hpp"

namespace RTT
{
    namespace DLib
    {
        /**
         * The DistributionLibrary (DL) Command wrapper class.  It
         * maps the Command C++ interface to the 'C' api.  The
         * implementation dispatches a Command through the
         * 'Distribution Library' to a remote CommandProcessor.  It
         * assumes the presense of a 'sendCommand' function which
         * takes the same arguments as the command.
         *
         * @param CommandT
         * The function signature of the command. For example, bool(
         * int, Frame, double)
         *
         */
        template<class CommandT, class ProtocolT>
        class DLibCommandImpl
            : public CommandBase<CommandT>
        {
        protected:
            int id;
        public:
            /**
             * The defaults are reset by the constructor.
             */
            DLibCommandImpl()
                : id(0)
            {
            }

            /** 
             * Call this operator if the DLibCommand takes no arguments.
             * 
             * @return true if ready and succesfully sent.
             */
            bool invoke() {
                if (id)
                    return ProtocolT::sendCommand(id);
                return false;
            }

            template<class T1>
            bool invoke( T1 a1 ) {
                if (id)
                    return ProtocolT::sendCommand(id, a1);
                return false;
            }

            template<class T1, class T2>
            bool invoke( T1 a1, T2 a2 ) {
                if (id)
                    return ProtocolT::sendCommand(id, a1, a2);
                return false;
            }

            template<class T1, class T2, class T3>
            bool invoke( T1 a1, T2 a2, T3 a3 ) {
                if (id)
                    return ProtocolT::sendCommand(id, a1, a2, a3);
                return false;
            }

            template<class T1, class T2, class T3, class T4>
            bool invoke( T1 a1, T2 a2, T3 a3, T4 a4 ) {
                if (id)
                    return ProtocolT::sendCommand(id, a1, a2, a4);
                return false;
            }
        };


        /**
         * A Command which is dispatched using the 'Distribution Library' to a CommandProcessor.
         * It inherits from the DLibCommandImpl class.
         * @param CommandT The function signature of the command. For example,
         * bool( int, Frame, double)
         * @param ProtocolT A class containing static functions which implement the
         * DLib Command Protocol.
         */
        template<class CommandT, class ProtocolT>
        class DLibCommand 
            : public Invoker<CommandT,DLibCommandImpl<CommandT,ProtocolT> >
        {
        public:
            typedef CommandT Signature;

            /** 
             * Create a DLibCommand object which executes a remote command
             * 
             * @param component The name of the target component.
             * @param name The name of this command.
             */
            DLibCommand(std::string component, std::string name)
            {
                this->id = ProtocolT::getCommandId(component,command);
                if (this->id == 0) {
                    log(Error) << "Could not find Component '"<<component <<"' or Command '"<<name"' in that component."<<endlog();
                }
            }

            virtual void readArguments() {}

            virtual bool ready() const {
                DispatchInterface::Status st = ProtocolT::getCommandStatus(this->id);
                return s == DispatchInterface::Ready || s == DispatchInterface::Done;
            }

            virtual bool dispatch() {
                return false;
            }

            virtual bool execute() {
                return false;
            }
        
            virtual bool done() const {
                return ProtocolT::getCommandStatus(this->id) == DispatchInterface::Done;
            }
     
            virtual void reset() {
                return ProtocolT::resetCommand(this->id);
            }

            virtual bool sent() const {
                return ProtocolT::getCommandStatus(this->id) >= DispatchInterface::Sent;
            }

            virtual bool accepted() const {
                return ProtocolT::getCommandStatus(this->id) >= DispatchInterface::Accepted;
            }

            virtual bool executed() const {
                return ProtocolT::getCommandStatus(this->id) >= DispatchInterface::Executed;
            }

            virtual bool valid() const {
                return getCommandStatus(this->id) >= DispatchInterface::Valid;
            }

            virtual ConditionInterface* createCondition() const
            {
                return new detail::ConditionFunctor<bool(void)>( boost::bind<bool>( boost::mem_fn(&DLibCommand::done), this) );
            }

            /** 
             * Creates a clone of this DLibCommand object.
             * Use this method to get a new command object
             * which has its own state information.
             * 
             * @return 
             */
            virtual DispatchInterface* clone() const {
                return new DLibCommand(*this);
            }

            virtual CommandBase<CommandT>* cloneI() const {
                return new DLibCommand(*this);
            }
        };
    }
}
#endif
