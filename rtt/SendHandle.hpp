#ifndef ORO_CORELIB_SEND_HANDLE_HPP
#define ORO_CORELIB_SEND_HANDLE_HPP

#include "rtt-config.h"
#include "internal/CollectSignature.hpp"
#include "internal/CollectBase.hpp"
#include "internal/InvokerSignature.hpp"
#include "internal/InvokerBase.hpp"
#include "base/MethodBase.hpp"
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
          public internal::InvokerSignature<boost::function_traits<Signature>::arity,
                                          Signature,
                                          base::MethodBase<Signature>* >
	{
	private:
        typedef internal::CollectSignature<boost::function_traits<typename internal::CollectType<Signature>::Ft>::arity,
                                           typename internal::CollectType<Signature>::Ft,
                                           internal::CollectBase<Signature>* >      CBase;
        typedef internal::InvokerSignature<boost::function_traits<Signature>::arity,
                                           Signature,
                                           base::MethodBase<Signature>* >      IBase;
	public:
        /**
         * Create an empty SendHandle.
         */
		SendHandle() {}

        SendHandle(base::MethodBase<Signature>* coll) : CBase(coll), IBase(coll) {}

        /**
         * Create a copy-equivalent SendHandle.
         */
        SendHandle(const SendHandle& hs) : CBase(hs.cimpl), IBase(hs.impl) {}

        /**
         * No-op destructor, does not change signal/slot state.
         */
        ~SendHandle() {
		    //impl->dispose();
        }

        /**
         * Inspect if this SendHandle is pointing to a valid (existing) invocation.
         * @return false if no invocation is associated with this SendHandle.
         */
        operator bool() const { return this->impl != 0;}

        /**
         * Inspect if this SendHandle is pointing to valid (existing) invocation.
         * @return false if no invocation is associated with this handle.
         */
        bool ready() const { return this->impl != 0;}

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
