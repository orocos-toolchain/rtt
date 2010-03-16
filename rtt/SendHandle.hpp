#ifndef ORO_CORELIB_SEND_HANDLE_HPP
#define ORO_CORELIB_SEND_HANDLE_HPP

#include "rtt-config.h"
#include "Logger.hpp"
#include "internal/CollectSignature.hpp"
#include "internal/CollectBase.hpp"
#include "internal/ReturnSignature.hpp"
#include "internal/ReturnBase.hpp"
#include <boost/type_traits.hpp>

namespace RTT
{
    /**
     * @brief The SendHandle is used to collect the result values
     * of an asynchronous invocation. The template argument Signature
     * must have the same type as the method being invoked.
     */
    template<class Signature>
	class SendHandle
        : public internal::CollectSignature<boost::function_traits<typename internal::CollectType<Signature>::Ft>::arity,
                                            typename internal::CollectType<Signature>::Ft,
                                            internal::CollectBase<Signature>* >,
          public internal::ReturnSignature<boost::function_traits<Signature>::arity,
                                          Signature,
                                          typename internal::CollectBase<Signature>::shared_ptr >
	{
	public:
        typedef internal::CollectSignature<boost::function_traits<typename internal::CollectType<Signature>::Ft>::arity,
                                           typename internal::CollectType<Signature>::Ft,
                                           internal::CollectBase<Signature>* >      CBase;
        typedef internal::ReturnSignature<boost::function_traits<Signature>::arity,
                                           Signature,
                                           typename internal::CollectBase<Signature>::shared_ptr >      RBase;
        /**
         * Create an empty SendHandle.
         */
		SendHandle() {}

        SendHandle(typename internal::CollectBase<Signature>::shared_ptr coll) : CBase( coll.get() ), RBase(coll) {}

        /**
         * Create a copy-equivalent SendHandle.
         */
        SendHandle(const SendHandle& hs) : CBase(hs.cimpl), RBase(hs.impl) {}

        /**
         * No-op destructor, does not change signal/slot state.
         */
        ~SendHandle() {
		    //impl->dispose();
        }

        SendHandle& operator=(base::DisposableInterface* implementation)
        {
            if (this->RBase::impl && this->RBase::impl == implementation)
                return *this;
            this->RBase::impl.reset( boost::dynamic_pointer_cast< internal::ReturnBase<Signature> >(implementation) );
            if ( !this->RBase::impl && implementation ) {
                log(Error) << "Tried to assign SendHandle from incompatible type."<< endlog();
            }
            this->CBase::cimpl = dynamic_cast< internal::CollectBase<Signature>* >(implementation);
            if ( !this->CBase::cimpl && implementation ) {
                log(Error) << "Tried to assign SendHandle from incompatible type."<< endlog();
            }
            return *this;
        }

        /**
         * Inspect if this SendHandle is pointing to a valid (existing) invocation.
         * @return false if no invocation is associated with this SendHandle.
         */
        operator bool() const { return ready();}

        /**
         * Inspect if this SendHandle is pointing to valid (existing) invocation.
         * @return false if no invocation is associated with this handle.
         */
        bool ready() const { return this->CBase::cimpl && this->RBase::impl ;}

        using CBase::collect;

        /**
         * Collect this operator if the method has no arguments.
         */
        SendStatus collect()
        {
            if (this->impl)
                return this->impl->collect();
            return SendFailure;
        }
	protected:
	};
}
#endif
