 
#ifndef ORO_CORBA_DATAOBJECTPROXY_HPP
#define ORO_CORBA_DATAOBJECTPROXY_HPP


#include "../DataObjectInterfaces.hpp"
#include "../DataSources.hpp"
#include "OperationsC.h"

namespace RTT
{ namespace Corba {


    /**
     * A class which provides access to remote Corba Data.
     * It is constructed proxy-side and Set/Get operations
     * are sent or received from the Event Service. It
     * hence \b mirrors remote data.
     *
     * @ingroup CoreLibBuffers
     */
    template<class T>
    class CorbaDataObjectProxy
        : public DataObjectInterface<T>
    {
        std::string name;
        AssignableExpression_var mec;
    public:
        /** 
         * Construct a CorbaDataObject which uses the Corba Event Service
         * as transport medium.
         * 
         * @param _name The name of this CorbaDataObject.
         * @param ec The event service which transfers data of type \a T
         */
        CorbaDataObjectProxy(const std::string& _name, AssignableExpression_ptr ec )
            : name(_name), mec(AssignableExpression::_duplicate(ec) ) 
        {
        }

        /**
         * Destructor
         */
        ~CorbaDataObjectProxy() {
        }
        
        /** 
         * Return the name of this CorbaDataObject.
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
        void Get( DataType& pull ) const {
            Logger::In in("CorbaDataObjectProxy::Get");
            log(Debug) << "Reading DataObject value."<<endlog();
            CORBA::Any_var v;
            v = mec->get();
            ReferenceDataSource<T> rds(pull);
            rds.ref();
            if ( rds.update( v.in() ) == false) {
                log(Error) << "Could not read remote value: wrong data type."<<endlog();
                return;
            }
        }

        /**
         * Get a copy of the data of the module.
         *
         * @return The result of the module.
         */
        DataType Get() const { DataType p; this->Get(p); return p; }
            
        /**
         * Set the data to a certain value.
         *
         * @param push The data which must be set.
         */
        void Set( const DataType& push ) { 
            Logger::In in("CorbaDataObjectProxy::Set");
            log(Debug) << "Sending DataObject value."<<endlog();
            // Trick: we must create a datasource to convert 'push' to
            // an Any. We do not convert it ourselves, since this would
            // invoke Get() first.
            ValueDataSource<T> vds(push);
            vds.ref();
            CORBA::Any_var toset = vds.createAny();
            mec->set( toset.in() ); 
        }

        CorbaDataObjectProxy<DataType>* clone() const {
            return new CorbaDataObjectProxy<DataType>(name, mec.in());
        }

        CorbaDataObjectProxy<DataType>* copy( std::map<const DataSourceBase*, DataSourceBase*>&  ) const {
            return const_cast<CorbaDataObjectProxy<DataType>*>(this);
        }

    };
}}

#endif
    
