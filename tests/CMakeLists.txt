IF ( BUILD_TESTS)

    # This gathers all the .cpp files into the variable 'SRCS'
    ENABLE_TESTING()
    INCLUDE_DIRECTORIES( ${PROJ_SOURCE_DIR}/src ${PROJ_BINARY_DIR}/src ${PROJ_BINARY_DIR}/src/os ${PROJ_BINARY_DIR}/src/os/${OROCOS_TARGET})
    ADD_DEFINITIONS("-g")

    # repeat for each test
    ADD_EXECUTABLE( core-test test-runner.cpp time_test.cpp property_test.cpp logger_test.cpp buffers_test.cpp )
    TARGET_LINK_LIBRARIES( core-test orocos-rtt cppunit )
    ADD_TEST( core-test core-test )

    ADD_EXECUTABLE( task-test test-runner.cpp taskthread_test.cpp tasks_test.cpp tasks_multiple_test.cpp )
    TARGET_LINK_LIBRARIES( task-test orocos-rtt cppunit )
    ADD_TEST( task-test task-test )

    ADD_EXECUTABLE( event-test test-runner.cpp event_test.cpp eventservice_test.cpp )
    TARGET_LINK_LIBRARIES( event-test orocos-rtt cppunit )
    ADD_TEST( event-test event-test )

    ADD_EXECUTABLE( taskcontext-test test-runner.cpp generictask_test.cpp )
    TARGET_LINK_LIBRARIES( taskcontext-test orocos-rtt cppunit )
    ADD_TEST( taskcontext-test taskcontext-test )

    ADD_EXECUTABLE( parser-test test-runner.cpp types_test.cpp template_factory_test.cpp dispatch_test.cpp )
    TARGET_LINK_LIBRARIES( parser-test orocos-rtt cppunit )
    ADD_TEST( parser-test parser-test )
    
    ADD_EXECUTABLE( program-test test-runner.cpp program_test.cpp function_test.cpp )
    TARGET_LINK_LIBRARIES( program-test orocos-rtt cppunit)
    ADD_TEST( program-test program-test )

    ADD_EXECUTABLE( state-test test-runner.cpp state_test.cpp )
    TARGET_LINK_LIBRARIES( state-test orocos-rtt cppunit )
    ADD_TEST( state-test state-test )

ENDIF ( BUILD_TESTS)
