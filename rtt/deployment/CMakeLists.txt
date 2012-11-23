if ( PLUGINS_ENABLE )

    # See also plugin/CMakeLists.txt
    if (NOT DEFAULT_PLUGIN_PATH)
      set(DEFAULT_COMPONENT_PATH "${CMAKE_INSTALL_PREFIX}/lib/orocos")
    else()
      set(DEFAULT_COMPONENT_PATH "${DEFAULT_PLUGIN_PATH}")      
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


