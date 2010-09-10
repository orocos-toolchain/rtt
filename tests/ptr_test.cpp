/***************************************************************************
  tag: Peter Soetens  Mon Jan 10 15:59:51 CET 2005  buffers_test.cpp

                        buffers_test.cpp -  description
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


#include <boost/test/unit_test.hpp>
#include <boost/test/floating_point_comparison.hpp>

#include "ptr_test.hpp"

using namespace std;

using namespace RTT;
using namespace RTT::extras;

struct MyStruct
{
    static bool deleted;
    static bool copied;

    MyStruct() {}
    MyStruct(MyStruct const& s) { copied = true; }
    ~MyStruct() { deleted = true; }
    MyStruct const* get_this() const { return this; }
};
bool MyStruct::deleted = false;
bool MyStruct::copied = false;

BOOST_FIXTURE_TEST_SUITE(PtrTestSuite, PtrTest)

BOOST_AUTO_TEST_CASE( testReadOnly )
{
    MyStruct::deleted = false;
    MyStruct* value = new MyStruct;
    {
        // This takes ownership
        ReadOnlyPointer<MyStruct> ptr1(value);

        BOOST_CHECK(ptr1.valid());
        BOOST_CHECK(&(*ptr1) == value);
        BOOST_CHECK(ptr1->get_this() == value);

        // Try sharing the ownership and verify the object is not deleted
        { ReadOnlyPointer<MyStruct> ptr2(ptr1); }
        BOOST_CHECK(!MyStruct::deleted);
    }
    // No more ReadOnlyPointer on the object, it should be deleted now
    BOOST_CHECK(MyStruct::deleted);
}

BOOST_AUTO_TEST_CASE( testPromotion )
{
    MyStruct::deleted = false;
    MyStruct::copied = false;
    MyStruct* value = new MyStruct;
    MyStruct* write;

    // First test: no copy needed
    MyStruct::deleted = false;
    MyStruct::copied = false;
    {
        // This takes ownership
        ReadOnlyPointer<MyStruct> ptr1(value);
        // This should remove the ownership from ptr1, invalidate it and return
        // the original value
        write = ptr1.write_access();

        BOOST_CHECK(!ptr1.valid());
        BOOST_CHECK(write == value);
    }
    BOOST_CHECK(!MyStruct::copied);
    BOOST_CHECK(!MyStruct::deleted);

    // Second test: copy needed
    MyStruct::deleted = false;
    MyStruct::copied = false;
    {
        // This takes ownership
        ReadOnlyPointer<MyStruct> ptr1(value);
        // And this makes the ownership shared
        ReadOnlyPointer<MyStruct> ptr2(ptr1);
        // This should remove the ownership from ptr1 and invalidate it. But it
        // should return a copy.
        write = ptr1.write_access();

        //BOOST_CHECK(!ptr1.valid());
        BOOST_CHECK(write != value);

        BOOST_CHECK(MyStruct::copied);
        BOOST_CHECK(!MyStruct::deleted);
    } // and the original value should be deleted because of ptr2
    BOOST_CHECK(MyStruct::deleted);

    delete write;
}

BOOST_AUTO_TEST_SUITE_END()
