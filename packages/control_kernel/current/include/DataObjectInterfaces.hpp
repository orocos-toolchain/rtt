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
 
#ifndef DATAOBJECTINTERFACES_HPP
#define DATAOBJECTINTERFACES_HPP


#include <os/MutexLock.hpp>
#include <os/oro_atomic.h>
#include <string>

namespace ORO_ControlKernel
{

    /**
     * @brief A DataObjectInterface instance makes a data
     * struct available for the outside world to read from and write to.
     *
     * @see DataObject
     */
    template <class _DataType>
    struct DataObjectInterface
    {
        virtual ~DataObjectInterface() {}

        /**
         * The type of the data.
         */
        typedef _DataType DataType;
            
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
    };


    /**
     * @brief A class which provides locked/protected access to one typed element of data.
     *
     * It allows multiple read/write requests using a single lock. This is the in any case
     * threadsafe implementation, and can be blocking in situations where you do not want
     * that. Use the DataObjectPrioritySet and DataObjectPriorityGet classes for non
     * blocking Set or Get operations.
     */
    template<class _DataType>
    class DataObjectLocked
        : public DataObjectInterface< _DataType >
    {
        mutable ORO_OS::Mutex lock;
            
        /**
         * One element of Data.
         */
        _DataType data;

        std::string name;
    public:
        /** 
         * Construct a DataObjectLocked by name.
         * 
         * @param _name The name of this DataObject.
         * @param _prefix The prefix of this DataObject (not used).
         */
        DataObjectLocked(const std::string& _name, const std::string& _prefix = "")
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
        typedef _DataType DataType;
            
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
        DataType Get() const { _DataType cache;  Get(cache); return cache; }

        /**
         * Set the data to a certain value.
         *
         * @param push The data which must be set.
         */
        void Set( const DataType& push ) { ORO_OS::MutexLock locker(lock); data = push; }
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
     * @invar The Set() context has a strictly higher priority than the Get() context.
     * @invar Set() never blocks.
     */
    template<class _DataType>
    class DataObjectPrioritySet
        : public DataObjectInterface< _DataType >
    {
        mutable ORO_OS::Mutex lock;
        mutable bool dirty_flag;
            
        /**
         * One element of Data.
         */
        _DataType data;
        _DataType copy;

        std::string name;
    public:
        /** 
         * Construct a DataObjectPriorityGet by name.
         * 
         * @param _name The name of this DataObject.
         * @param _prefix The prefix of this DataObject (not used).
         */
        DataObjectPrioritySet(const std::string& _name, const std::string& _prefix = "")
            : data(), copy(), name(_name) {}

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
        typedef _DataType DataType;
            
        /**
         * Get a copy of the Data of the module.
         *
         * @param pull A copy of the data.
         */
        void Get( DataType& pull ) const
        { if (dirty_flag) pull = copy; else {ORO_OS::MutexLock locker(lock); pull = data;} }

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
                {copy = push; dirty_flag = true;}
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
     * @invar The Set() context has a strictly lower priority than the Get() context.
     * @invar Get() never blocks.
     */
    template<class _DataType>
    class DataObjectPriorityGet
        : public DataObjectInterface< _DataType >
    {
        mutable ORO_OS::Mutex lock;
            
        /**
         * One element of Data.
         */
        _DataType data;
        _DataType copy;

        std::string name;
    public:
        /** 
         * Construct a DataObjectPriorityGet by name.
         * 
         * @param _name The name of this DataObject.
         * @param _prefix The prefix of this DataObject (not used).
         */
        DataObjectPriorityGet(const std::string& _name, const std::string& _prefix  = std::string())
            : data(), copy(), name(_name) {}

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
        typedef _DataType DataType;
            
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
                {pull = copy;}
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
            copy = data;
        }
    };

    /**
     * @brief This DataObject is implemented with a circular
     * buffer, such that reads and writes can happen concurrently.
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
    template<class _DataType>
    class DataObjectBuffer
        : public DataObjectInterface< _DataType >
    {
        /**
         * Internal buffer structure.
         */
        struct DataBuf {
            DataBuf()
                : data(), counter(), next()
            {}
            _DataType data; mutable atomic_t counter; DataBuf* next;
        };

        DataBuf* read_ptr;
        DataBuf* write_ptr;

        /** 
         * @brief The size of the buffer.
         *
         * The size of the buffer is for now statically determined, 
         * which allows for 7 readers and 1 writer (a total of 8 threads !)
         * This is to be
         * improved. Heaping is an option, but the hardest problem
         * remains setting the size of the buffer.
         */
        static const unsigned int BUF_LEN=10;
            
        /**
         * A 3 element Data buffer
         */
        DataBuf data[BUF_LEN];

        std::string name;
    public:
        /** 
         * Construct a DataObjectBuffer by name.
         * 
         * @param _name The name of this DataObject.
         * @param _prefix The prefix of this DataObject (not used).
         */
        DataObjectBuffer(const std::string& _name, const std::string& _prefix  = std::string()) 
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
         * The type of the data.
         */
        typedef _DataType DataType;

        /**
         * Get a copy of the data.
         *
         * @return A reference to a copy of the data.
         */
        DataType Get() const {DataType cache; Get(cache); return cache; }
            
        /**
         * Get a copy of the Data (non blocking).
         *
         * @param pull A copy of the data.
         */
        void Get( DataType& pull ) const 
        {   
            DataBuf* reading = read_ptr;        // atomic copy
            atomic_inc(&reading->counter);       // atomic increment
            pull = reading->data;               // takes some time
            atomic_dec(&reading->counter);       // atomic decrement
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
            DataBuf* wrote_ptr = write_ptr;
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
    };

    /**
     * @brief A class which provides unchecked access to one typed element of data.
     *
     * It is the most simple form of the DataObjectInterface implementation and
     * hence also the fastest. It is however not thread safe.
     */
    template<class _DataType>
    class DataObject
        : public DataObjectInterface< _DataType >
    {
        /**
         * One element of Data.
         */
        _DataType data;

        std::string name;
    public:
        /** 
         * Construct a DataObject by name.
         * 
         * @param _name The name of this DataObject.
         * @param _prefix The prefix of this DataObject (not used).
         */
        DataObject(const std::string& _name, const std::string& _prefix  = std::string())
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
        typedef _DataType DataType;
            
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

    };

    // These classes are unused.  (ps)
#if 0
    /**
     * @brief A class which provides update based access to one typed element of data.
     */
    template<class _DataType>
    class DataObjectUpdated
        : public DataObjectInterface< _DataType >
    {
        /**
         * One element of Data.
         */
        _DataType data;
        mutable Event    updated;

        std::string name;
    public:
        /** 
         * Construct a DataObject by name.
         * 
         * @param _name The name of this DataObject.
         * @param _prefix The prefix of this DataObject (not used).
         */
        DataObjectUpdated(const std::string& _name, const std::string& _prefix  = std::string()) : name(_name) {}

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
        typedef _DataType DataType;
            
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
        void Set( const DataType& push ) { data = push; updated.fire(); }

        /**
         * Allows you to register a Listenener,Completer handler
         * to be notified when the DataObject is updated.
         */
        ORO_CoreLib::HandlerRegistrationInterface* eventGet() { return &updated; }
    };

    /**
     * @brief This DataObject updates the data with each Get() method call.
     * @see DataObject
     * @see DataObjectInterface
     */
    template<class _DataType>
    class DataRecursive
        :public DataObjectInterface<_DataType>
    {
        /**
         * One element of Data.
         */
        _DataType data;
    public:
        typedef _DataType DataType;
            
        /**
         * Create a recursive data element which fetches the data from a
         * DataFlowInterface.
         *
         * @param _module The module to provide the data for this instance.
         */
        DataRecursive( DataFlowInterface* _module) : module(_module) {}
            
        void Get( DataType& pull ) const { module->update(); pull = data; }
        DataType Get() const { module->update(); return data; }
        void Set( const DataType& push ) { data = push; }
    protected:
        mutable DataFlowInterface* module;
    };

    using ORO_CoreLib::HeartBeatGenerator;

    /**
     * @brief This DataObject refreshes the data after time S when a Get() method call is done.
     * @see DataObjectInterface
     * @see DataObject
     * @see DataRecursive
     * @note not a working implementation.
     */
    template<class _DataType>
    class DataRefreshed
        :public DataObjectInterface<_DataType>
    {
        /**
         * One element of Data.
         */
        _DataType data;
        
        /**
         * Time of data freshness.
         */
        mutable ORO_CoreLib::Seconds period;

        /**
         * Time of last measurement.
         */
        mutable HeartBeatGenerator::ticks timeStamp;
        std::string name;
    public:
        /** 
         * Construct a DataObject by name.
         * 
         * @param _name The name of this DataObject.
         * @param _prefix The prefix of this DataObject (not used).
         */
        DataRefreshed(const std::string& _name, const std::string& _prefix  = std::string()) : name(_name) {}

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

        typedef _DataType DataType;

        /**
         * Create a DataRefreshed with freshness <refresh>.
         *
         * @param refresh The maximum age of the data.
         */
        DataRefreshed(ORO_CoreLib::Seconds refresh) : period(refresh), timeStamp(0) {}
            
        void Get( DataType& pull ) const  
        { 
            if ( period < HeartBeatGenerator::secondsSince(period) )
                {
                    period = HeartBeatGenerator::secondsGet();
                    module->update(); 
                }
            pull = data; 
        }
            
        DataType Get() const
        { 
            if ( period < HeartBeatGenerator::secondsSince(period) )
                {
                    period = HeartBeatGenerator::secondsGet();
                    module->update(); 
                }
            return data; 
        }
            
        void Set( const DataType& push ) { data = push; }

    };
#endif

}

#endif
    
