IF ( BUILD_TESTS)

    # This gathers all the .cpp files into the variable 'SRCS'
    FILE( GLOB SRCS [^.]*.cpp )

     ENABLE_TESTING()
     INCLUDE_DIRECTORIES( ${PROJ_SOURCE_DIR} )

     # repeat for each test
     ADD_EXECUTABLE( test_runner ${SRCS} )
     TARGET_LINK_LIBRARIES( test_runner rtt cppunit )
     ADD_TEST( test_runner test_runner )

ENDIF ( BUILD_TESTS)
