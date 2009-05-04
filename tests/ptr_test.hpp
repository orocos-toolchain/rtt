/***************************************************************************
  tag: Peter Soetens  Mon Jan 10 15:59:18 CET 2005  buffers_test.hpp

                        buffers_test.hpp -  description
                           -------------------
    begin                : Mon January 10 2005
    copyright            : (C) 2005 Peter Soetens
    email                : peter.soetens@mech.kuleuven.ac.be

 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/


#ifndef BUFFERSTEST_H
#define BUFFERSTEST_H

#include <cppunit/extensions/HelperMacros.h>

#include <ReadWritePointer.hpp>

class PtrTest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE( PtrTest );
    CPPUNIT_TEST( testReadOnly );
    CPPUNIT_TEST( testPromotion );
    CPPUNIT_TEST_SUITE_END();
public:

    void testReadOnly();
    void testPromotion();
};

#endif
