/**
 * typekit_test.cpp
 *
 *  Created on: May 10, 2010
 *      Author: kaltan
 */

#include "unit.hpp"

#include <types/TemplateTypeInfo.hpp>
#include <types/TemplateConstructor.hpp>
#include <types/Operators.hpp>
#include <types/OperatorTypes.hpp>
#include <types/RealTimeToolkit.hpp>

#include <types/VectorTemplateComposition.hpp>

struct TypekitFixture
{
};

// Registers the fixture into the 'registry'
BOOST_FIXTURE_TEST_SUITE( TypekitTestSuite, TypekitFixture )

//! Tests the StdVectorTemplateTypeInfo class.
BOOST_AUTO_TEST_CASE( testVectorTypeInfo )
{
    RTT::types::TypeInfoRepository::Instance()->addType( new types::StdVectorTemplateTypeInfo<std::string,true>("strings") );
    RTT::types::TypeInfoRepository::Instance()->type("strings")->addConstructor(newConstructor(types::stdvector_ctor<std::string>() ) );
    RTT::types::TypeInfoRepository::Instance()->type("strings")->addConstructor(newConstructor(types::stdvector_ctor2<std::string>() ) );
    RTT::types::TypeInfoRepository::Instance()->type("strings")->addConstructor(new types::StdVectorBuilder<std::string>() );

#if 1
    RTT::types::TypeInfoRepository::Instance()->addType( new types::StdVectorTemplateTypeInfo<bool,true>("bools") );
    RTT::types::TypeInfoRepository::Instance()->type("bools")->addConstructor(newConstructor(types::stdvector_ctor<bool>() ) );
    RTT::types::TypeInfoRepository::Instance()->type("bools")->addConstructor(newConstructor(types::stdvector_ctor2<bool>() ) );
    RTT::types::TypeInfoRepository::Instance()->type("bools")->addConstructor(new types::StdVectorBuilder<bool>() );
#endif

    RTT::types::TypeInfoRepository::Instance()->addType( new types::StdVectorTemplateTypeInfo<int,true>("ints") );
    RTT::types::TypeInfoRepository::Instance()->type("ints")->addConstructor(newConstructor(types::stdvector_ctor<int>() ) );
    RTT::types::TypeInfoRepository::Instance()->type("ints")->addConstructor(newConstructor(types::stdvector_ctor2<int>() ) );
    RTT::types::TypeInfoRepository::Instance()->type("ints")->addConstructor(new types::StdVectorBuilder<int>() );
}

BOOST_AUTO_TEST_SUITE_END()
