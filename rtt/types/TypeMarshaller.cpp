#include "TypeMarshaller.hpp"

RTT::types::TypeMarshaller::~TypeMarshaller()
{

}

void* RTT::types::TypeMarshaller::createCookie() const
{
    return 0;
}

void RTT::types::TypeMarshaller::deleteCookie(void* cookie) const
{

}

