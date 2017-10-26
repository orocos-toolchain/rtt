/***************************************************************************
  tag: Peter Soetens  Mon Jan 19 14:11:26 CET 2004  DataObjectLockFree.hpp

                        DataObjectLockFree.hpp -  description
                           -------------------
    begin                : Mon January 19 2004
    copyright            : (C) 2004 Peter Soetens
    email                : peter.soetens@mech.kuleuven.ac.be

 ***************************************************************************
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU General Public                   *
 *   License as published by the Free Software Foundation;                 *
 *   version 2 of the License.                                             *
 *                                                                         *
 *   As a special exception, you may use this file as part of a free       *
 *   software library without restriction.  Specifically, if other files   *
 *   instantiate templates or use macros or inline functions from this     *
 *   file, or you compile this file and link it with other files to        *
 *   produce an executable, this file does not by itself cause the         *
 *   resulting executable to be covered by the GNU General Public          *
 *   License.  This exception does not however invalidate any other        *
 *   reasons why the executable file might be covered by the GNU General   *
 *   Public License.                                                       *
 *                                                                         *
 *   This library is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU     *
 *   Lesser General Public License for more details.                       *
 *                                                                         *
 *   You should have received a copy of the GNU General Public             *
 *   License along with this library; if not, write to the Free Software   *
 *   Foundation, Inc., 59 Temple Place,                                    *
 *   Suite 330, Boston, MA  02111-1307  USA                                *
 *                                                                         *
 ***************************************************************************/

#ifndef CORELIB_DATAOBJECT_LOCK_FREE_HPP
#define CORELIB_DATAOBJECT_LOCK_FREE_HPP


#include "../os/oro_arch.h"
#include "DataObjectInterface.hpp"
#include "../Logger.hpp"
#include "../types/Types.hpp"
#include "../internal/DataSourceTypeInfo.hpp"

namespace RTT
{ namespace base {

    /**
     * @brief This DataObject is a Lock-Free implementation,
     * such that reads and writes can happen concurrently without priority
     * inversions.
     *
     * When there are more writes than reads, the last write will
     * be returned. The internal buffer can get full if too many
     * concurrent reads are taking to long. In that case, each new
     * read will read the element the previous read returned.
     *
     * @verbatim
     * The following Truth table applies when a Low Priority thread is
     * preempted by a High Priority thread :
     *
     *   L\H | Set | Get |
     *   Set | Ok  | Ok  |
     *   Get | Ok  | Ok  |
     *
     * legend : L : Low Priority thread
     *          H : High Priority thread
     *          Blk: Blocks High Priority thread (bad!)
     *          internal::NA : Not allowed !
     * @endverbatim
     * Further, multiple reads may occur before, during and after
     * a write operation simultaneously. The buffer needs readers+2*writers
     * elements to be guaranteed non blocking.
     * @ingroup PortBuffers
     */
    template<class T>
    class DataObjectLockFree
        : public DataObjectInterface<T>
    {
    public:
        typedef typename DataObjectInterface<T>::value_t value_t;
        typedef typename DataObjectInterface<T>::reference_t reference_t;
        typedef typename DataObjectInterface<T>::param_t param_t;

        typedef typename DataObjectBase::Options Options;

        /**
         * @brief The maximum number of threads.
         */
        const unsigned int MAX_THREADS;

    private:
        /**
         * Conversion of number of threads to size of buffer.
         */
        const unsigned int BUF_LEN; // = MAX_THREADS+2

        /**
         * Internal buffer structure.
         * Both the read and write pointers pointing to this struct
         * must be declared volatile, since they are modified in other threads.
         * I did not declare data as volatile,
         * since we only read/write it in secured buffers.
         */
        struct DataBuf {
            DataBuf()
                : data(), status(NoData), counter(), next()
            {
                oro_atomic_set(&counter, 0);
            }
            value_t data;
            FlowStatus status;
            mutable oro_atomic_t counter;
            DataBuf* next;
        };

        typedef DataBuf* volatile VolPtrType;
        typedef DataBuf  ValueType;
        typedef DataBuf* PtrType;

        VolPtrType read_ptr;
        VolPtrType write_ptr;

        /**
         * A 3 element Data buffer
         */
        DataBuf* data;

        bool initialized;

    public:

        /**
         * Construct an uninitialized DataObjectLockFree.
         * @param max_threads The maximum number of threads accessing this DataObject.
         */
        DataObjectLockFree( const Options &options = Options() )
            : MAX_THREADS(options.max_threads()), BUF_LEN( options.max_threads() + 2),
              read_ptr(0),
              write_ptr(0),
              initialized(false)
        {
        	data = new DataBuf[BUF_LEN];
        	read_ptr = &data[0];
        	write_ptr = &data[1];
        }

        /**
         * Construct a DataObjectLockFree.
         * @param initial_value The initial value of this DataObject.
         * @param max_threads The maximum number of threads accessing this DataObject.
         * @note You have to specify the maximum number of threads explicitly
         */
        DataObjectLockFree( param_t initial_value, const Options &options = Options() )
            : MAX_THREADS(options.max_threads()), BUF_LEN( options.max_threads() + 2),
              read_ptr(0),
              write_ptr(0),
              initialized(false)
        {
            data = new DataBuf[BUF_LEN];
            read_ptr = &data[0];
            write_ptr = &data[1];
            data_sample(initial_value);
        }

        ~DataObjectLockFree() {
            delete[] data;
        }

        /**
         * Get a copy of the data.
         * This method will allocate memory twice if data is not a value type.
         * Use Get(reference_t) for the non-allocating version.
         *
         * @return A copy of the data.
         */
        virtual value_t Get() const {
            value_t cache = value_t();
            Get(cache);
            return cache;
        }

        /**
         * Get a copy of the Data (non allocating).
         * If pull has reserved enough memory to store the copy,
         * no memory will be allocated.
         *
         * @param pull A copy of the data.
         * @param copy_old_data If true, also copy the data if the data object
         *                      has not been updated since the last call.
         * @param copy_sample   If true, copy the data unconditionally.
         */
        virtual FlowStatus Get( reference_t pull, bool copy_old_data, bool copy_sample ) const
        {
            if (!initialized && !copy_sample) {
                return NoData;
            }

            PtrType reading;
            // loop to combine Read/Modify of counter
            // This avoids a race condition where read_ptr
            // could become write_ptr ( then we would read corrupted data).
            do {
                reading = read_ptr;            // copy buffer location
                oro_atomic_inc(&reading->counter); // lock buffer, no more writes
                // XXX smp_mb
                if ( reading != read_ptr )     // if read_ptr changed,
                    oro_atomic_dec(&reading->counter); // better to start over.
                else
                    break;
            } while ( true );
            // from here on we are sure that 'reading'
            // is a valid buffer to read from.

            FlowStatus result = reading->status;
            if (result == NewData) {
                pull = reading->data;               // takes some time
                reading->status = OldData;          // CAS?
            } else if (((result == OldData) && copy_old_data) || copy_sample) {
                pull = reading->data;               // takes some time
            }

            // XXX smp_mb
            oro_atomic_dec(&reading->counter);       // release buffer
            return result;
        }

        /**
         * Get a copy of the Data (non allocating).
         * If pull has reserved enough memory to store the copy,
         * no memory will be allocated.
         *
         * @param pull A copy of the data.
         * @param copy_old_data If true, also copy the data if the data object
         *                      has not been updated since the last call.
         */
        virtual FlowStatus Get( reference_t pull, bool copy_old_data = true ) const
        {
            return Get( pull, copy_old_data, /* copy_sample = */ false );
        }

        /**
         * Set the data to a certain value (non blocking).
         *
         * @param push The data which must be set.
         */
        virtual bool Set( param_t push )
        {
            /**
             * This method can not be called concurrently (only one
             * producer). With a minimum of 3 buffers, if the
             * write_ptr+1 field is not occupied, it will remain so
             * because the read_ptr is at write_ptr-1 (and can
             * not increment the counter on write_ptr+1). Hence, no
             * locking is needed.
             */

            if (!initialized) {
                log(Error) << "You set a lock-free data object of type " << internal::DataSourceTypeInfo<T>::getType() << " without initializing it with a data sample. "
                           << "This might not be real-time safe." << endlog();
                data_sample(value_t(), true);
            }

            // writeout in any case
            PtrType writing = write_ptr;
            writing->data = push;
            writing->status = NewData;

            // if next field is occupied (by read_ptr or counter),
            // go to next and check again...
            while ( oro_atomic_read( &write_ptr->next->counter ) != 0 || write_ptr->next == read_ptr )
                {
                    write_ptr = write_ptr->next;
                    if (write_ptr == writing)
                        return false; // nothing found, to many readers !
                }

            // we will be able to move, so replace read_ptr
            read_ptr  = writing;
            write_ptr = write_ptr->next; // we checked this in the while loop
            return true;
        }

        virtual bool data_sample( param_t sample, bool reset = true ) {
            if (!initialized || reset) {
                // prepare the buffer.
                for (unsigned int i = 0; i < BUF_LEN; ++i) {
                    data[i].data = sample;
                    data[i].status = NoData;
                    data[i].next = &data[i+1];
                }
                data[BUF_LEN-1].next = &data[0];
                initialized = true;
                return true;
            } else {
                return initialized;
            }
        }

        /**
         * Reads back a data sample.
         */
        virtual value_t data_sample() const {
            value_t sample;
            (void) Get(sample, /* copy_old_data = */ true, /* copy_sample = */ true);
            return sample;
        }

        // This is actually a copy of Get(), but it only sets the status to NoData once a valid buffer has been found.
        // Subsequent read() calls will read from the same buffer and will return NoData until a new sample has been written.
        virtual void clear() {
            if (!initialized) return;

            PtrType reading;
            // loop to combine Read/Modify of counter
            // This avoids a race condition where read_ptr
            // could become write_ptr ( then we would read corrupted data).
            do {
                reading = read_ptr;            // copy buffer location
                oro_atomic_inc(&reading->counter); // lock buffer, no more writes
                // XXX smp_mb
                if ( reading != read_ptr )     // if read_ptr changed,
                    oro_atomic_dec(&reading->counter); // better to start over.
                else
                    break;
            } while ( true );
            // from here on we are sure that 'reading'
            // is a valid buffer to read from.

            reading->status = NoData;

            // XXX smp_mb
            oro_atomic_dec(&reading->counter);       // release buffer
        }
    };
}}

#endif
