#ifndef TESTCASEREPORTING_HPP
#define TESTCASEREPORTING_HPP

#include "TestCase.hpp"

#include <corelib/WriteCout.hpp>
#include <corelib/ObservableReadInterface.hpp>
#include <corelib/ReportWriter.hpp>
#include <corelib/ReportHandler.hpp>
#include <corelib/ReportingPropertyBag.hpp>
#include <corelib/marshalling/SimpleMarshaller.hpp>
#include <corelib/marshalling/XMLMarshaller.hpp>


namespace UnitTesting
{

    using namespace ORO_CoreLib;

    /**
     * A simple TestCase to test the Reporting component.
     */
    class TestCaseReporting :
        public TestCase
    {
    public:
        /**
         * Constructs a TestCaseReporting object.
         * @param suite  The test suite this test case is a part of.
         */
        TestCaseReporting( TestSuite* suite, WriteInterface* output );

        virtual bool initialize();

        virtual  void step();

        virtual void finalize();

        /**
         * Do cleanup in kernel space (delete) and destroy the test case.
         */
        virtual ~TestCaseReporting();

    protected:
        // Forbidden
        TestCaseReporting();

    private:
        /**
         * Stream for output to cout.
         */
        WriteInterface* out;

#ifndef HAVE_FIFO_RT        
        ObservableReadInterface* rtfIn;
        /**
         * Writes new fifo info to stdout
         */
        class Writer : public EventListenerInterface
        {
        public:
            Writer( WriteInterface*, ObservableReadInterface* );
            virtual void handleEvent( CallbackInterface* );
            WriteInterface* writer;
            ObservableReadInterface* reader;
        } notifier;
#endif
             
        ReportWriter* rw;
        typedef ReportingPropertyBag Reporter;
        Reporter* repBag;
        
        WriteInterface* rtf;
		std::string errorStr;

        /**
         * This property can be reported
         * by the ReportingPropertyBag.
         */
        Property<double> toMonitor;

    };

}

#endif // TESTCASESIMPLE_HPP

