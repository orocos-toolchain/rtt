/***************************************************************************
  tag: Peter Soetens  Thu Oct 10 16:17:00 CEST 2002  TestCaseCPFMarshaller.hpp 

                        TestCaseCPFMarshaller.hpp -  description
                           -------------------
    begin                : Thu October 10 2002
    copyright            : (C) 2002 Peter Soetens
    email                : peter.soetens@mech.kuleuven.ac.be
 
 ***************************************************************************
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU Lesser General Public            *
 *   License as published by the Free Software Foundation; either          *
 *   version 2.1 of the License, or (at your option) any later version.    *
 *                                                                         *
 *   This library is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU     *
 *   Lesser General Public License for more details.                       *
 *                                                                         *
 *   You should have received a copy of the GNU Lesser General Public      *
 *   License along with this library; if not, write to the Free Software   *
 *   Foundation, Inc., 59 Temple Place,                                    *
 *   Suite 330, Boston, MA  02111-1307  USA                                *
 *                                                                         *
 ***************************************************************************/ 
 

#ifndef TESTCASECPFMARSHALLER_HPP
#define TESTCASECPFMARSHALLER_HPP

#include "TestCase.hpp"
//#include <iostream>
#include <corelib/Property.hpp>
#include <corelib/marshalling/CPFMarshaller.hpp>

namespace UnitTesting
{

    using namespace ORO_CoreLib;

    /**
     * A simple TestCase to test the TestCase class ;-)
     */

    class TestCaseCPFMarshaller :
                public TestCase
    {

        public:
            /**
             * Constructs a TestCaseCPFMarshaller object.
             * @param suite  The test suite this test case is a part of.
             */
            TestCaseCPFMarshaller( TestSuite* suite );

            /**
             * Initialise the simple test case, that is, give a message.
             */
            virtual bool initialize();

            /**
             * This function will be executed periodically. It just gives a message.
             */
            virtual  void step();

            /**
             * Cleanup the test case after execution. It just gives a message.
             */
            virtual void finalize();

            /**
             * Do cleanup in kernel space (delete) and destroy the test case.
             */
            virtual ~TestCaseCPFMarshaller();

        protected:

            // Forbidden
            TestCaseCPFMarshaller();

        private:

            /**
             * Counter for the number of steps.
             */
            int stepCounter;

            /**
             * The CPF marshaller doesn't work
             */
            std::string errorMarshallingAsCPF;

            /**
             * Test flag.
             */
            bool isSet;
    
			//PropertyBag bag;

    };

}

#endif // TESTCASECPFMARSHALLER_HPP

