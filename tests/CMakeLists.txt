IF ( BUILD_TESTS)

    # This gathers all the .cpp files into the variable 'SRCS'
    FILE( GLOB SRCS [^.]*.cpp )

    # Add a test program
    GLOBAL_ADD_TESTS( buffers_test buffers_test.cpp )
    # Use  TARGET_LINK_LIBRARIES( report libs... ) to add library deps.

    TARGET_LINK_LIBRARIES( buffers_test rtt )

ENDIF ( BUILD_TESTS)