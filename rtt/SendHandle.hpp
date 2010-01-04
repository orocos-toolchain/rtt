#ifndef ORO_CORELIB_SEND_HANDLE_HPP
#define ORO_CORELIB_SEND_HANDLE_HPP

#include "rtt-config.h"
#include "internal/CollectSignature.hpp"
#include "internal/CollectBase.hpp"
#include <boost/type_traits.hpp>

namespace RTT
{
    /**
     * @brief The SendHandle is used to collect the result values
     * of an asynchronous invocation.
     */
    template<class Signature>
	class SendHandle
        : public internal::CollectSignature<boost::function_traits<Signature>::arity, Signature, internal::CollectBase<Signature>* >
	{
	private:
		typedef internal::CollectBase<Signature>*      collector_t; //! the collection object
	public:
        /**
         * Create an empty SendHandle.
         */
		SendHandle();

        SendHandle(collector_t coll);

        /**
         * Create a copy-equivalent SendHandle.
         */
        SendHandle(const SendHandle& hs);

        /**
         * No-op destructor, does not change signal/slot state.
         */
		~SendHandle() {
		    //m_coll->dispose();
		}

        /**
         * Inspect if this SendHandle is pointing to a valid (existing) invocation.
         * @return false if no invocation is associated with this SendHandle.
         */
        operator bool() const;

        /**
         * Inspect if this SendHandle is pointing to valid (existing) invocation.
         * @return false if no invocation is associated with this handle.
         */
        bool ready() const;

        SendStatus collect() {
            return m_coll->collect();
        }

        template<class A1>
        SendStatus collect(A1 a1) {
            return m_coll->collect(a1);
        }

        SendStatus collectIfDone() {
            return m_coll->collectIfDone();
        }

        template<class A1>
        SendStatus collectIfDone(A1 a1) {
            return m_coll->collectIfDone(a1);
        }

	protected:
        /**
         * This is actually a smart pointer which always
         * points to an existing connection object.
         */
        collector_t  m_coll;
        ExecutionEngine* m_ee;
	};
}
#endif
