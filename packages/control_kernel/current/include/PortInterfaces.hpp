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
#include "DataServer.hpp"

namespace ORO_ControlKernel
{
    namespace detail 
    {
        using ORO_CoreLib::CallbackInterface;
        using ORO_CoreLib::Completer;

        /**
         * @brief The DataObjectType templated interface of a port for
         * reading a \a DataObjectType object.
         */
        template <class _DataObjectType>
        class ReadPortInterface
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

            virtual ~ReadPortInterface() {}

            /**
             * Read from the following data object.
             *
             * @param _data The data to be added.
             */
            virtual void readFrom( const DataObjectType* _data ) = 0;

            /**
             * No longer read from the following data object.
             *
             * @param _data The data to be removed.
             */
            virtual void disconnect( const DataObjectType* _data ) = 0;

            /**
             * Inspect the port if the data it is pointing to is updated.
             */
            virtual bool isUpdated() const = 0;

            /**
             * checked access to the DataObject.
             */
            virtual const DataObjectType* dObj() = 0;
        };

        /**
         * @brief A Port which can be used to connect to, read from and
         * disconnect from a DataObject of a given type.
         *
         * @param _DataObjectType The type of the DataObject to be read.
         */
        template <class _DataObjectType>
        class StandardReadPort
            : public ReadPortInterface< _DataObjectType>
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
            bool isUpdated() const { return true; }

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
            : public ReadPortInterface< _DataObjectType>,
              public ORO_CoreLib::EventListenerInterface
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
            bool isUpdated() const { return updated; }

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
         * @brief The DataObjectType templated interface of a port for
         * writing a \a _DataObjectType object.
         */
        template <class _DataObjectType>
        class WritePortInterface
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

            virtual ~WritePortInterface() {}

            /**
             * Read from the following data object.
             *
             * @param _data The data to be added.
             */
            virtual void writeTo( DataObjectType* _data ) = 0;

            /**
             * No longer read from the following _data object.
             *
             * @param _data The data to be removed.
             */
            virtual void disconnect( DataObjectType* _data ) = 0;

            /**
             * checked access to the DataObject.
             */
            virtual DataObjectType* dObj() = 0;
        };

        /**
         * @brief A Port which can be used to connect to, write to and
         * disconnect from a DataObject of a given type.
         *
         * @param _DataObjectType The type of the DataObject to be written to.
         */
        template <class _DataObjectType>
        class StandardWritePort
            : public WritePortInterface< _DataObjectType>
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

        /**
         * A Component can read from multiple read ports. This
         * readport is for reading nameserved data objects.
         */
        template< typename _DataObjectType >
        class NameServeReadPort
            : public detail::ReadPortInterface< _DataObjectType >
        {
            const _DataObjectType* front_end;
        public:
            typedef _DataObjectType DataObjectType;

            NameServeReadPort() : front_end(0) {}

            ~NameServeReadPort() {
                delete front_end;
            }

            void createPort(const std::string& name, const std::string& prefix )
            {
                // name and prefix force us to postpone creation of the frontend,
                // until we are loaded into a kernel.
                front_end = new DataObjectType( name, prefix );
            }

            void erasePort()
            {
                delete front_end;
                front_end = 0;
            }

            const DataObjectType* dObj() {
                return front_end;
            }

            virtual void readFrom( const DataObjectType* _data )
            {
                front_end = _data;
            }

            virtual void disconnect( const DataObjectType* _data )
            {
                front_end = 0;
            }

            virtual bool isUpdated() const { return true; }
        };

        /**
         * Each Component has one Write Port. This kind of write port
         * is meant for nameserved data objects.
         */
        template< typename _DataObjectType >
        class NameServeWritePort
            : public detail::WritePortInterface< _DataObjectType >
        {
            _DataObjectType* front_end;
            // only needed for storage, DataObjectReporting is a common class
            // of all NameSubClass specialisations.
            DataObjectReporting* data_object;
        public:
            typedef _DataObjectType DataObjectType;

            NameServeWritePort() : front_end(0), data_object(0) {}

            ~NameServeWritePort() {
                delete front_end;
                delete data_object;
            }

            template< class _DataObjectContainer >
            void createPort(const std::string& name, const std::string& prefix, _DataObjectContainer dummy )
            {
                // create the frontend (for access)
                // todo : do without name, frontends must be nameless servers but keep prefix
                front_end   = new DataObjectType( name, prefix );
                // create the dataobject servers (for storage)
                typedef detail::NameServedDataObject<typename _DataObjectContainer::template DataObject<typename DataObjectType::NamesTypes> > DataStorage;
                data_object = new DataStorage( name, prefix );

            }

            void erasePort()
            {
                delete front_end;
                delete data_object;
                front_end = 0;
                data_object = 0;
            }

            DataObjectType* dObj() {
                return front_end;
            }

            virtual void writeTo( DataObjectType* _data )
            {
                front_end = _data;
            }

            virtual void disconnect( DataObjectType* _data )
            {
                front_end = 0;
            }

            virtual bool isUpdated() const { return true; }

        };

    }

    /**
     * User friendly read-port construction.
     */
    template< typename DataList >
    struct Expects
    {
        /**
         * Our DataObjecType is a NameServedDataObject of DataObjectInterfaces
         */
        typedef typename detail::NamesDOFactory<DataList>::interface DataObjectType;

        typedef typename detail::NamesDOFactory<DataList>::interface::DataType DataType;

        /**
         * The ReadPort is an NameServeReadPort
         */
        typedef NameServeReadPort<DataObjectType>    ReadPort;
    };

    /**
     * User friendly write-port construction.
     */
    template< typename DataList >
    struct Writes
    {
        /**
         * Our DataObjecType is a NameServedDataObject of DataObjectInterfaces
         */
        typedef typename detail::NamesDOFactory<DataList>::interface DataObjectType;
        typedef typename detail::NamesDOFactory<DataList>::interface::DataType DataType;

        /**
         * The WritePort is an NameServeWritePort
         */
        typedef NameServeWritePort<DataObjectType>    WritePort;
    };
}

#endif
    
