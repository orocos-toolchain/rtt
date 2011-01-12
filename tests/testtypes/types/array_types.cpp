#include "types/Types.hpp"
#include "types/CArrayTypeInfo.hpp"
#include "types/BoostArrayTypeInfo.hpp"
#include "../../datasource_fixture.hpp"
#include <rtt/types/SequenceTypeInfo.hpp>

void loadArrayTypes()
{
    // these are all execised by the datasource_fixture:

    // 3 types of arrays: carray, boost array and std::vector
    Types()->addType( new CArrayTypeInfo< carray<int> >("int[]") );
    Types()->addType( new BoostArrayTypeInfo< boost::array<int,5> >("int5") );
    Types()->addType( new SequenceTypeInfo< vector<int> >("ints") );

    Types()->addType( new CArrayTypeInfo< carray<double> >("double[]") );
    Types()->addType( new CArrayTypeInfo< carray<char> >("char[]") );
}
