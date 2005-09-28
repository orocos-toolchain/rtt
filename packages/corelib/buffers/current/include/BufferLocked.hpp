


#ifndef ORO_CORELIB_BUFFER_LOCKED_HPP
#define ORO_CORELIB_BUFFER_LOCKED_HPP

#include <os/Mutex.hpp>
#include <os/MutexLock.hpp>
#include "BufferInterface.hpp"

namespace ORO_CoreLib
{

    /**
     * Implements a very simple blocking threadsafe buffer, using mutexes (locks).
     *
     * @see BufferLockFree
     */
    template<class T>
    class BufferLocked
        :public BufferInterface<T>
    {
    public:

        typedef typename ReadInterface<T>::reference_t reference_t;
        typedef typename WriteInterface<T>::param_t param_t;
        typedef typename BufferInterface<T>::size_t size_t;
        typedef T value_t;

        /**
         * Create a buffer of size \a size.
         */
        BufferLocked( size_t size )
            : buf()
        {
            buf.reserve(size);
        }

        /**
         * Destructor
         */
        ~BufferLocked() {}
        
        bool Push( param_t item )
        {
            ORO_OS::MutexLock locker(lock);
            if ( buf.capacity() == buf.size() )
                return false;
            buf.push_back( item );
            return true;
        }

        size_t Push(const std::vector<T>& items)
        {
            ORO_OS::MutexLock locker(lock);
            typename std::vector<T>::const_iterator itl( items.begin() );
            while ( size() != buf.capacity() && itl != items.end() ) {
                buf.push_back( *itl );
                ++itl;
            }
            return (itl - items.begin());
                
        }
        bool Pop( reference_t item )
        {
            ORO_OS::MutexLock locker(lock);
            if ( buf.empty() )
                return false;
            item = buf.front();
            buf.erase( buf.begin() );
            return true;
        }

        size_t Pop(std::vector<T>& items )
        {
            ORO_OS::MutexLock locker(lock);
            int quant = 0;
            while ( !buf.empty() ) {
                items.push_back( buf.front() );
                buf.erase( buf.begin() );
                ++quant;
            }
            return quant;
        }

        size_t capacity() const {
            ORO_OS::MutexLock locker(lock);
            return buf.capacity();
        }

        size_t size() const {
            ORO_OS::MutexLock locker(lock);
            return buf.size();
        }

    private:
        std::vector<T> buf;
        mutable ORO_OS::Mutex lock;

    };
}

#endif // BUFFERSIMPLE_HPP
