
if ( PLUGINS_ENABLE )

    # Do this first.
    if (NOT DEFAULT_PLUGIN_PATH)
        message("No default plugin path given, setting it to ${CMAKE_INSTALL_PREFIX}/lib/orocos.")
        message("Define the DEFAULT_PLUGIN_PATH cmake variable to override this.")
        set(DEFAULT_PLUGIN_PATH "${CMAKE_INSTALL_PREFIX}/lib/orocos")
    endif()

    configure_file( pluginpath.cpp.in ${CMAKE_CURRENT_BINARY_DIR}/pluginpath.cpp @ONLY )

    if (NOT (${CMAKE_CURRENT_BINARY_DIR} STREQUAL ${CMAKE_CURRENT_SOURCE_DIR}))
        # This to reduce upgrade pains for in-tree upgraders:
        execute_process( COMMAND ${CMAKE_COMMAND} -E remove -f ${CMAKE_CURRENT_SOURCE_DIR}/pluginpath.cpp )
    endif()

    FILE( GLOB CPPS [^.]*.cpp )
    FILE( GLOB HPPS [^.]*.hpp [^.]*.h [^.]*.inl)
    GLOBAL_ADD_INCLUDE( rtt/plugin ${HPPS})
    GLOBAL_ADD_SRC( ${CPPS} ${CMAKE_CURRENT_BINARY_DIR}/pluginpath.cpp )
    
endif( PLUGINS_ENABLE )
