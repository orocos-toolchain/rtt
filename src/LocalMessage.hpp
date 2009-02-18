
#ifndef ORO_LOCAL_MESSAGE_HPP
#define ORO_LOCAL_MESSAGE_HPP

#include <boost/function.hpp>
#include <string>
#include "Invoker.hpp"
#include "MessageBase.hpp"
#include "MessageStorage.hpp"
#include "MessageProcessor.hpp"

namespace RTT
{
    namespace detail
    {
        template<class FunctionT>
        class LocalMessageImpl
            : public MessageBase<FunctionT>
        {
        protected:
            MessageStorage<FunctionT> mmesg;
            MessageProcessor* mmp;
        public:
            typedef FunctionT Signature;
            typedef boost::function_traits<Signature> traits;
            typedef bool result_type;

            bool execute() {
                mmesg.exec();
                return true;
            }

            /**
             * Invoke this operator if the message has no arguments.
             */
            result_type invoke()
            {
                LocalMessageBase<FunctionT>* c = this->clone();
                return mmp->process(this);
            }

            /**
             * Invoke this operator if the message has one argument.
             */
            template<class T1>
            result_type invoke(T1 t)
            {
                LocalMessageImpl<FunctionT>* c = this->cloneM(); // RT-clone
                c->mmesg.store(t);
                return mmp->process( c );
            }

            /**
             * Invoke this operator if the message has two arguments.
             */
            template<class T1, class T2>
            result_type invoke(T1 t1, T2 t2)
            {
                return mmesg(t1, t2);
            }

            /**
             * Invoke this operator if the message has three arguments.
             */
            template<class T1, class T2, class T3>
            result_type invoke(T1 t1, T2 t2, T3 t3)
            {
                return mmesg(t1, t2, t3);
            }

            /**
             * Invoke this operator if the message has four arguments.
             */
            template<class T1, class T2, class T3, class T4>
            result_type invoke(T1 t1, T2 t2, T3 t3, T4 t4)
            {
                return mmesg(t1, t2, t3, t4);
            }

        };

        /**
         * A message which executes a local function.
         *
         * Usage:
         @code
         LocalMessage<double(int, double)> mymeth( &Class::foo, &c);
         double result = mymeth( 3, 1.9);
         @endcode
        */
        template<class FunctionT>
        struct LocalMessage
            : public Invoker<FunctionT,LocalMessageImpl<FunctionT> >
        {
            typedef FunctionT Signature;
            typedef bool result_type;
            typedef boost::function_traits<Signature> traits;

            /**
             * Construct a LocalMessage from a member function pointer
             * and an object derived from TaskContext.
             *
             * @param mesg A pointer to a class member function
             * @param object An object of the class which has \a mesg as member function
             * and which is a subclass of TaskContext.
             */
            template<class M, class ObjectType>
            LocalMessage(M mesg, ObjectType object)
            {
                this->mmesg = detail::MessageBinder<Signature>()(mesg, object);
                this->mmp = object->engine()->messages();
            }

            /**
             * Construct a LocalMessage from a member function pointer and
             * object to be executed in a TaskContext.
             *
             * @param mesg A pointer to a class member function
             * @param object An object of the class which has \a mesg as member function
             * and which is a subclass of TaskContext.
             * @param taskc The TaskContext in which this message must be executed.
             */
            template<class M, class ObjectType>
            LocalMessage(M mesg, ObjectType object, TaskContext* taskc)
            {
                this->mmesg = detail::MessageBinder<Signature>()(mesg, object);
                this->mmp = taskc->engine()->messages();
            }

            /**
             * Construct a LocalMessage from a function pointer or function object.
             *
             * @param mesg a pointer to a function or function object.
             * @param taskc The TaskContext in which to execute this message
             */
            template<class M>
            LocalMessage(M mesg, TaskContext* taskc)
            {
                this->mmesg = mesg;
                this->mmp = taskc->engine()->messages();
            }

            boost::function<Signature> getMessageFunction() const
            {
                return this->mmesg;
            }

            ActionInterface* clone() const
            {
                return new LocalMessage<Signature>(*this);
            }

            MessageBase<Signature>* cloneI() const
            {
                return new LocalMessage<Signature>(*this);
            }
        };
    }
}

#endif
