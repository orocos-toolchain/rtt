#ifndef PORTINTERFACES_HPP
#define PORTINTERFACES_HPP


#include <corelib/EventCompleterInterface.hpp>
#include <corelib/CallbackInterface.hpp>

namespace ORO_ControlKernel
{

    using ORO_CoreLib::CallbackInterface;
    using ORO_CoreLib::Completer;

    /**
     * A wrapper class that contains the ReadPort for a certain
     * DataObjectType.
     */
    struct StandardReadPort
    {

        /**
         * A KernelModulReadPort interface defines an object that
         * can read certain data from a _DataType.
         * 
         */
        template <class DataObjectType>
        class ReadPort
        {
        public:
            /**
             * The type of data the Port will read
             */
            typedef DataObjectType DataType;

            /**
             * Default contructor.
             */
            ReadPort() : _dObj(0) {}

            /**
             * Read from the following data object.
             *
             * @param _data The data to be added.
             */
            void readFrom( const DataType* _data ) 
            { 
                if(_dObj == 0) 
                    {
                        _dObj = _data; 
                    }
            }

            /**
             * No longer read from the following data object.
             *
             * @param _data The data to be removed.
             */
            void disconnect( const DataType* _data ) 
            { if (_dObj == _data) _dObj = 0; }

            /**
             * Inspect the port if the data it is pointing to is updated.
             */
            bool isUpdated() { return true; }

            /**
             * checked access to the DataObject.
             */
            const DataType* dObj()
            {
                return _dObj;
            }

        private:
            /**
             * The real data.
             */
            const DataType* _dObj;
        };
    };

    /**
     * A wrapper class that holds the ReadPort implementation
     * for an updated readport.
     */
    struct UpdatedReadPort
    {

        /**
         * A KernelModulReadPort interface defines an object that
         * can read certain data from a _DataType. This port uses an
         * event-update based isUpdated() method, which means that the DataObject
         * will notify this port when new data is available.
         */
        template <class DataObjType>
        class ReadPortUpdated
            : public ORO_CoreLib::EventListenerInterface
        {
        public:
            /**
             * The type of data the Port will read
             */
            typedef DataObjType DataType;

            /**
             * Default contructor.
             */
            ReadPortUpdated() : _dObj(0), updated(false) {}

            /**
             * Read from the following data object.
             *
             * @param _data The data to be added.
             */
            void readFrom( const DataType* _data ) 
            { 
                if(_dObj == 0) 
                    {
                        _dObj = _data; 
                        _dObj->event()->handlerAdd(this, Completer::None);
                    }
            }

            /**
             * No longer read from the following data object.
             *
             * @param _data The data to be removed.
             */
            void disconnect( const DataType* _data ) 
            { 
                if (_dObj == _data) 
                    {
                        _dObj = 0; 
                        _dObj->event()->handlerRemove(this, Completer::None);
                    }
            }

            /**
             * Inspect the port if the data it is pointing to is updated.
             */
            bool isUpdated() { return updated; }

            virtual void handleEvent( CallbackInterface* )
            {
                updated = true;
            }

            /**
             * checked access to the DataObject.
             */
            const DataType* dObj()
            {
                updated = false;
                return _dObj;
            }

        private:
            /**
             * The real data.
             */
            const DataType* _dObj;

            /**
             * Flag keeping track of DataObject updates.
             */
            bool updated;
        };
    };

    /**
     * A wrapper class for a write port for writing to data objects
     * of a certain type.
     */
    struct StandardWritePort
    {
        /**
         * A WritePort interface defines an object that
         * can write certain data from a DataObject.
         * 
         */
        template <class DataObjType>
        class WritePort
        {
        public:
            /**
             * The type of data the Port will read
             */
            typedef DataObjType DataType;

            /**
             * Default contructor.
             */
            WritePort() : _dObj(0) {}

            /**
             * Read from the following data object.
             *
             * @param _data The data to be added.
             */
            void writeTo( DataType* _data ) 
            { if(_dObj == 0) _dObj = _data; }

            /**
             * No longer read from the following _data object.
             *
             * @param _data The data to be removed.
             */
            void disconnect( DataType* _data )
            { if (_dObj == _data) _dObj = 0; }

            /**
             * checked access to the DataObject.
             */
            DataType* dObj()
            {
                return _dObj;
            }

        private:
            /**
             * The real data.
             */
            DataType* _dObj;
        };
    };

    /**
     * Grouping ports together.
     */
    struct StandardPort : public StandardReadPort, public StandardWritePort {};
    struct UpdatedPort : public UpdatedReadPort, public StandardWritePort {};
}

#endif
    
