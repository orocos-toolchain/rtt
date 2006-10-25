IF ( BUILD_TESTS)

    # This gathers all the .cpp files into the variable 'SRCS'
    FILE( GLOB SRCS [^.]*.cpp )

     ENABLE_TESTING()
     INCLUDE_DIRECTORIES( ${PROJ_SOURCE_DIR}/src  ${PROJ_BINARY_DIR}/src )
     LINK_DIRECTORIES(${LXRT_LINK_DIR})
     
     # repeat for each test
     ADD_EXECUTABLE( test_runner ${SRCS} )
     MESSAGE("LXRT_LINK_DIR ${LXRT_LINK_DIR}")
     TARGET_LINK_LIBRARIES( test_runner rtt cppunit ${XERCES_LIB} ${LXRT_LIB})
     ADD_TEST( test_runner test_runner )

ENDIF ( BUILD_TESTS)
