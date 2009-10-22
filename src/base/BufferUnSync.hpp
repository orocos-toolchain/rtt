
#ifndef ORO_CORELIB_BUFFER_UNSYNC_HPP
#define ORO_CORELIB_BUFFER_UNSYNC_HPP

#include "BufferInterface.hpp"
#include <deque>

namespace RTT
{ namespace base {


    /**
     * Implements a \b not threadsafe buffer. Only use when no more than one
     * thread accesses this buffer at a time.
     *
     * @see BufferLockFree, BufferUnSync
     * @ingroup Ports
     */
    template<class T>
    class BufferUnSync
        :public BufferInterface<T>
    {
    public:
        typedef typename ReadInterface<T>::reference_t reference_t;
        typedef typename WriteInterface<T>::param_t param_t;
        typedef typename BufferInterface<T>::size_type size_type;
        typedef T value_t;

        /**
         * Create a buffer of size \a size.
         */
        BufferUnSync( size_type size, const T& initial_value = T() )
            : cap(size), buf()
        {
            data_sample(initial_value);
        }

        virtual void data_sample( const T& sample )
        {
            buf.resize(cap, sample);
            buf.resize(0);
        }

        /**
         * Destructor
         */
        ~BufferUnSync() {}

        bool Push( param_t item )
        {
            if (cap == (size_type)buf.size() ) {
                return false;
            }
            buf.push_back( item );
            return true;
        }

        size_type Push(const std::vector<T>& items)
        {
            typename std::vector<T>::const_iterator itl( items.begin() );
            while ( ((size_type)buf.size() != cap) && (itl != items.end()) ) {
                buf.push_back( *itl );
                ++itl;
            }
            return (itl - items.begin());
        }
        bool Pop( reference_t item )
        {
            if ( buf.empty() ) {
                return false;
            }
            item = buf.front();
            buf.pop_front();
            return true;
        }

        size_type Pop(std::vector<T>& items )
        {
            int quant = 0;
            while ( !buf.empty() ) {
                items.push_back( buf.front() );
                buf.pop_front();
                ++quant;
            }
            return quant;
        }

        value_t front() const
        {
            value_t item = value_t();
            if ( !buf.empty() )
                item = buf.front();
            return item;
        }

        size_type capacity() const {
            return cap;
        }

        size_type size() const {
            return buf.size();
        }

        void clear() {
            buf.clear();
        }

        bool empty() const {
            return buf.empty();
        }

        bool full() const {
            return (size_type)buf.size() ==  cap;
        }
    private:
        size_type cap;
        std::deque<T> buf;
    };
}}

#endif // BUFFERSIMPLE_HPP
