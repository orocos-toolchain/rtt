
    # See ADD_TEST() below.
    ENABLE_TESTING()

    INCLUDE_DIRECTORIES( ${PROJ_SOURCE_DIR}/src ${PROJ_BINARY_DIR}/src ${PROJ_BINARY_DIR}/src/os ${PROJ_BINARY_DIR}/src/os/${OROCOS_TARGET})
    ADD_DEFINITIONS("-g")

    # repeat for each test
    ADD_EXECUTABLE( core-test EXCLUDE_FROM_ALL test-runner.cpp time_test.cpp property_test.cpp logger_test.cpp buffers_test.cpp )
    TARGET_LINK_LIBRARIES( core-test orocos-rtt cppunit )
    ADD_TEST( core-test core-test )

    ADD_EXECUTABLE( task-test EXCLUDE_FROM_ALL test-runner.cpp taskthread_test.cpp tasks_test.cpp tasks_multiple_test.cpp )
    TARGET_LINK_LIBRARIES( task-test orocos-rtt cppunit )
    ADD_TEST( task-test task-test )

    ADD_EXECUTABLE( event-test EXCLUDE_FROM_ALL test-runner.cpp event_test.cpp eventservice_test.cpp )
    TARGET_LINK_LIBRARIES( event-test orocos-rtt cppunit )
    ADD_TEST( event-test event-test )

    ADD_EXECUTABLE( taskcontext-test EXCLUDE_FROM_ALL test-runner.cpp generictask_test.cpp )
    TARGET_LINK_LIBRARIES( taskcontext-test orocos-rtt cppunit )
    ADD_TEST( taskcontext-test taskcontext-test )

    ADD_EXECUTABLE( parser-test EXCLUDE_FROM_ALL test-runner.cpp types_test.cpp template_factory_test.cpp dispatch_test.cpp )
    TARGET_LINK_LIBRARIES( parser-test orocos-rtt cppunit )
    ADD_TEST( parser-test parser-test )
    
    ADD_EXECUTABLE( program-test EXCLUDE_FROM_ALL test-runner.cpp program_test.cpp function_test.cpp )
    TARGET_LINK_LIBRARIES( program-test orocos-rtt cppunit)
    ADD_TEST( program-test program-test )

    ADD_EXECUTABLE( state-test EXCLUDE_FROM_ALL test-runner.cpp state_test.cpp )
    TARGET_LINK_LIBRARIES( state-test orocos-rtt cppunit )
    ADD_TEST( state-test state-test )


ADD_CUSTOM_TARGET(check "ctest" 
  DEPENDS core-test task-test event-test taskcontext-test parser-test program-test state-test
  WORKING_DIRECTORY ${PROJ_BINARY_DIR}/tests)