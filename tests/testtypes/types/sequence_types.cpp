#include <rtt/types/Types.hpp>
#include "types/SequenceTypeInfo.hpp"
#include "../../datasource_fixture.hpp"

void loadSequenceTypes()
{
    // sequence of sequences
    Types()->addType( new SequenceTypeInfo<std::vector<std::vector<double> > >("matrix") );

    // sequences of the structs
    Types()->addType( new SequenceTypeInfo< vector<AType> >("ATypes") );
    Types()->addType( new SequenceTypeInfo< vector<BType> >("BTypes") );
    Types()->addType( new SequenceTypeInfo< vector<CType> >("CTypes") );

}
