#ifndef DATAOBJECTINTERFACES_HPP
#define DATAOBJECTINTERFACES_HPP


#include "ModuleControlInterface.hpp"
#include <corelib/Time.hpp>
#include <corelib/HeartBeatGenerator.hpp>
#include <corelib/Event.hpp>
#include <os/MutexLock.hpp>

namespace ORO_ControlKernel
{

    /**
     * A DataObjectInterface instance makes a data
     * struct available for the outside world to read from and write to.
     * We don't use this class when efficiency is important 
     * (a virtual function can not be inlined), but it is handy for 
     * complexer DataObjects.
     *
     * @see DataObject
     */
    template <class _DataType>
    struct DataObjectInterface
    {
        /**
         * The type of the data.
         */
        typedef _DataType DataType;
            
        /**
         * Get a copy of the Data of the module.
         *
         * @param pull A copy of the data.
         */
        virtual void Get( DataType& pull ) const = 0;

        /**
         * Get a const reference of the data of the module.
         *
         * @return The result of the module.
         */
        virtual const DataType& Get() const = 0;

        /**
         * Set the data to a certain value.
         *
         * @param push The data which must be set.
         */
        virtual void Set( const DataType& push ) = 0;

    };


    /**
     * A class which provides locked/protected access to one typed element of data.
     * It allows multiple read/write requests using a single lock.
     */
    template<class _DataType>
    class DataObjectLocked
    {
        mutable ORO_OS::Mutex lock;
            
        /**
         * One element of Data.
         */
        _DataType data;

        mutable _DataType cache;
    public:
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
         * Get a const reference to a copy of the data of the module.
         * This method is thread-safe.
         *
         * @return The result of the module.
         */
        const DataType& Get() const { Get(cache); return cache; }

        /**
         * Set the data to a certain value.
         *
         * @param push The data which must be set.
         */
        void Set( const DataType& push ) { ORO_OS::MutexLock locker(lock); data = push; }
    };

    /**
     * A class which provides locked/protected access to one typed element of data,
     * where the locking is optimised for Low priority thread reads and High priority
     * thread writes. The Set() operation is never blocking.
     *
     * @invar The Set() context has a strictly higher priority than the Get() context.
     * @invar Set() never blocks.
     */
    template<class _DataType>
    class DataObjectPrioritySet
    {
        mutable ORO_OS::Mutex lock;
        mutable bool dirty_flag;
            
        /**
         * One element of Data.
         */
        _DataType data;
        _DataType copy;

        mutable _DataType cache;
    public:
        /**
         * The type of the data.
         */
        typedef _DataType DataType;
            
        /**
         * Get a copy of the Data of the module.
         *
         * @param pull A copy of the data.
         */
        void Get( DataType& pull ) const { if (dirty_flag) pull = copy; else {ORO_OS::MutexLock locker(lock); pull = data;} }

        /**
         * Get a const reference to a copy of the data of the module.
         * This method is thread-safe.
         *
         * @return The result of the module.
         */
        const DataType& Get() const { Get(cache); return cache; }

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
     * A class which provides locked/protected access to one typed element of data,
     * where the locking is optimised for Low priority thread writes and High priority
     * thread reads. The Get() operation is always non blocking.
     *
     * @invar The Set() context has a strictly lower priority than the Get() context.
     * @invar Get() never blocks.
     */
    template<class _DataType>
    class DataObjectPriorityGet
    {
        mutable ORO_OS::Mutex lock;
            
        /**
         * One element of Data.
         */
        _DataType data;
        _DataType copy;

        mutable _DataType cache;
    public:
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
            ORO_OS::MutexTryLock locker(lock); 
            if ( locker.isSuccessful() ) 
                {pull = data;} 
            else 
                {pull = copy;}
        }

        /**
         * Get a const reference to a copy of the data of the module.
         * This method is thread-safe.
         *
         * @return The result of the module.
         */
        const DataType& Get() const { Get(cache); return cache; }

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
     * A class which provides unchecked access to one typed element of data.
     * It is the most simple form of the DataObjectInterface implementation.
     */
    template<class _DataType>
    class DataObject
    {
        /**
         * One element of Data.
         */
        _DataType data;

    public:
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
         * Get a const reference of the data of the module.
         *
         * @return The result of the module.
         */
        const DataType& Get() const { return data; }
            
        /**
         * Set the data to a certain value.
         *
         * @param push The data which must be set.
         */
        void Set( const DataType& push ) { data = push; }

    };

    /**
     * A class which provides update based access to one typed element of data.
     */
    template<class _DataType>
    class DataObjectUpdated
    {
        /**
         * One element of Data.
         */
        _DataType data;
        mutable Event    updated;
    public:
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
         * Get a const reference of the data of the module.
         *
         * @return The result of the module.
         */
        const DataType& Get() const { return data; }
            
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
     * This DataObject updates the data with each Get() method call.
     * @see DataObject
     * @see DataObjectInterface
     */
    template<class _DataType>
    class DataRecursive
        :public DataObject<_DataType>
    {
        /**
         * One element of Data.
         */
        _DataType data;
    public:
        typedef _DataType DataType;
            
        /**
         * Create a recursive data element which fetches the data from a
         * ModuleControlInterface.
         *
         * @param _module The module to provide the data for this instance.
         */
        DataRecursive( ModuleControlInterface* _module) : module(_module) {}
            
        void Get( DataType& pull ) const { module->update(); pull = data; }
        const DataType& Get() const { module->update(); return data; }
        void Set( const DataType& push ) { data = push; }
    protected:
        mutable ModuleControlInterface* module;
    };

    using ORO_CoreLib::HeartBeatGenerator;

    /**
     * This DataObject refreshes the data after time S when a Get() method call is done.
     * @see DataObjectInterface
     * @see DataObject
     * @see DataRecursive
     */
    template<class _DataType>
    class DataRefreshed
        :public DataObject<_DataType>
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
    public:
        typedef _DataType DataType;

        /**
         * Create a DataRefreshed with freshness <refresh>.
         *
         * @param refresh The maximum age of the data.
         */
        DataRefreshed(ORO_CoreLib::Seconds refresh) : period(refresh), timeStamp(0) {}
            
        void Get( DataType& pull ) const  
        { 
            if ( period < HeartBeatGenerator::timeSecondsGet(period) )
                {
                    period = HeartBeatGenerator::timeGet();
                    module->update(); 
                }
            pull = data; 
        }
            
        const DataType& Get() const
        { 
            if ( period < HeartBeatGenerator::timeSecondsGet(period) )
                {
                    period = HeartBeatGenerator::timeGet();
                    module->update(); 
                }
            return data; 
        }
            
        void Set( const DataType& push ) { data = push; }

    };


}

#endif
    
