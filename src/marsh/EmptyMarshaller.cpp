#include "EmptyMarshaller.hpp"

using namespace RTT;

EmptyMarshaller::EmptyMarshaller()
{
}

EmptyMarshaller::~EmptyMarshaller() {}

void EmptyMarshaller::flush() {}

void EmptyMarshaller::serialize(PropertyBase* v)
{
}

void EmptyMarshaller::serialize(const PropertyBag &v)
{
}
