
Include(AddUnitTest.cmake)

IF (BUILD_TESTING)
   if (NOT RUNTIME_OUTPUT_DIRECTORY)
        set(RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
   endif()
   if ( NOT Boost_UNIT_TEST_FRAMEWORK_FOUND )
     message(FATAL_ERROR "Can't build unit tests without Boost unit_test_framework.")
   endif()

   SET(BUILD_TEST_RT_MEM_POOL_SIZE "20000" CACHE STRING "Size of real-time memory pool used by test cases (bytes)")
   SET(COMPILE_DEFS "${OROCOS-RTT_DEFINITIONS};BUILD_TEST_RT_MEM_POOL_SIZE=${BUILD_TEST_RT_MEM_POOL_SIZE}")

   IF (OROCOS_TARGET STREQUAL win32)
      #SET(TEST_LIBRARIES ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})
      LIST(APPEND OROCOS-RTT_DEFINITIONS BOOST_TEST_NO_LIB)
      set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /EHa") # required for boost.test's use of _set_se_translator()
   ENDIF()

   if (PLUGINS_ENABLE_MARSHALLING)
       list(APPEND MARSHALLING_LIBRARIES rtt-marshalling-${OROCOS_TARGET}_plugin)
   endif()
   IF (NOT APPLE)
	 LIST(APPEND TEST_LIBRARIES ${Boost_SERIALIZATION_LIBRARY})
   ENDIF()
   if (PLUGINS_ENABLE_SCRIPTING)
      list(APPEND SCRIPTING_LIBRARIES rtt-scripting-${OROCOS_TARGET}_plugin)
   endif()

    # Due to generation of some .h files in build directories, we also need to include some build dirs in our include paths.
    INCLUDE_DIRECTORIES(${PROJ_SOURCE_DIR} ${PROJ_SOURCE_DIR}/rtt ${PROJ_SOURCE_DIR}/rtt/os/${OROCOS_TARGET} )
    INCLUDE_DIRECTORIES(${PROJ_BINARY_DIR}/rtt ${PROJ_BINARY_DIR}/rtt/os ${PROJ_BINARY_DIR}/rtt/os/${OROCOS_TARGET} )
    INCLUDE_DIRECTORIES(${PROJ_BINARY_DIR}/rtt/marsh ${PROJ_BINARY_DIR}/rtt/scripting )
    INCLUDE_DIRECTORIES( ${OROCOS-RTT_INCLUDE_DIRS} )

    LINK_DIRECTORIES( ${PROJ_BINARY_DIR}/rtt )

    # Setup fixtures library
    FILE( GLOB OP_FIXTURES operations_fixture*.cpp datasource_fixture.cpp)
    ADD_LIBRARY( fixtures SHARED ${OP_FIXTURES} )
    TARGET_LINK_LIBRARIES( fixtures orocos-rtt-${OROCOS_TARGET}_dynamic ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY} rtt-typekit-${OROCOS_TARGET}_plugin )
    LIST(APPEND TEST_LIBRARIES fixtures rtt-typekit-${OROCOS_TARGET}_plugin ${OROCOS-RTT_USER_LINK_LIBS} )
    SET_TARGET_PROPERTIES( fixtures PROPERTIES
      DEFINE_SYMBOL "RTT_UNIT_DLL_EXPORT"
      VERSION "${RTT_VERSION}"
      SOVERSION "${RTT_SOVERSION}"
      COMPILE_DEFINITIONS OROCOS_TARGET=${OROCOS_TARGET}
      )
    IF (UNIX AND NOT APPLE)
      SET_TARGET_PROPERTIES( fixtures PROPERTIES
         LINK_FLAGS "-Wl,-z,defs")
    ENDIF ()

    IF(ENABLE_CORBA)
      INCLUDE_DIRECTORIES( ${PROJ_BINARY_DIR}/rtt/transports/corba/ ${CORBA_INCLUDE_DIRS})
      LINK_DIRECTORIES( ${PROJ_BINARY_DIR}/rtt/transports/corba/ ${CORBA_LINK_DIRECTORIES})
      # Work around unresolved symbols in tests when using CORBA *and* STATIC
      IF(BUILD_STATIC)
	    LINK_LIBRARIES( orocos-rtt-corba-${OROCOS_TARGET} orocos-rtt-${OROCOS_TARGET} orocos-rtt-corba-${OROCOS_TARGET} orocos-rtt-${OROCOS_TARGET})
      ENDIF(BUILD_STATIC)
      if (NOT OROCOS_TARGET STREQUAL win32)
        add_definitions(-D_REENTRANT)
      endif()
    ENDIF(ENABLE_CORBA)
    IF(ENABLE_MQ)
      INCLUDE_DIRECTORIES( ${PROJ_BINARY_DIR}/rtt/transports/mqueue/)
      LINK_DIRECTORIES( ${PROJ_BINARY_DIR}/rtt/transports/mqueue/)
      # Work around unresolved symbols in tests when using MQUEUE *and* STATIC
      IF(BUILD_STATIC)
        LINK_LIBRARIES( orocos-rtt-mqueue-${OROCOS_TARGET} orocos-rtt-${OROCOS_TARGET} orocos-rtt-mqueue-${OROCOS_TARGET} orocos-rtt-${OROCOS_TARGET})
      ENDIF(BUILD_STATIC)
    ENDIF(ENABLE_MQ)

    # Copy over CPF files. It *must* be done like this to work on MSVC:
    add_custom_target(SetupTests ALL
      COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}
      COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/testPropMarshVectLegacy.cpf ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}
      COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/property_loading.cpf ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR})

    ADD_EXECUTABLE( main-test test-main.cpp )
    TARGET_LINK_LIBRARIES( main-test orocos-rtt-${OROCOS_TARGET}_dynamic ${OROCOS-RTT_USER_LINK_LIBS})
    SET_TARGET_PROPERTIES( main-test PROPERTIES
    COMPILE_DEFINITIONS "${COMPILE_DEFS}")
    ADD_TEST( main-test ${RUNTIME_OUTPUT_DIRECTORY}/main-test )

    if ( ${Boost_VERSION} GREATER 103599 )
      ADD_EXECUTABLE( list-test test-runner.cpp  listlocked_test.cpp )
      TARGET_LINK_LIBRARIES( list-test orocos-rtt-${OROCOS_TARGET}_dynamic ${TEST_LIBRARIES})
      SET_TARGET_PROPERTIES( list-test PROPERTIES
	COMPILE_DEFINITIONS "${COMPILE_DEFS}")
      ADD_TEST( list-test ${RUNTIME_OUTPUT_DIRECTORY}/list-test )
      list(APPEND ORO_EXTRA_TESTS "list-test")
    endif()

    # repeat for each test
    ADD_EXECUTABLE( core-test test-runner.cpp logger_test.cpp time_test.cpp ptr_test.cpp )
    TARGET_LINK_LIBRARIES( core-test orocos-rtt-${OROCOS_TARGET}_dynamic ${TEST_LIBRARIES})
    SET_TARGET_PROPERTIES( core-test PROPERTIES
    COMPILE_DEFINITIONS "${COMPILE_DEFS}")
    ADD_TEST( core-test ${RUNTIME_OUTPUT_DIRECTORY}/core-test )

    ADD_EXECUTABLE( task-test test-runner.cpp tasks_test.cpp taskthread_test.cpp taskthread_fd_test.cpp tasks_multiple_test.cpp )
    TARGET_LINK_LIBRARIES( task-test orocos-rtt-${OROCOS_TARGET}_dynamic ${TEST_LIBRARIES})
    SET_TARGET_PROPERTIES( task-test PROPERTIES
    COMPILE_DEFINITIONS "${COMPILE_DEFS}")
    ADD_TEST( task-test ${RUNTIME_OUTPUT_DIRECTORY}/task-test )

    IF(UNIX AND NOT OROCOS_TARGET STREQUAL "xenomai" )
      ADD_EXECUTABLE( specactivities-test test-runner.cpp
	specialized_activities.cpp)
      TARGET_LINK_LIBRARIES( specactivities-test orocos-rtt-${OROCOS_TARGET}_dynamic ${TEST_LIBRARIES})
      SET_TARGET_PROPERTIES( specactivities-test PROPERTIES
        COMPILE_DEFINITIONS "${COMPILE_DEFS}")
      ADD_TEST( specactivities-test ${RUNTIME_OUTPUT_DIRECTORY}/specactivities-test )
          list(APPEND ORO_EXTRA_TESTS "specactivities-test")
    ENDIF()

    IF(ENABLE_CORBA)
      ADD_EXECUTABLE( corba-test test-runner-corba.cpp corba_test.cpp )
      TARGET_LINK_LIBRARIES( corba-test 
            orocos-rtt-${OROCOS_TARGET}_dynamic 
            orocos-rtt-corba-${OROCOS_TARGET}_dynamic 
            ${CORBA_LIBRARIES} ${TEST_LIBRARIES})
      SET_TARGET_PROPERTIES( corba-test PROPERTIES
	   COMPILE_DEFINITIONS "${COMPILE_DEFS}"
	   )
      ADD_TEST( corba-test ${RUNTIME_OUTPUT_DIRECTORY}/corba-test )
      list(APPEND ORO_EXTRA_TESTS "corba-test")

        # This program requires the Unix 'system()' function and 'killall'. To be ported to windows.
        IF (UNIX)
           # Launches our servers.
           ADD_EXECUTABLE(setup_corba setup_corba.cpp)
           SET_TARGET_PROPERTIES( setup_corba PROPERTIES
                COMPILE_DEFINITIONS "${COMPILE_DEFS}"
           )
           # Launched before ctest.
           SET(CORBA_SETUP_COMMAND COMMAND setup_corba)
        ENDIF(UNIX)
       
          ADD_EXECUTABLE( corba-ipc-test test-runner-corba.cpp corba_ipc_test.cpp )
          TARGET_LINK_LIBRARIES( corba-ipc-test 
                orocos-rtt-${OROCOS_TARGET}_dynamic 
                orocos-rtt-corba-${OROCOS_TARGET}_dynamic 
                ${CORBA_LIBRARIES} ${TEST_LIBRARIES})
          SET_TARGET_PROPERTIES( corba-ipc-test PROPERTIES
            COMPILE_DEFINITIONS "${COMPILE_DEFS}"
            )
          ADD_TEST( corba-ipc-test ${RUNTIME_OUTPUT_DIRECTORY}/corba-ipc-test )
          list(APPEND ORO_EXTRA_TESTS "corba-ipc-test")
    
          # This server is required to run when the corba-ipc-test runs
          ADD_EXECUTABLE( corba-ipc-server corba_ipc_server.cpp )
          TARGET_LINK_LIBRARIES( corba-ipc-server 
                orocos-rtt-${OROCOS_TARGET}_dynamic 
                orocos-rtt-corba-${OROCOS_TARGET}_dynamic 
                ${CORBA_LIBRARIES} ${TEST_LIBRARIES})
          SET_TARGET_PROPERTIES( corba-ipc-server PROPERTIES
            COMPILE_DEFINITIONS "${COMPILE_DEFS}"
            )
          ADD_SIMPLE_TEST(test-corba-main ORO_EXTRA_TESTS "orocos-rtt-corba-${OROCOS_TARGET}_dynamic" ) 
    ENDIF(ENABLE_CORBA)

    IF(ENABLE_MQ)
      ADD_EXECUTABLE( mqueue-test test-runner.cpp mqueue_test.cpp )
      TARGET_LINK_LIBRARIES( mqueue-test orocos-rtt-${OROCOS_TARGET}_dynamic
        orocos-rtt-mqueue-${OROCOS_TARGET}_dynamic ${TEST_LIBRARIES})
      SET_TARGET_PROPERTIES( mqueue-test PROPERTIES
        COMPILE_DEFINITIONS "${COMPILE_DEFS}")
      ADD_TEST( mqueue-test ${RUNTIME_OUTPUT_DIRECTORY}/mqueue-test )
      list(APPEND ORO_EXTRA_TESTS "mqueue-test")

      ADD_UNIT_TEST(mqueue_archive_test ORO_EXTRA_TESTS "${TEST_LIBRARIES}")

    ENDIF(ENABLE_MQ)

    IF(ENABLE_MQ AND ENABLE_CORBA)
      ADD_EXECUTABLE( corba-mqueue-test test-runner-corba.cpp corba_mqueue_test.cpp )
      TARGET_LINK_LIBRARIES( corba-mqueue-test orocos-rtt-${OROCOS_TARGET}_dynamic
        orocos-rtt-mqueue-${OROCOS_TARGET}_dynamic orocos-rtt-corba-${OROCOS_TARGET}_dynamic
        ${CORBA_LIBRARIES} ${TEST_LIBRARIES})
      SET_TARGET_PROPERTIES( corba-mqueue-test PROPERTIES
        COMPILE_DEFINITIONS "${COMPILE_DEFS}")
      ADD_TEST( corba-mqueue-test ${RUNTIME_OUTPUT_DIRECTORY}/corba-mqueue-test )
      list(APPEND ORO_EXTRA_TESTS "corba-mqueue-test")
      
      # These tests require the Unix 'system()' function and 'killall'. To be ported to windows.
      IF (UNIX)
          ADD_EXECUTABLE( corba-mqueue-ipc-test test-runner-corba.cpp corba_mqueue_ipc_test.cpp )
          TARGET_LINK_LIBRARIES( corba-mqueue-ipc-test 
                orocos-rtt-${OROCOS_TARGET}_dynamic
                orocos-rtt-mqueue-${OROCOS_TARGET}_dynamic
                orocos-rtt-corba-${OROCOS_TARGET}_dynamic 
                ${CORBA_LIBRARIES} ${TEST_LIBRARIES})
          SET_TARGET_PROPERTIES( corba-mqueue-ipc-test PROPERTIES
            COMPILE_DEFINITIONS "${COMPILE_DEFS}"
            )
          ADD_TEST( corba-mqueue-ipc-test ${RUNTIME_OUTPUT_DIRECTORY}/corba-mqueue-ipc-test )
          list(APPEND ORO_EXTRA_TESTS "corba-mqueue-ipc-test")
    
          # This server is required to run when the corba-mqueue-ipc-test runs
          ADD_EXECUTABLE( corba-mqueue-ipc-server corba_mqueue_ipc_server.cpp )
          TARGET_LINK_LIBRARIES( corba-mqueue-ipc-server 
                orocos-rtt-${OROCOS_TARGET}_dynamic 
                orocos-rtt-mqueue-${OROCOS_TARGET}_dynamic
                orocos-rtt-corba-${OROCOS_TARGET}_dynamic 
                ${CORBA_LIBRARIES} ${TEST_LIBRARIES})
          SET_TARGET_PROPERTIES( corba-mqueue-ipc-server PROPERTIES
            COMPILE_DEFINITIONS "${COMPILE_DEFS}"
            )
     ENDIF()

      
    ENDIF(ENABLE_MQ AND ENABLE_CORBA)

    if( TESTS_OS_NO_ASM )
    else()
        ADD_UNIT_TEST(buffers_test ORO_EXTRA_TESTS "${TEST_LIBRARIES}")
    endif()
        
    ADD_UNIT_TEST(method_test ORO_EXTRA_TESTS "fixtures" )
    if (ORO_REMOTING)
        ADD_UNIT_TEST(remote_method_test ORO_EXTRA_TESTS "${TEST_LIBRARIES}" )
    endif()
    ADD_UNIT_TEST(functors_test ORO_EXTRA_TESTS "${TEST_LIBRARIES}" )
    ADD_UNIT_TEST(service_test ORO_EXTRA_TESTS "fixtures" )
    ADD_UNIT_TEST(service_port_test ORO_EXTRA_TESTS "fixtures" )
    ADD_UNIT_TEST(event_test ORO_EXTRA_TESTS "${TEST_LIBRARIES}" )
    ADD_UNIT_TEST(operation_test ORO_EXTRA_TESTS "${TEST_LIBRARIES}" )
    ADD_UNIT_TEST(taskstates_test ORO_EXTRA_TESTS "${TEST_LIBRARIES}" )
    ADD_UNIT_TEST(channelelements_test ORO_EXTRA_TESTS "${TEST_LIBRARIES}" )
    ADD_UNIT_TEST(ports_test ORO_EXTRA_TESTS "${TEST_LIBRARIES}" )
    ADD_UNIT_TEST(dataflow_performance_test ORO_EXTRA_TESTS "${TEST_LIBRARIES}" )
    ADD_UNIT_TEST(configuration_test ORO_EXTRA_TESTS "${TEST_LIBRARIES}" )
    ADD_UNIT_TEST(dev_test ORO_EXTRA_TESTS "${TEST_LIBRARIES}" )
    ADD_UNIT_TEST(slave_test ORO_EXTRA_TESTS "${TEST_LIBRARIES}" )
    if(PLUGINS_ENABLE_SCRIPTING)
        ADD_UNIT_TEST(scripting_test ORO_EXTRA_TESTS "${TEST_LIBRARIES};${SCRIPTING_LIBRARIES}" )
        ADD_UNIT_TEST(types_test ORO_EXTRA_TESTS "${TEST_LIBRARIES};${SCRIPTING_LIBRARIES}" )
        ADD_UNIT_TEST(program_test ORO_EXTRA_TESTS "${TEST_LIBRARIES};${SCRIPTING_LIBRARIES}" )
        ADD_UNIT_TEST(state_test ORO_EXTRA_TESTS "${TEST_LIBRARIES};${SCRIPTING_LIBRARIES}" )
        if(OS_RT_MALLOC)
          ADD_UNIT_TEST(rtstring_test ORO_EXTRA_TESTS "${TEST_LIBRARIES};${SCRIPTING_LIBRARIES}" )
          ADD_UNIT_TEST(tlsf_test ORO_EXTRA_TESTS "${TEST_LIBRARIES}")
          set_target_properties(tlsf_test PROPERTIES COMPILE_FLAGS "${TLSF_FLAGS}")
        endif(OS_RT_MALLOC)
        ADD_UNIT_TEST(function_test ORO_EXTRA_TESTS "${TEST_LIBRARIES};${SCRIPTING_LIBRARIES}" )
    endif()
    if(PLUGINS_ENABLE_MARSHALLING)
        ADD_UNIT_TEST(enum_type_test ORO_EXTRA_TESTS "${TEST_LIBRARIES};${MARSHALLING_LIBRARIES}")
        ADD_UNIT_TEST(enum_string_type_test ORO_EXTRA_TESTS "${TEST_LIBRARIES};${MARSHALLING_LIBRARIES}")
        ADD_UNIT_TEST(marshalling_test ORO_EXTRA_TESTS "${TEST_LIBRARIES};${MARSHALLING_LIBRARIES}" )
        ADD_UNIT_TEST(property_loader_test ORO_EXTRA_TESTS "${TEST_LIBRARIES};${MARSHALLING_LIBRARIES}" )
	    ADD_UNIT_TEST(property_marsh_test ORO_EXTRA_TESTS "${TEST_LIBRARIES};${MARSHALLING_LIBRARIES}" )
    endif()
    ADD_UNIT_TEST(property_test ORO_EXTRA_TESTS "${TEST_LIBRARIES}" )
    ADD_UNIT_TEST(property_composition_test ORO_EXTRA_TESTS "${TEST_LIBRARIES}" )
    ADD_UNIT_TEST(type_discovery_test ORO_EXTRA_TESTS "${TEST_LIBRARIES}" )
    ADD_UNIT_TEST(type_discovery_struct_test ORO_EXTRA_TESTS "fixtures;${TEST_LIBRARIES}" )
    ADD_UNIT_TEST(type_discovery_container_test ORO_EXTRA_TESTS "${TEST_LIBRARIES}" )
    ADD_UNIT_TEST(datasource_test ORO_EXTRA_TESTS "fixtures;${TEST_LIBRARIES}")
    ADD_UNIT_TEST(typekit_test ORO_EXTRA_TESTS "${TEST_LIBRARIES}" )
    if(PLUGINS_ENABLE)
        ADD_UNIT_TEST(plugins_test ORO_EXTRA_TESTS "${TEST_LIBRARIES}" )
        ADD_SUBDIRECTORY(testproject/plugins)
        ADD_SUBDIRECTORY(testproject/types)
        ADD_SUBDIRECTORY(testtypes/types)
    endif()

    IF(ENABLE_CORBA AND UNIX)
       #This test must be last: it stops the corba servers (on Unix systems)
       ADD_SIMPLE_TEST(cleanup_corba ORO_EXTRA_TESTS "")
    ENDIF(ENABLE_CORBA AND UNIX)
    
	# link to root build dir, to pick up CTest options when run from this dir
	EXECUTE_PROCESS(COMMAND ${CMAKE_COMMAND} -E create_symlink
	  ${CMAKE_BINARY_DIR}/DartConfiguration.tcl
	  ${CMAKE_CURRENT_BINARY_DIR}/DartConfiguration.tcl)

    ADD_CUSTOM_TARGET(check ${CMAKE_COMMAND} -E remove *.tst core *.ior orocos.log
      ${CORBA_SETUP_COMMAND}
      COMMAND ctest -V
      DEPENDS main-test core-test task-test ${ORO_EXTRA_TESTS}  
        WORKING_DIRECTORY "${RUNTIME_OUTPUT_DIRECTORY}")

    #ADD_DEPENDENCIES(check prep-cpf)
ENDIF()
