#ifndef ORO_CORELIB_BUFFER_POLICY_HPP
#define ORO_CORELIB_BUFFER_POLICY_HPP
#include <os/Semaphore.hpp>

namespace ORO_CoreLib
{

    /**
     * Use this policy to indicate that you \b do \b not want
     * to block on an \a empty or \a full buffer, queue, fifo,...
     */
    struct NonBlockingPolicy
    {
        NonBlockingPolicy(unsigned int ) {}
        void push(int /*i*/ = 1 ) {
        }
        void pop(int /*i*/ = 1 ) {
        }
        void reset( int /*i*/ ) {
        }
    };

    /**
     * Use this policy to indicate that you \b do want
     * to block on an \a empty or \a full buffer, queue, fifo,...
     */
    struct BlockingPolicy
    {
        BlockingPolicy(unsigned int c) : count(c) {}
        void push(int add = 1) {
            while (add-- > 0) {
                count.signal();
            }
        }
        void pop(int sub = 1) {
            while (sub-- > 0) {
                count.wait();
            }
        }
        void reset( int c ) {
            while( c > count.value() )
                count.signal();
            while( c < count.value() )
                count.wait();
        }
    private:
        ORO_OS::Semaphore count;
    };
}
#endif
