/***************************************************************************
  tag: Peter Soetens  Mon Jan 19 14:11:26 CET 2004  PortInterfaces.hpp 

                        PortInterfaces.hpp -  description
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
 
#ifndef PORTINTERFACES_HPP
#define PORTINTERFACES_HPP


#include <corelib/EventCompleterInterface.hpp>
#include <corelib/CallbackInterface.hpp>

namespace ORO_ControlKernel
{
    namespace detail 
    {
        using ORO_CoreLib::CallbackInterface;
        using ORO_CoreLib::Completer;

        /**
         * @brief A Port which can be used to connect to, read from and
         * disconnect from a DataObject of a given type.
         *
         * @param _DataObjectType The type of the DataObject to be read.
         */
        template <class _DataObjectType>
        class StandardReadPort
        {
        public:
            /**
             * The type of data the Port will read
             */
            typedef _DataObjectType DataObjectType;

            /**
             * This is a helper class for use in the
             * NSControlKernel (it is nonnensical in other kernels).
             */
            template < class _DataType >
            struct DataObject
            {
                typedef typename _DataObjectType::template DataObject< _DataType >::type type;
            };

            /**
             * Default contructor.
             */
            StandardReadPort() : _dObj(0) {}

            /**
             * Read from the following data object.
             *
             * @param _data The data to be added.
             */
            void readFrom( const DataObjectType* _data ) 
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
            void disconnect( const DataObjectType* _data ) 
            { if (_dObj == _data) _dObj = 0; }

            /**
             * Inspect the port if the data it is pointing to is updated.
             */
            bool isUpdated() { return true; }

            /**
             * checked access to the DataObject.
             */
            const DataObjectType* dObj()
            {
                return _dObj;
            }

        private:
            /**
             * The real data.
             */
            const DataObjectType* _dObj;
        };

        /**
         * @brief An Event based Port which can be used to connect to, read from and
         * disconnect from a DataObject of a given type.
         *
         * This port uses an
         * event-update based isUpdated() method, which means that the DataObject
         * will notify this port when new data is available.
         *
         * @param _DataObjectType The type of the DataObject to be read.
         */
        template <class _DataObjectType>
        class ReadPortUpdated
            : public ORO_CoreLib::EventListenerInterface
        {
        public:
            /**
             * The type of data the Port will read
             */
            typedef _DataObjectType DataObjectType;

            /**
             * Default contructor.
             */
            ReadPortUpdated() : _dObj(0), updated(false) {}

            /**
             * Read from the following data object.
             *
             * @param _data The data to be added.
             */
            void readFrom( const DataObjectType* _data ) 
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
            void disconnect( const DataObjectType* _data ) 
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
            const DataObjectType* dObj()
            {
                updated = false;
                return _dObj;
            }

        private:
            /**
             * The real data.
             */
            const DataObjectType* _dObj;

            /**
             * Flag keeping track of DataObject updates.
             */
            bool updated;
        };

        /**
         * @brief A Port which can be used to connect to, write to and
         * disconnect from a DataObject of a given type.
         *
         * @param _DataObjectType The type of the DataObject to be written to.
         */
        template <class _DataObjectType>
        class StandardWritePort
        {
        public:
            /**
             * The type of data the Port will read
             */
            typedef _DataObjectType DataObjectType;

            /**
             * This is a helper class for use in the
             * NSControlKernel (it is nonnensical in other kernels).
             */
            template < class _DataType >
            struct DataObject
            {
                typedef typename _DataObjectType::template DataObject< _DataType >::type type;
            };

            /**
             * Default contructor.
             */
            StandardWritePort() : _dObj(0) {}

            /**
             * Read from the following data object.
             *
             * @param _data The data to be added.
             */
            void writeTo( DataObjectType* _data ) 
            { if(_dObj == 0) _dObj = _data; }

            /**
             * No longer read from the following _data object.
             *
             * @param _data The data to be removed.
             */
            void disconnect( DataObjectType* _data )
            { if (_dObj == _data) _dObj = 0; }

            /**
             * checked access to the DataObject.
             */
            DataObjectType* dObj()
            {
                return _dObj;
            }

        private:
            /**
             * The real data.
             */
            DataObjectType* _dObj;
        };

        /**
         * @brief Type container which holds info about the DataObject type, 
         * WritePort type and ReadPort type for standard ports.
         *
         * @param _DataObjectType The type of DataObject to be used.
         */
        template<class _DataObjectType>
        struct StandardPort
        {
            /**
             * @brief The DataObjectType of this port.
             */
            typedef _DataObjectType DataObjectType;

            /**
             * @brief The DataType of the DataObjectType of this port.
             */
            typedef typename DataObjectType::DataType DataType;

            /**
             * @brief The WritePort type of this port.
             */
            typedef StandardWritePort<DataObjectType> WritePort;

            /**
             * @brief The ReadPort type of this port.
             */
            typedef StandardReadPort<DataObjectType> ReadPort;
        };
    }

}

#endif
    
