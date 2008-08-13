#include "CorbaConnection.hpp"
#include "../PortInterface.hpp"
#include "CorbaLib.hpp"
#include "ExpressionProxy.hpp"

namespace RTT
{

    namespace Corba
    {

        CorbaConnection::CorbaConnection(const std::string& name,
                DataFlowInterface_ptr dflow, PortableServer::POA_ptr) :
            mname(name), mdatachannel(0), mbufchannel(0),
                    mdflow(DataFlowInterface::_duplicate(dflow) )
        {
        }
        CorbaConnection::CorbaConnection(const std::string& name,
                DataFlowInterface_ptr dflow,
                AssignableExpression_ptr datachannel, PortableServer::POA_ptr) :
                    mname(name),
                    mdatachannel(AssignableExpression::_duplicate(datachannel)),
                    mbufchannel(0),
                    mdflow(DataFlowInterface::_duplicate(dflow) )

        {

        }
        CorbaConnection::CorbaConnection(const std::string& name,
                DataFlowInterface_ptr dflow, BufferChannel_ptr bufchannel,
                PortableServer::POA_ptr) :
            mname(name), mdatachannel(0),
                    mbufchannel(BufferChannel::_duplicate(bufchannel)),
                    mdflow(DataFlowInterface::_duplicate(dflow) )
        {

        }

        CorbaConnection::~CorbaConnection()
        {
        }

        DataSourceBase::shared_ptr CorbaConnection::getDataSource() const
        {
            if (lcc)
                return lcc->getDataSource();
            detail::TypeTransporter* tt = getTypeInfo()->getProtocol( ORO_CORBA_PROTOCOL_ID );
            // getDataSource returns data value or buffer's front() value
            // Data case
            if (tt && mdflow->getConnectionModel( mname.c_str() ) == DataFlowInterface::Data) {
                if (mdatachannel.in() )
                    return tt->dataProxy( (void*)mdatachannel.in() );
                return tt->dataProxy( mdflow->createDataChannel(mname.c_str() ) );
            }
            // Buffer case
            if (tt && mdflow->getConnectionModel( mname.c_str() ) == DataFlowInterface::Buffered) {
                // return a data source proxy to front().
                // make sure there is a remote buffer.
                BufferChannel_var buf = mdflow->createBufferChannel(mname.c_str());
                // get the front() data object
                Expression_var front = mdflow->createDataObject(mname.c_str());
                if ( front.in() == 0 )
                    return DataSourceBase::shared_ptr();
                // convert to local data object
                DataSourceBase::shared_ptr ds = ExpressionProxy::CreateDataSource( front.in() );
                DataSourceBase::shared_ptr ret = tt->narrowDataSource( ds.get() );
                if (ret)
                    return ret; // full conversion succesful.
                return ds;      // return proxy (basic functionality).
            }
            assert( tt && "CORBA should always return a fallback protocol!");
            return DataSourceBase::shared_ptr();

        }

        BufferBase::shared_ptr CorbaConnection::getBuffer() const
        {
            if (lcc)
                return lcc->getBuffer();
            detail::TypeTransporter* tt = getTypeInfo()->getProtocol( ORO_CORBA_PROTOCOL_ID );
            if (tt && mdflow->getConnectionModel(mname.c_str()) == DataFlowInterface::Buffered) {
                if ( mbufchannel.in() )
                    return BufferBase::shared_ptr( tt->bufferProxy( (void*) mbufchannel.in() ) );
                return BufferBase::shared_ptr( tt->bufferProxy( mdflow->createBufferChannel(mname.c_str() ) ) );
            }
            assert( tt && "CORBA should always return a fallback protocol!");
            return BufferBase::shared_ptr();
        }
        bool CorbaConnection::connect()
        {
            this->mconnected = true;
            if (lcc)
                return lcc->connect();
            return true;
        }

        bool CorbaConnection::connected() const
        {
            if (lcc)
                return lcc->connected();
            return mdflow->isConnected( mname.c_str() );
        }

        bool CorbaConnection::disconnect()
        {
            this->mconnected = false;
            if (lcc)
                return lcc->disconnect();
            return true;
        }

        bool CorbaConnection::addPort(PortInterface* r)
        {
            if (!lcc)
                return this->createConnection(r);
            else
                return lcc->addPort(r);
        }

        bool CorbaConnection::removePort(PortInterface* r)
        {
            if (!lcc)
                return ConnectionInterface::removePort(r);
            else
                return lcc->removePort(r);
        }

        int CorbaConnection::serverProtocol()
        {
            return ORO_CORBA_PROTOCOL_ID;
        }

        bool CorbaConnection::createConnection(PortInterface* p)
        {
            assert(!lcc);
            if ( p->serverProtocol() != 0) {
                //ConnectionInterface::addPort(p); // store for later retrieval.
                return true;
            }
            CORBA::String_var tname = mdflow->getDataType(mname.c_str());
            if (p->getConnectionModel() != int(mdflow->getConnectionModel(mname.c_str()))
                    || p->getTypeInfo()->getTypeName() != std::string( tname.in() ))
            {
                log(Error) << "Can not connect incompatible ports. Local: "
                           << p->getName() << ". Remote: " << mname<<endlog();
                log(Error) << "Local/Remote model: "<<  p->getConnectionModel() <<" " << mdflow->getConnectionModel(mname.c_str());
                return false;
            }

            // Create remote servers, and create connection with proxies to it.
            BufferBase::shared_ptr impl = getBuffer();
            if ( impl ) {
                log(Debug) << "Using scndry port to create bufferconnection."<<endlog();
                lcc = p->createConnection( impl );
                assert(lcc);
                lcc->addPort(p);
                for ( PList::iterator it( ports.begin() ); it != ports.end(); ++it)
                    lcc->addPort(*it);
                if ( mdflow->isConnected( mname.c_str() ) )
                    return lcc->connect();
                return true;
            }
            DataSourceBase::shared_ptr ds = getDataSource();
            if ( ds ) {
                log(Debug) << "Using scndry port to create dataconnection."<<endlog();
                lcc = p->createConnection( ds );
                assert(lcc);
                lcc->addPort(p);
                for ( PList::iterator it( ports.begin() ); it != ports.end(); ++it)
                    lcc->addPort(*it);
                if ( mdflow->isConnected( mname.c_str() ) )
                    return lcc->connect();
                return true;
            }
            // failed to create: store port for later adding.
            ConnectionInterface::addPort( p );
            return true;
        }

        const TypeInfo* CorbaConnection::getTypeInfo() const {
            TypeInfo* ret = TypeInfoRepository::Instance()->type( mdflow->getDataType( mname.c_str()) ) ;
            if (ret) return ret;
            return detail::DataSourceTypeInfo<detail::UnknownType>::getTypeInfo();
        }

    }

}
