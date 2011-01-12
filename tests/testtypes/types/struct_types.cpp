#include <rtt/types/Types.hpp>
#include "types/StructTypeInfo.hpp"
#include "../../datasource_fixture.hpp"

void loadStructTypes()
{
    // classical structs from datasource fixture
    Types()->addType( new StructTypeInfo< AType >("AType") );
    Types()->addType( new StructTypeInfo< BType >("BType") );
    Types()->addType( new StructTypeInfo< CType >("CType") );
}
