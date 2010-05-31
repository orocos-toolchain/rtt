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


#include "../os/oro_atomic.h"
#include "DataObjectInterface.hpp"

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
     * @ingroup Ports
     */
    template<class T>
    class DataObjectLockFree
        : public DataObjectInterface<T>
    {
    public:
        /**
         * The type of the data.
         */
        typedef T DataType;

        /**
         * @brief The maximum number of threads.
         *
         * When used in data flow, this is always 2.
         */
        const unsigned int MAX_THREADS; // = 2
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
                : data(), counter(), next()
            {
                oro_atomic_set(&counter, 0);
            }
            DataType data; mutable oro_atomic_t counter; DataBuf* next;
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
    public:

        /**
         * Construct a DataObjectLockFree by name.
         *
         * @param _name The name of this DataObject.
         * @param initial_value The initial value of this DataObject.
         */
        DataObjectLockFree( const T& initial_value = T(), unsigned int max_threads = 2 )
            : MAX_THREADS(max_threads), BUF_LEN( max_threads + 2),
              read_ptr(0),
              write_ptr(0)
        {
        	data = new DataBuf[BUF_LEN];
        	read_ptr = &data[0];
        	write_ptr = &data[1];
            data[BUF_LEN-1].next = &data[0];
            data_sample(initial_value);
        }

        ~DataObjectLockFree() {
            delete[] data;
        }

        /**
         * Get a copy of the data.
         * This method will allocate memory twice if data is not a value type.
         * Use Get(DataType&) for the non-allocating version.
         *
         * @return A copy of the data.
         */
        virtual DataType Get() const {DataType cache; Get(cache); return cache; }

        /**
         * Get a copy of the Data (non allocating).
         * If pull has reserved enough memory to store the copy,
         * no memory will be allocated.
         *
         * @param pull A copy of the data.
         */
        virtual void Get( DataType& pull ) const
        {
            PtrType reading;
            // loop to combine Read/Modify of counter
            // This avoids a race condition where read_ptr
            // could become write_ptr ( then we would read corrupted data).
            do {
                reading = read_ptr;            // copy buffer location
                oro_atomic_inc(&reading->counter); // lock buffer, no more writes
                if ( reading != read_ptr )     // if read_ptr changed,
                    oro_atomic_dec(&reading->counter); // better to start over.
                else
                    break;
            } while ( true );
            // from here on we are sure that 'reading'
            // is a valid buffer to read from.
            pull = reading->data;               // takes some time
            oro_atomic_dec(&reading->counter);       // release buffer
        }

        /**
         * Set the data to a certain value (non blocking).
         *
         * @param push The data which must be set.
         */
        virtual void Set( const DataType& push )
        {
            /**
             * This method can not be called concurrently (only one
             * producer). With a minimum of 3 buffers, if the
             * write_ptr+1 field is not occupied, it will remain so
             * because the read_ptr is at write_ptr-1 (and can
             * not increment the counter on write_ptr+1). Hence, no
             * locking is needed.
             */
            // writeout in any case
            write_ptr->data = push;
            PtrType wrote_ptr = write_ptr;
            // if next field is occupied (by read_ptr or counter),
            // go to next and check again...
            while ( oro_atomic_read( &write_ptr->next->counter ) != 0 || write_ptr->next == read_ptr )
                {
                    write_ptr = write_ptr->next;
                    if (write_ptr == wrote_ptr)
                        return; // nothing found, to many readers !
                }

            // we will be able to move, so replace read_ptr
            read_ptr  = wrote_ptr;
            write_ptr = write_ptr->next; // we checked this in the while loop
        }

        virtual void data_sample( const DataType& sample ) {
            // prepare the buffer.
            for (unsigned int i = 0; i < BUF_LEN-1; ++i) {
                data[i].data = sample;
                data[i].next = &data[i+1];
            }

        }

        virtual DataObjectLockFree<DataType>* clone() const {
            return new DataObjectLockFree<DataType>();
        }

        virtual DataObjectLockFree<DataType>* copy( std::map<const DataSourceBase*, DataSourceBase*>&  ) const {
            return const_cast<DataObjectLockFree<DataType>*>(this);
        }

    };
}}

#endif

