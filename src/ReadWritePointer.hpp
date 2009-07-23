#ifndef RTT_READ_WRITE_POINTER_HPP
#define RTT_READ_WRITE_POINTER_HPP

#include <boost/intrusive_ptr.hpp>
#include <boost/call_traits.hpp>

#include "os/Mutex.hpp"
#include "os/MutexLock.hpp"

namespace RTT
{
    namespace details
    {
        template<typename T>
        struct ROPtrInternal
        {
            OS::Mutex  lock;
            T*     value;
            size_t readers;

            ROPtrInternal(T* value)
                : value(value), readers(0) {}
            ~ROPtrInternal() { delete value; }

            void ref()
            { OS::MutexLock do_lock(lock);
                ++readers;
            }
            bool deref()
            { OS::MutexLock do_lock(lock);
                return (--readers);
            }
        };
        template<typename T>
        void intrusive_ptr_add_ref(typename RTT::details::ROPtrInternal<T>* data)
        {
            data->ref();
        }
        template<typename T>
        void intrusive_ptr_release(typename RTT::details::ROPtrInternal<T>* data)
        {
            if (!data->deref())
                delete data;
        }
    }

    template<typename T>
    class ReadOnlyPointer
    {
        typedef details::ROPtrInternal<T> Internal;
        boost::intrusive_ptr<Internal> internal;
        typedef boost::call_traits<T> traits;

    public:
        ReadOnlyPointer(T* ptr)
            : internal(new Internal(ptr)) {}

        typename traits::const_reference operator *() const { return *(internal->value); }
        T* operator ->() const { return internal->value; }

        bool valid() const
        { return internal; }

        T* write_access()
        {
            boost::intrusive_ptr<Internal> safe = this->internal;
            if (!safe)
                return 0;

            T* value = 0;
            { OS::MutexLock do_lock(safe->lock);
                if (safe->readers == 2)
                { // we're the only owner (don't forget the one just above ...).
                  // Just promote the current copy
                    std::swap(value, safe->value);
                }
                else
                { // there are other owners, do a copy
                    value = new T(*safe->value);
                }
            }

            internal.reset();
            return value;
        }
    };
}

#endif

