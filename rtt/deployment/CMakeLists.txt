if ( PLUGINS_ENABLE )

    # Do this first.
    if (NOT DEFAULT_PLUGIN_PATH)
    IF ( ROSLIB_SUPPORT )
	# Necessary when building Debian packages:
	set(ROS_STACK_DIR_FINAL $ENV{ROS_STACK_DIR_FINAL})
	if (ROS_STACK_DIR_FINAL)
	  set(DEFAULT_COMPONENT_PATH "${ROS_STACK_DIR_FINAL}/orocos_toolchain/install/lib/orocos")
	else(ROS_STACK_DIR_FINAL)
	  set(DEFAULT_COMPONENT_PATH "${PROJECT_SOURCE_DIR}/../install/lib/orocos" CACHE STRING "Default component lookup path when none given. (semi-)colon separated list." FORCE)
	endif(ROS_STACK_DIR_FINAL)
    ENDIF( ROSLIB_SUPPORT )

        message("No default component path given, setting it to ${CMAKE_INSTALL_PREFIX}/lib/orocos.")
        message("Define the DEFAULT_COMPONENT_PATH cmake variable to override this.")
        set(DEFAULT_COMPONENT_PATH "${CMAKE_INSTALL_PREFIX}/lib/orocos")
    endif()

    configure_file( comppath.cpp.in ${CMAKE_CURRENT_BINARY_DIR}/comppath.cpp @ONLY )

    if (NOT ${CMAKE_CURRENT_BINARY_DIR} STREQUAL ${CMAKE_CURRENT_SOURCE_DIR} )
        # This to reduce upgrade pains for in-tree upgraders:
        execute_process( COMMAND ${CMAKE_COMMAND} -E remove -f ${CMAKE_CURRENT_SOURCE_DIR}/comppath.cpp )
    endif()
    
    FILE( GLOB CPPS [^.]*.cpp )
    FILE( GLOB HPPS [^.]*.hpp [^.]*.h [^.]*.inl)
    GLOBAL_ADD_INCLUDE( rtt/deployment ${HPPS})
    GLOBAL_ADD_SRC( ${CPPS} ${CMAKE_CURRENT_BINARY_DIR}/comppath.cpp )

endif( PLUGINS_ENABLE )


