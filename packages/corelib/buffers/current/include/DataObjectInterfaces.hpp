/***************************************************************************
  tag: Peter Soetens  Mon Jan 19 14:11:26 CET 2004  DataObjectInterfaces.hpp 

                        DataObjectInterfaces.hpp -  description
                           -------------------
    begin                : Mon January 19 2004
    copyright            : (C) 2004 Peter Soetens
    email                : peter.soetens@mech.kuleuven.ac.be
 
 ***************************************************************************
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU Lesser General Public            *
 *   License as published by the Free Software Foundation; either          *
 *   version 2.1 of the License, or (at your option) any later version.    *
 *                                                                         *
 *   This library is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU     *
 *   Lesser General Public License for more details.                       *
 *                                                                         *
 *   You should have received a copy of the GNU Lesser General Public      *
 *   License along with this library; if not, write to the Free Software   *
 *   Foundation, Inc., 59 Temple Place,                                    *
 *   Suite 330, Boston, MA  02111-1307  USA                                *
 *                                                                         *
 ***************************************************************************/ 
 
#ifndef CORELIB_DATAOBJECTINTERFACES_HPP
#define CORELIB_DATAOBJECTINTERFACES_HPP


#include <os/MutexLock.hpp>
#include <os/oro_atomic.h>
#include <string>

#include "DataSource.hpp"

namespace ORO_CoreLib
{

    /**
     * @brief A DataObjectInterface extends the AssignableDataSource with
     * implementations of multi-threaded read/write solutions.
     *
     * @see DataObject
     * @param T The \a DataType which can be Get() or Set() with this DataObject.
     */
    template <class T>
    struct DataObjectInterface
        : public AssignableDataSource<T>
    {
        typedef typename boost::intrusive_ptr<DataObjectInterface<T> > shared_ptr;

        /**
         * Destructor. Since the DataObjectInterface has become a DataSource (Orocos 0.20.0)
         * This will become protected in upcomming releases.
         */
        virtual ~DataObjectInterface() {}

        /**
         * The type of the data.
         */
        typedef T DataType;
            
        /**
         * Get a copy of the Data of this data object.
         *
         * @param pull A copy of the data.
         */
        virtual void Get( DataType& pull ) const = 0;

        /**
         * Get a copy of the data of this data object.
         *
         * @return A copy of the data.
         */
        virtual DataType Get() const = 0;

        /**
         * Set the data to a certain value.
         *
         * @param push The data which must be set.
         */
        virtual void Set( const DataType& push ) = 0;

        /** 
         * Return the name of this DataObject.
         * 
         * @return The name
         */
        virtual const std::string& getName() const = 0;

        virtual typename DataSource<DataType>::result_t get() const {
            return this->Get();
        }

        virtual void set( typename AssignableDataSource<DataType>::param_t t ) {
            this->Set( t );
        }

        virtual typename AssignableDataSource<DataType>::reference_t set() {
            // return null reference, allowed by API.
            typename DataSource<DataType>::value_t* tmp = 0;
            return typename AssignableDataSource<DataType>::reference_t(*tmp);
        }

    };


    /**
     * @brief A class which provides locked/protected access to one typed element of data.
     *
     * It allows multiple read/write requests using a single lock. This is the in any case
     * threadsafe implementation, and can be blocking in situations where you do not want
     * that. Use the DataObjectPrioritySet and DataObjectPriorityGet classes for non
     * blocking Set or Get operations.
     */
    template<class T>
    class DataObjectLocked
        : public DataObjectInterface<T>
    {
        mutable ORO_OS::Mutex lock;
            
        /**
         * One element of Data.
         */
        T data;

        std::string name;
    public:
        /** 
         * Construct a DataObjectLocked by name.
         * 
         * @param _name The name of this DataObject.
         * @param _prefix The prefix of this DataObject (not used).
         */
        DataObjectLocked(const std::string& _name, const std::string&  = "")
            : data(), name(_name) {}

        /** 
         * Return the name of this DataObject.
         * 
         * @return The name
         */
        const std::string& getName() const { return name;}

        void setName( const std::string& _name )
        {
            name = _name;
        }

        /**
         * The type of the data.
         */
        typedef T DataType;
            
        /**
         * Get a copy of the Data of the module.
         *
         * @param pull A copy of the data.
         */
        void Get( DataType& pull ) const { ORO_OS::MutexLock locker(lock); pull = data; }

        /**
         * Get a copy of the data of the module.
         * This method is thread-safe.
         *
         * @return The result of the module.
         */
        DataType Get() const { DataType cache;  Get(cache); return cache; }

        /**
         * Set the data to a certain value.
         *
         * @param push The data which must be set.
         */
        void Set( const DataType& push ) { ORO_OS::MutexLock locker(lock); data = push; }

        DataObjectLocked<DataType>* clone() const {
            return new DataObjectLocked<DataType>(name);
        }

        DataObjectLocked<DataType>* copy( std::map<const DataSourceBase*, DataSourceBase*>&  ) {
            return this;
        }
    };

    /**
     * @brief A DataObject which allows only the high priority thread to Set(),
     *        and both low and high priorities to Get().
     *
     * A class which provides locked/protected access to one typed element of data,
     * where the locking is optimised for Low priority thread reads and High priority
     * thread writes. The Set() operation is never blocking.
     *
     * @verbatim
     * The following Truth table applies when a Low Priority thread is
     * preempted by a High Priority thread :
     *
     *   L\H | Set | Get |
     *   Set | NA  | Blk |
     *   Get | Ok  | Blk |
     *
     * legend : L : Low Priority thread
     *          H : High Priority thread
     *          Blk: Blocks High Priority thread (bad!)
     *          NA : Not Allowed !
     * @endverbatim
     * If only one thread accesses the DataObject simultaneously (e.g. single
     * threaded systems), then the Get() and Set() methods will never block.
     *
     * @invariant The Set() context has a strictly higher priority than the Get() context.
     * @invariant Set() never blocks.
     */
    template<class T>
    class DataObjectPrioritySet
        : public DataObjectInterface<T>
    {
        mutable ORO_OS::Mutex lock;
        mutable bool dirty_flag;
            
        /**
         * One element of Data.
         */
        T data;
        T mcopy;

        std::string name;
    public:
        /** 
         * Construct a DataObjectPriorityGet by name.
         * 
         * @param _name The name of this DataObject.
         * @param _prefix The prefix of this DataObject (not used).
         */
        DataObjectPrioritySet(const std::string& _name, const std::string&  = "")
            : data(), mcopy(), name(_name) {}

        /** 
         * Return the name of this DataObject.
         * 
         * @return The name
         */
        const std::string& getName() const { return name;}

        void setName( const std::string& _name )
        {
            name = _name;
        }

        /**
         * The type of the data.
         */
        typedef T DataType;
            
        /**
         * Get a copy of the Data of the module.
         *
         * @param pull A copy of the data.
         */
        void Get( DataType& pull ) const
        { if (dirty_flag) pull = mcopy; else {ORO_OS::MutexLock locker(lock); pull = data;} }

        /**
         * @brief Get a copy of the data of the module.
         *
         * @return A copy of the data.
         */
        DataType Get() const { DataType cache; Get(cache); return cache; }

        /**
         * Set the data to a certain value.
         *
         * @param push The data which must be set.
         */
        void Set( const DataType& push ) 
        { 
            ORO_OS::MutexTryLock locker(lock); 
            if (locker.isSuccessful()) 
                {data = push; dirty_flag = false;} 
            else 
                {mcopy = push; dirty_flag = true;}
        }

        DataObjectPrioritySet<DataType>* clone() const {
            return new DataObjectPrioritySet<DataType>(name);
        }

        DataObjectPrioritySet<DataType>* copy( std::map<const DataSourceBase*, DataSourceBase*>&  ) {
            return this;
        }
    };

    /**
     * @brief A DataObject which allows only the high priority thread to Get(),
     *        and both low and high priorities to Set().
     *
     * A class which provides locked/protected access to one typed element of data,
     * where the locking is optimised for Low priority thread writes and High priority
     * thread reads. The Get() operation is always non blocking.
     *
     * @warning This DataObject can only be used in a two thread with distinct static
     * priority model
     *
     * @verbatim
     * The following Truth table applies when a Low Priority thread is
     * preempted by a High Priority thread :
     *
     *   L\H | Set | Get |
     *   Set | NA  | Ok  |
     *   Get | Blk | Ok  |
     *
     * legend : L : Low Priority thread
     *          H : High Priority thread
     *          Blk: Blocks High Priority thread (bad!)
     *          NA : Not allowed !
     * @endverbatim
     * If only one thread accesses the DataObject simultaneously (e.g. single
     * threaded systems), then the Get() and Set() methods will never block.
     *
     * @invariant The Set() context has a strictly lower priority than the Get() context.
     * @invariant Get() never blocks.
     */
    template<class T>
    class DataObjectPriorityGet
        : public DataObjectInterface<T>
    {
        mutable ORO_OS::Mutex lock;
            
        /**
         * One element of Data.
         */
        T data;
        T mcopy;

        std::string name;
    public:
        /** 
         * Construct a DataObjectPriorityGet by name.
         * 
         * @param _name The name of this DataObject.
         * @param _prefix The prefix of this DataObject (not used).
         */
        DataObjectPriorityGet(const std::string& _name, const std::string&   = std::string())
            : data(), mcopy(), name(_name) {}

        /** 
         * Return the name of this DataObject.
         * 
         * @return The name
         */
        const std::string& getName() const { return name;}

        void setName( const std::string& _name )
        {
            name = _name;
        }

        /**
         * The type of the data.
         */
        typedef T DataType;
            
        /**
         * Get a copy of the Data of the module.
         *
         * @param pull A copy of the data.
         */
        void Get( DataType& pull ) const 
        { 
            // If two low priority gets access this method,
            // while Set() is in copy, 
            // the second one will pull in the copy which
            // is modified by the Set().
            ORO_OS::MutexTryLock locker(lock); 
            if ( locker.isSuccessful() ) 
                {pull = data;} 
            else 
                {pull = mcopy;}
        }

        /**
         * Get a copy of the data of the module.
         *
         * @return A copy of the data.
         */
        DataType Get() const { DataType cache; Get(cache); return cache; }

        /**
         * Set the data to a certain value.
         *
         * @param push The data which must be set.
         */
        void Set( const DataType& push ) 
        { 
            {
                ORO_OS::MutexLock locker(lock); 
                data = push; 
            }
            mcopy = data;
        }

        DataObjectPriorityGet<DataType>* clone() const {
            return new DataObjectPriorityGet<DataType>(name);
        }

        DataObjectPriorityGet<DataType>* copy( std::map<const DataSourceBase*, DataSourceBase*>&  ) {
            return this;
        }
    };

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
     *   Set | NA  | Ok  |
     *   Get | Ok  | Ok  |
     *
     * legend : L : Low Priority thread
     *          H : High Priority thread
     *          Blk: Blocks High Priority thread (bad!)
     *          NA : Not allowed !
     * @endverbatim
     * Further, multiple reads may occur before, during and after
     * a write operation simultaneously. The buffer needs readers+3
     * elements to be guaranteed non blocking.
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
         * The size of the buffer is for now statically determined, 
         * which allows for 7 readers and 1 writer (a total of 8 threads !)
         * This is to be improved, although knowing the max number of
         * threads in a RT application is not so hard.
         */
        static const unsigned int MAX_THREADS=8;
    private:
        /**
         * Conversion of number of threads to size of buffer.
         */
        static const unsigned int BUF_LEN=MAX_THREADS+2;

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
                atomic_set(&counter, 0);
            }
            DataType data; mutable atomic_t counter; DataBuf* next;
        };

        typedef DataBuf* volatile VolPtrType;
        typedef DataBuf  ValueType;
        typedef DataBuf* PtrType;

        VolPtrType read_ptr;
        VolPtrType write_ptr;

        /**
         * A 3 element Data buffer
         */
        DataBuf data[BUF_LEN];

        std::string name;

    public:
            
        /** 
         * Construct a DataObjectLockFree by name.
         * 
         * @param _name The name of this DataObject.
         * @param _prefix The prefix of this DataObject (not used).
         */
        DataObjectLockFree(const std::string& _name, const std::string&   = std::string()) 
            : read_ptr(&data[ 0 ]), 
              write_ptr(&data[ 1 ]), 
              name(_name)
        {
            // prepare the buffer.
            for (unsigned int i = 0; i < BUF_LEN-1; ++i)
                data[i].next = &data[i+1];
            data[BUF_LEN-1].next = &data[0];
        }

        /** 
         * Return the name of this DataObject.
         * 
         * @return The name
         */
        const std::string& getName() const { return name;}

        void setName( const std::string& _name )
        {
            name = _name;
        }

        /**
         * Get a copy of the data.
         * This method will allocate memory twice if data is not a value type.
         * Use Get(DataType&) for the non-allocating version.
         *
         * @return A copy of the data.
         */
        DataType Get() const {DataType cache; Get(cache); return cache; }
            
        /**
         * Get a copy of the Data (non allocating).
         * If pull has reserved enough memory to store the copy,
         * no memory will be allocated.
         *
         * @param pull A copy of the data.
         */
        void Get( DataType& pull ) const 
        {   
            PtrType reading;
            // loop to combine Read/Modify of counter
            // This avoids a race condition where read_ptr
            // could become write_ptr ( then we would read corrupted data).
            do {
                reading = read_ptr;            // copy buffer location
                atomic_inc(&reading->counter); // lock buffer, no more writes
                if ( reading != read_ptr )     // if read_ptr changed, 
                    atomic_dec(&reading->counter); // better to start over.
                else
                    break;
            } while ( true );
            // from here on we are sure that 'reading'
            // is a valid buffer to read from.
            pull = reading->data;               // takes some time
            atomic_dec(&reading->counter);       // release buffer
        }

        /**
         * Set the data to a certain value (non blocking).
         *
         * @param push The data which must be set.
         */
        void Set( const DataType& push ) 
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
            while ( atomic_read( &write_ptr->next->counter ) != 0 || write_ptr->next == read_ptr )
                {
                    write_ptr = write_ptr->next;
                    if (write_ptr == wrote_ptr)
                        return; // nothing found, to many readers !
                }

            // we will be able to move, so replace read_ptr
            read_ptr  = wrote_ptr;
            write_ptr = write_ptr->next; // we checked this in the while loop
        }

        DataObjectLockFree<DataType>* clone() const {
            return new DataObjectLockFree<DataType>(name);
        }

        DataObjectLockFree<DataType>* copy( std::map<const DataSourceBase*, DataSourceBase*>&  ) {
            return this;
        }

    };

    /**
     * @brief A class which provides unchecked access to one typed element of data.
     *
     * It is the most simple form of the DataObjectInterface implementation and
     * hence also the fastest. It is however not thread safe.
     */
    template<class T>
    class DataObject
        : public DataObjectInterface<T>
    {
        /**
         * One element of Data.
         */
        T data;

        std::string name;
    public:
        /** 
         * Construct a DataObject by name.
         * 
         * @param _name The name of this DataObject.
         * @param _prefix The prefix of this DataObject (not used).
         */
        DataObject(const std::string& _name, const std::string&   = std::string())
            : data(), name(_name) {}

        /** 
         * Return the name of this DataObject.
         * 
         * @return The name
         */
        const std::string& getName() const { return name;}

        void setName( const std::string& _name )
        {
            name = _name;
        }

        /**
         * The type of the data.
         */
        typedef T DataType;
            
        /**
         * Get a copy of the Data of the module.
         *
         * @param pull A copy of the data.
         */
        void Get( DataType& pull ) const { pull = data; }

        /**
         * Get a copy of the data of the module.
         *
         * @return The result of the module.
         */
        DataType Get() const { return data; }
            
        /**
         * Set the data to a certain value.
         *
         * @param push The data which must be set.
         */
        void Set( const DataType& push ) { data = push; }

        DataObject<DataType>* clone() const {
            return new DataObject<DataType>(name);
        }

        DataObject<DataType>* copy( std::map<const DataSourceBase*, DataSourceBase*>&  ) {
            return this;
        }

    };
}

#endif
    
