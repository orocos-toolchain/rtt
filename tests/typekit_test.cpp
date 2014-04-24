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

#include <types/SequenceTypeInfo.hpp>

struct TypekitFixture
{
};

// Registers the fixture into the 'registry'
BOOST_FIXTURE_TEST_SUITE( TypekitTestSuite, TypekitFixture )

//! Tests the SequenceTypeInfo class.
BOOST_AUTO_TEST_CASE( testVectorTypeInfo )
{
    Types()->addType( new types::SequenceTypeInfo<std::vector<std::string> >("strings") );
#if 0 // not supported
    Types()->addType( new types::SequenceTypeInfo<std::vector<bool> >("bools") );
#endif

    Types()->addType( new types::SequenceTypeInfo<std::vector<int> >("ints") );
}

//! This test tries to compose/decompose a default built variable of
//! every known type. So this is not a test covering the nominal case...
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
        if ( input && output && ti->decomposeType(input->getDataSource()) ) {
            BOOST_CHECK_MESSAGE( ti->composeType( ti->decomposeType(input->getDataSource()), output->getDataSource()), "Decomposition/Composition of " + *it + " failed!" );
        }
    }
}

BOOST_AUTO_TEST_SUITE_END()
