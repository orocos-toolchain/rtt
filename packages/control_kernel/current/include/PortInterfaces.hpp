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


#include "DataServer.hpp"

namespace ORO_ControlKernel
{
    namespace detail 
    {
        /**
         * @brief The DataObjectType templated interface of a port for
         * reading a \a DataObjectType object.
         * @param _DataObjectType The Type of DataObject accessed through this port,
         * for example <tt> DataObjectInterface< double > </tt>.
         */
        template <class _DataObjectType>
        class ReadPortInterface
        {
        public:
            /**
             * @brief The type of data the Port will read
             */
            typedef _DataObjectType DataObjectType;

            /**
             * @brief This is a helper class for use in the
             * NSControlKernel (it is nonnensical in other kernels).
             */
            template < class _DataType >
            struct DataObject
            {
                typedef typename _DataObjectType::template DataObject< _DataType >::type type;
            };

            virtual ~ReadPortInterface() {}

            /**
             * @brief Read from the following data object.
             *
             * @param _data The data to be added.
             */
            virtual void readFrom( const DataObjectType* _data ) = 0;

            /**
             * @brief No longer read from the following data object.
             *
             * @param _data The data to be removed.
             */
            virtual void disconnect( const DataObjectType* _data ) = 0;

            /**
             * @brief Inspect the port if the data it is pointing to is updated.
             */
            virtual bool isUpdated() const = 0;

            /**
             * @brief Checked access to the DataObject.
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

            void readFrom( const DataObjectType* _data ) 
            { 
                if(_dObj == 0) 
                    {
                        _dObj = _data; 
                    }
            }

            void disconnect( const DataObjectType* _data ) 
            { if (_dObj == _data) _dObj = 0; }

            bool isUpdated() const { return true; }

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

#if 0 //  Not used
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

            void readFrom( const DataObjectType* _data ) 
            { 
                if(_dObj == 0) 
                    {
                        _dObj = _data; 
                        _dObj->event()->handlerAdd(this, Completer::None);
                    }
            }

            void disconnect( const DataObjectType* _data ) 
            { 
                if (_dObj == _data) 
                    {
                        _dObj = 0; 
                        _dObj->event()->handlerRemove(this, Completer::None);
                    }
            }

            bool isUpdated() const { return updated; }

            virtual void handleEvent( CallbackInterface* )
            {
                updated = true;
            }

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

#endif

        /**
         * @brief The DataObjectType templated interface of a port for
         * writing a \a _DataObjectType object.
         */
        template <class _DataObjectType>
        class WritePortInterface
        {
        public:
            /**
             * @brief The type of data the Port will read
             */
            typedef _DataObjectType DataObjectType;

            /**
             * @brief This is a helper class for use in the
             * NSControlKernel (it is nonnensical in other kernels).
             */
            template < class _DataType >
            struct DataObject
            {
                typedef typename _DataObjectType::template DataObject< _DataType >::type type;
            };

            virtual ~WritePortInterface() {}

            /**
             * @brief Read from the following data object.
             *
             * @param _data The data to be added.
             */
            virtual void writeTo( DataObjectType* _data ) = 0;

            /**
             * @brief No longer read from the following _data object.
             *
             * @param _data The data to be removed.
             */
            virtual void disconnect( DataObjectType* _data ) = 0;

            /**
             * @brief Checked access to the DataObject.
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

            void writeTo( DataObjectType* _data ) 
            { if(_dObj == 0) _dObj = _data; }

            void disconnect( DataObjectType* _data )
            { if (_dObj == _data) _dObj = 0; }

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
            NameServedDataObjectBase* data_object;
        public:
            typedef _DataObjectType DataObjectType;

            NameServeWritePort() : front_end(0), data_object(0) {}

            ~NameServeWritePort() {
                delete front_end;
                delete data_object;
            }

            /**
             * @brief Return the WritePortInterface of this Component.
             *
             * Usefull to get a pointer to this interface, like in
             * writePort()->dObj() to resolve ambiguity with the dObj()
             * method of the ReadPortInterfaces.
             */
            WritePortInterface<_DataObjectType>* writePort() {
                return this;
            }

            void createPort(const std::string& name, const std::string& prefix) 
            {
                // create the frontend (for access)
                // todo : do without name, frontends must be nameless servers but keep prefix
                front_end   = new DataObjectType( name, prefix );
            }

            void erasePort()
            {
                delete front_end;
                front_end = 0;
            }
            
            /**
             * @brief A Hook for the component to initialise or modifiy the dObj()
             * instance (the Port/Frontend).
             *
             * After this, the DataObject
             * Server is created which will take the contents of dObj() to
             * create the instances.
             */
            virtual void initDataObject() {}

            template< class _DataObjectContainer >
            void createDataObject(const std::string& name, const std::string& prefix, _DataObjectContainer dummy )
            {
                this->initDataObject();
                // create the dataobject servers (for storage)
                typedef detail::NameServedDataObject<typename _DataObjectContainer::template DataObject<typename DataObjectType::NamesTypes> > DataStorage;
                DataStorage* storage = new DataStorage( name, prefix, false ); // do not create yet !
                //assert( front_end != 0);

                // Now create all the DataObjects, with the front_end data.
                storage->reload( front_end->begin(), front_end->end() );
                data_object = storage;
            }

            /**
             * Recreate the DataObjects of this Port, using the 
             * FrontEnd. This is intended when a loaded component
             * changes the contents of the ServedTypes structure
             * and wants the new types to be constructed. This is
             * part of a component reload. Only new DataObjects are
             * added, old ones remain.
             */
            template< class _DataObjectContainer >
            void reloadDataObject( _DataObjectContainer dummy )
            {
                // Reload the DataObject struct, it may be changed by
                // the writing component.
                typedef detail::NameServedDataObject<typename _DataObjectContainer::template DataObject<typename DataObjectType::NamesTypes> > DataStorage;
                DataStorage* current = dynamic_cast<DataStorage*>(data_object);
                //assert( current != 0 );
                current->reload( front_end->begin(), front_end->end() );
            }

            void eraseDataObject()
            {
                delete data_object;
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
     * @brief Read-port construction, Use this class to construct a component
     * which expects certain types in its DataObjects.
     * @param DataList A list of ServedTypes containing all the expected Data types.
     */
    template< typename DataList >
    struct Expects
    {
        /**
         * Our DataObjectType is a NameServedDataObject of DataObjectInterfaces
         */
        typedef typename detail::NamesDOFactory<DataList>::interface DataObjectType;

        typedef typename detail::NamesDOFactory<DataList>::interface::DataType DataType;

        /**
         * The ReadPort is an NameServeReadPort
         */
        typedef detail::NameServeReadPort<DataObjectType>    ReadPort;
    };

    /**
     * @brief Write-port construction, Use this class to construct a component
     * which writes/produces certain types in its DataObjects.
     * @param DataList A list of ServedTypes containing all the to-be produced Data types.
     */
    template< typename DataList >
    struct Writes
    {
        /**
         * Our DataObjectType is a NameServedDataObject of DataObjectInterfaces
         */
        typedef typename detail::NamesDOFactory<DataList>::interface DataObjectType;
        typedef typename detail::NamesDOFactory<DataList>::interface::DataType DataType;

        /**
         * The WritePort is an NameServeWritePort
         */
        typedef detail::NameServeWritePort<DataObjectType>    WritePort;
    };
}

#endif
    
