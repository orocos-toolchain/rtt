#include <iostream>
#include "Property.hpp"
#include "SimpleDemarshaller.hpp"
#include "SimpleMarshaller.hpp"


int main()
{
	using namespace std;
	using namespace StoneHead;

    PropertyBag bag;

    SimpleDemarshaller<istream> ds( cin );
    ds.deserialize( bag );

    SimpleMarshaller<ostream> ms( cout );
    ms.serialize( bag );
    return 0;
}
