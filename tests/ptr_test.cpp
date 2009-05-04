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



#include "ptr_test.hpp"

using namespace std;

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( PtrTest );

using namespace RTT;

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

void PtrTest::testReadOnly()
{
    MyStruct::deleted = false;
    MyStruct* value = new MyStruct;
    {
        // This takes ownership
        ReadOnlyPointer<MyStruct> ptr1(value);

        CPPUNIT_ASSERT(ptr1.valid());
        CPPUNIT_ASSERT(&(*ptr1) == value);
        CPPUNIT_ASSERT(ptr1->get_this() == value);

        // Try sharing the ownership and verify the object is not deleted
        { ReadOnlyPointer<MyStruct> ptr2(ptr1); }
        CPPUNIT_ASSERT(!MyStruct::deleted);
    }
    // No more ReadOnlyPointer on the object, it should be deleted now
    CPPUNIT_ASSERT(MyStruct::deleted);
}

void PtrTest::testPromotion()
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

        CPPUNIT_ASSERT(!ptr1.valid());
        CPPUNIT_ASSERT(write == value);
    }
    CPPUNIT_ASSERT(!MyStruct::copied);
    CPPUNIT_ASSERT(!MyStruct::deleted);

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

        CPPUNIT_ASSERT(!ptr1.valid());
        CPPUNIT_ASSERT(write != value);

        CPPUNIT_ASSERT(MyStruct::copied);
        CPPUNIT_ASSERT(!MyStruct::deleted);
    } // and the original value should be deleted because of ptr2
    CPPUNIT_ASSERT(MyStruct::deleted);

    delete write;
}

