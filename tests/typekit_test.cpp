/***************************************************************************
  tag: The SourceWorks  Tue Sep 7 00:54:57 CEST 2010  typekit_test.cpp

                        typekit_test.cpp -  description
                           -------------------
    begin                : Tue September 07 2010
    copyright            : (C) 2010 The SourceWorks
    email                : peter@thesourceworks.com

 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/


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

#include <types/VectorTemplateComposition.hpp>

struct TypekitFixture
{
};

// Registers the fixture into the 'registry'
BOOST_FIXTURE_TEST_SUITE( TypekitTestSuite, TypekitFixture )

//! Tests the StdVectorTemplateTypeInfo class.
BOOST_AUTO_TEST_CASE( testVectorTypeInfo )
{
    Types()->addType( new types::StdVectorTemplateTypeInfo<std::string,true>("strings") );
    Types()->type("strings")->addConstructor(newConstructor(types::stdvector_ctor<std::string>() ) );
    Types()->type("strings")->addConstructor(newConstructor(types::stdvector_ctor2<std::string>() ) );
    Types()->type("strings")->addConstructor(new types::StdVectorBuilder<std::string>() );

#if 1
    Types()->addType( new types::StdVectorTemplateTypeInfo<bool,true>("bools") );
    Types()->type("bools")->addConstructor(newConstructor(types::stdvector_ctor<bool>() ) );
    Types()->type("bools")->addConstructor(newConstructor(types::stdvector_ctor2<bool>() ) );
    Types()->type("bools")->addConstructor(new types::StdVectorBuilder<bool>() );
#endif

    Types()->addType( new types::StdVectorTemplateTypeInfo<int,true>("ints") );
    Types()->type("ints")->addConstructor(newConstructor(types::stdvector_ctor<int>() ) );
    Types()->type("ints")->addConstructor(newConstructor(types::stdvector_ctor2<int>() ) );
    Types()->type("ints")->addConstructor(new types::StdVectorBuilder<int>() );
}

BOOST_AUTO_TEST_CASE( testComposeDecompose )
{
    vector<string> names = Types()->getTypes();
    for(vector<string>::iterator it = names.begin(); it != names.end(); ++it) {
        TypeInfo* ti = Types()->type(*it);
        BOOST_REQUIRE(ti);
        // might return null in case of void:
        PropertyBase* input = ti->buildProperty("A","B");
        PropertyBase* output = ti->buildProperty("C","D");
        // if it's decomposable, compose it as well.
        if ( input && output && ti->decomposeType(input->getDataSource()) )
            BOOST_CHECK( ti->composeType( ti->decomposeType(input->getDataSource()), output->getDataSource()) );
    }
}

BOOST_AUTO_TEST_SUITE_END()
