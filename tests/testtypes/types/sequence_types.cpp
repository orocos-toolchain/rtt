#include "rtt/types/Types.hpp"
#include "rtt/types/SequenceTypeInfo.hpp"
#include "tests/datasource_fixture.hpp"

void loadSequenceTypes()
{
    // sequence of sequences
    Types()->addType( new SequenceTypeInfo<std::vector<std::vector<double> > >("matrix") );

    // sequences of the structs
    Types()->addType( new SequenceTypeInfo< vector<AType> >("ATypes") );
    Types()->addType( new SequenceTypeInfo< vector<BType> >("BTypes") );
    Types()->addType( new SequenceTypeInfo< vector<CType> >("CTypes") );

}
