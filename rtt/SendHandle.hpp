#ifndef ORO_CORELIB_SEND_HANDLE_HPP
#define ORO_CORELIB_SEND_HANDLE_HPP

#include "rtt-config.h"
#include "Logger.hpp"
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
	public:
        typedef internal::CollectSignature<boost::function_traits<typename internal::CollectType<Signature>::Ft>::arity,
                                           typename internal::CollectType<Signature>::Ft,
                                           internal::CollectBase<Signature>* >      CBase;
        typedef internal::InvokerSignature<boost::function_traits<Signature>::arity,
                                           Signature,
                                           base::MethodBase<Signature>* >      IBase;
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

        SendHandle& operator=(base::DisposableInterface* implementation)
        {
            if (this->IBase::impl && this->IBase::impl == implementation)
                return *this;
            this->IBase::impl = dynamic_cast< base::MethodBase<Signature>* >(implementation);
            if ( !this->IBase::impl && implementation ) {
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
        bool ready() const { return this->CBase::cimpl != 0 && this->IBase::impl != 0;}

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
