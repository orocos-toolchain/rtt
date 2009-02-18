

#ifndef ORO_TASK_MESSAGE_HPP
#define ORO_TASK_MESSAGE_HPP

#include <boost/function.hpp>
#include <string>
#include "UnMember.hpp"
#include "MessageBase.hpp"
#include "LocalMessage.hpp"
#include "ActionInterface.hpp"
#include "Logger.hpp"

namespace RTT
{
    /**
     * @defgroup Messages Message Interface
     * Sending asynchronous messages.
     * @ingroup RTTComponentInterface
     */


    /**
     * A message which executes a function.
     *
     * Usage:
     @code
     Message<double(int, double)> mymeth("name", &Class::foo, &c);
     double result = mymeth( 3, 1.9);
     @endcode
     * @ingroup RTTComponentInterface
     * @ingroup Messages
     */
    template<class FunctionT>
    class Message
        : public detail::InvokerSignature<boost::function_traits<FunctionT>::arity,
                                          FunctionT,
                                          boost::shared_ptr< detail::MessageBase<FunctionT> > >
    {
        std::string mname;
        typedef detail::InvokerSignature<boost::function_traits<FunctionT>::arity,
                                         FunctionT,
                                         boost::shared_ptr< detail::MessageBase<FunctionT> > > Base;
    public:
        typedef FunctionT Signature;
        typedef typename boost::function_traits<Signature>::result_type result_type;
        typedef boost::function_traits<Signature> traits;
        typedef boost::shared_ptr< detail::MessageBase<FunctionT> > MessageBasePtr;

        /**
         * Create an empty Message object.
         * Use assignment to initialise it.
         * @see message
         */
        Message()
            : Base(), mname()
        {}

        /**
         * Create an empty Message object.
         * Use assignment to initialise it.
         * @see message
         */
        Message(std::string name)
            : Base(), mname(name)
        {}

        /**
         * Message objects may be copied.
         *
         * @param m the original
         */
        Message(const Message& m)
            : Base(m.impl),
              mname(m.mname)
        {}

        /**
         * Message objects may be assigned
         *
         * @param m the original
         *
         * @return *this
         */
        Message& operator=(const Message& m)
        {
            if ( this == &m )
                return *this;
            mname = m.mname;
            this->impl = m.impl;
            return *this;
        }

        /**
         * Initialise a nameless Message object from an implementation.
         *
         * @param implementation The implementation which is acquired
         * by the Message object. If it has the wrong type, it is freed.
         */
        Message(boost::shared_ptr<ActionInterface> implementation)
            : Base( boost::dynamic_pointer_cast< detail::MessageBase<Signature> >(implementation) ),
              mname()
        {
            if ( !this->impl && implementation ) {
                log(Error) << "Tried to construct Message from incompatible type."<< endlog();
            }
        }

        /**
         * Message objects may be assigned to an implementation.
         *
         * @param implementation the implementation, if it is not suitable,
         * it is freed.
         *
         * @return *this
         */
        Message& operator=(boost::shared_ptr<ActionInterface> implementation)
        {
            if (this->impl && this->impl == implementation)
                return *this;
            this->impl = boost::dynamic_pointer_cast< detail::MessageBase<Signature> >(implementation);
            if ( !this->impl && implementation ) {
                log(Error) << "Tried to assign Message '"<<mname<<"' from incompatible type."<< endlog();
            }
            return *this;
        }

        /**
         * Construct a Message from a class member pointer and an
         * object of that class.
         *
         * @param name The name of this message
         * @param meth A pointer to a class member function
         * @param object An object of the class which has \a meth as member function.
         */
        template<class M, class ObjectType>
        Message(std::string name, M meth, ObjectType object)
            : Base( MessageBasePtr(new detail::LocalMessage<Signature>(meth, object) ) ),
              mname(name)
        {}

        /**
         * Construct a Message from a function pointer or function object.
         *
         * @param name the name of this message
         * @param meth an pointer to a function or function object.
         */
        template<class M>
        Message(std::string name, M meth)
            : Base( MessageBasePtr(new detail::LocalMessage<Signature>(meth) ) ),
              mname(name)
        {}

        /**
         * Clean up the Message object.
         */
        ~Message()
        {
        }

        /**
         * Check if this Message is ready for execution.
         *
         * @return true if so.
         */
        bool ready() const {
            return this->impl;
        }


        /**
         * Get the name of this message.
         */
        const std::string& getName() const {return mname;}

        /**
         * Returns the internal implementation of the Message object.
         */
        const MessageBasePtr getMessageImpl() const {
            return this->impl;
        }

        /**
         * Sets the internal implementation of the Message object.
         */
        void setMessageImpl( MessageBasePtr new_impl) const {
            this->impl = new_impl;
        }
    };

    /**
     * Create a Message which executes a function locally.
     *
     * @param name The name of the resulting Message object
     * @param message A pointer to a member function to be executed.
     * @param object A pointer to the object which has the above member function.
     */
    template<class F, class O>
    Message< typename detail::UnMember<F>::type > message(std::string name, F message, O object) {
        return Message<  typename detail::UnMember<F>::type >(name, message, object);
    }

    /**
     * Create a Message which executes a function locally.
     *
     * @param name The name of the resulting Message object
     * @param message A pointer to a function to be executed.
     */
    template<class F>
    Message<F> message(std::string name, F message) {
        return Message<F>(name, message);
    }
    /**
     * Create a Message which executes a function locally.
     *
     * @param name The name of the resulting Message object
     * @param message A pointer to a function to be executed.
     */
    template<class F>
    Message< typename detail::ArgMember<F>::type > message_ds(std::string name, F message) {
        return Message<  typename detail::ArgMember<F>::type >(name, message);
    }
}

#endif
