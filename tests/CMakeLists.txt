IF (ENABLE_TESTS)
  INCLUDE(CTest)

    # See ADD_TEST() below.
    ENABLE_TESTING()
    INCLUDE_DIRECTORIES( ${PROJ_SOURCE_DIR}/src ${PROJ_BINARY_DIR}/src ${PROJ_BINARY_DIR}/src/os ${PROJ_BINARY_DIR}/src/os/${OROCOS_TARGET})
    INCLUDE_DIRECTORIES( ${OROCOS-RTT_INCLUDE_DIRS} )

    LINK_DIRECTORIES( ${PROJ_BINARY_DIR}/src )
    IF(ENABLE_CORBA)
      INCLUDE_DIRECTORIES( ${PROJ_BINARY_DIR}/src/corba/ ${CORBA_INCLUDE_DIRS})
      LINK_DIRECTORIES( ${PROJ_BINARY_DIR}/src/corba/ ${CORBA_LINK_DIRECTORIES})
      # Work around unresolved symbols in tests when using CORBA *and* STATIC
      IF(BUILD_STATIC)
	LINK_LIBRARIES( orocos-rtt-corba-${OROCOS_TARGET} orocos-rtt-${OROCOS_TARGET} orocos-rtt-corba-${OROCOS_TARGET} orocos-rtt-${OROCOS_TARGET})
      ENDIF(BUILD_STATIC)
    ENDIF(ENABLE_CORBA)

    #EXCLUDE_FROM_ALL is cmake 2.4.4
    SET(DO_EXCLUDE)
    IF (${CMAKE_VERSION} STRGREATER "2.4.3" )
      SET(DO_EXCLUDE EXCLUDE_FROM_ALL)
    ENDIF (${CMAKE_VERSION} STRGREATER "2.4.3" )

    # Copy over CPF files.
    CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/property_test_vect.cpf 
                   ${CMAKE_CURRENT_BINARY_DIR}/property_test_vect.cpf COPYONLY)

    # repeat for each test 
    ADD_EXECUTABLE( core-test ${DO_EXCLUDE} test-runner.cpp  buffers_test.cpp logger_test.cpp property_test.cpp time_test.cpp )
    TARGET_LINK_LIBRARIES( core-test orocos-rtt-${OROCOS_TARGET} )
    SET_TARGET_PROPERTIES( core-test PROPERTIES
    COMPILE_FLAGS "${CMAKE_CXX_FLAGS_ADD}" 
    COMPILE_DEFINITIONS ${OROCOS-RTT_DEFINITIONS} )
    ADD_TEST( core-test ${CMAKE_CURRENT_BINARY_DIR}/core-test )

    ADD_EXECUTABLE( task-test ${DO_EXCLUDE} test-runner.cpp tasks_test.cpp taskthread_test.cpp tasks_multiple_test.cpp )
    TARGET_LINK_LIBRARIES( task-test orocos-rtt-${OROCOS_TARGET} )
    SET_TARGET_PROPERTIES( task-test PROPERTIES
    COMPILE_FLAGS "${CMAKE_CXX_FLAGS_ADD}" 
    COMPILE_DEFINITIONS ${OROCOS-RTT_DEFINITIONS} )
    ADD_TEST( task-test ${CMAKE_CURRENT_BINARY_DIR}/task-test )

    ADD_EXECUTABLE( event-test ${DO_EXCLUDE} test-runner.cpp event_test.cpp eventservice_test.cpp )
    TARGET_LINK_LIBRARIES( event-test orocos-rtt-${OROCOS_TARGET} )
    SET_TARGET_PROPERTIES( event-test PROPERTIES
    COMPILE_FLAGS "${CMAKE_CXX_FLAGS_ADD}" 
    COMPILE_DEFINITIONS ${OROCOS-RTT_DEFINITIONS} )
    ADD_TEST( event-test ${CMAKE_CURRENT_BINARY_DIR}/event-test )

    ADD_EXECUTABLE( taskcontext-test ${DO_EXCLUDE} test-runner.cpp generictask_test.cpp generictask_test_2.cpp generictask_test_3.cpp)
    TARGET_LINK_LIBRARIES( taskcontext-test orocos-rtt-${OROCOS_TARGET} )
    SET_TARGET_PROPERTIES( taskcontext-test PROPERTIES
    COMPILE_FLAGS "${CMAKE_CXX_FLAGS_ADD}" 
    COMPILE_DEFINITIONS ${OROCOS-RTT_DEFINITIONS} )
    ADD_TEST( taskcontext-test ${CMAKE_CURRENT_BINARY_DIR}/taskcontext-test )

    ADD_EXECUTABLE( parser-test ${DO_EXCLUDE} test-runner.cpp types_test.cpp template_factory_test.cpp dispatch_test.cpp )
    TARGET_LINK_LIBRARIES( parser-test orocos-rtt-${OROCOS_TARGET} )
    SET_TARGET_PROPERTIES( parser-test PROPERTIES
    COMPILE_FLAGS "${CMAKE_CXX_FLAGS_ADD}" 
    COMPILE_DEFINITIONS ${OROCOS-RTT_DEFINITIONS} )
    ADD_TEST( parser-test ${CMAKE_CURRENT_BINARY_DIR}/parser-test )

    ADD_EXECUTABLE( program-test ${DO_EXCLUDE} test-runner.cpp program_test.cpp function_test.cpp )
    TARGET_LINK_LIBRARIES( program-test orocos-rtt-${OROCOS_TARGET} )
    SET_TARGET_PROPERTIES( program-test PROPERTIES
    COMPILE_FLAGS "${CMAKE_CXX_FLAGS_ADD}" 
    COMPILE_DEFINITIONS ${OROCOS-RTT_DEFINITIONS} )
    ADD_TEST( program-test ${CMAKE_CURRENT_BINARY_DIR}/program-test )

    ADD_EXECUTABLE( state-test ${DO_EXCLUDE} test-runner.cpp state_test.cpp )
    TARGET_LINK_LIBRARIES( state-test orocos-rtt-${OROCOS_TARGET} )
    SET_TARGET_PROPERTIES( state-test PROPERTIES
    COMPILE_FLAGS "${CMAKE_CXX_FLAGS_ADD}" 
    COMPILE_DEFINITIONS ${OROCOS-RTT_DEFINITIONS} )
    ADD_TEST( state-test ${CMAKE_CURRENT_BINARY_DIR}/state-test )

    ADD_EXECUTABLE( dev-test ${DO_EXCLUDE} test-runner.cpp dev_test.cpp )
    TARGET_LINK_LIBRARIES( dev-test orocos-rtt-${OROCOS_TARGET} )
    SET_TARGET_PROPERTIES( dev-test PROPERTIES
    COMPILE_FLAGS "${CMAKE_CXX_FLAGS_ADD}" 
    COMPILE_DEFINITIONS ${OROCOS-RTT_DEFINITIONS} )
    ADD_TEST( dev-test ${CMAKE_CURRENT_BINARY_DIR}/dev-test )
	
    IF(ENABLE_CORBA)
      ADD_EXECUTABLE( corba-test ${DO_EXCLUDE} test-runner.cpp corba_test.cpp )
      TARGET_LINK_LIBRARIES( corba-test orocos-rtt-${OROCOS_TARGET} orocos-rtt-corba-${OROCOS_TARGET} )
      SET_TARGET_PROPERTIES( corba-test PROPERTIES
	COMPILE_FLAGS "${CMAKE_CXX_FLAGS_ADD}" 
	COMPILE_DEFINITIONS ${OROCOS-RTT_DEFINITIONS} )
      ADD_TEST( corba-test corba-test )
      SET(ORO_EXTRA_TESTS "corba-test")
    ENDIF(ENABLE_CORBA)
	
    ADD_CUSTOM_TARGET(check ctest -V 
      DEPENDS core-test task-test event-test taskcontext-test parser-test program-test state-test dev-test ${ORO_EXTRA_TESTS}
        WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")
    
    #ADD_DEPENDENCIES(check prep-cpf)
ENDIF(ENABLE_TESTS)