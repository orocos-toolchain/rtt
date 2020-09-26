#include "rtt/types/Types.hpp"
#include "rtt/types/StructTypeInfo.hpp"
#include "tests/datasource_fixture.hpp"

void loadStructTypes()
{
    // classical structs from datasource fixture
    Types()->addType( new StructTypeInfo< AType >("AType") );
    Types()->addType( new StructTypeInfo< BType >("BType") );
    Types()->addType( new StructTypeInfo< CType >("CType") );
}
