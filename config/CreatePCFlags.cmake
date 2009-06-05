
# This macro converts CMake lists of defines, include dirs and link libraries to pkg-config
# style defines, include directives and link directives. The first three arguments are inputs
# The last three arguments are the outputs (any existing content in the output vars is deleted.)
# The first three arguments MUST be lists passed as a single argument. 
# The last three arguments will be strings.
#
# Usage: create_pc_flags( "${INCLUDES}" "${LIBS}" PC_INCLUDES PC_LIBS )
macro( create_pc_flags CM_DEFINES CM_INCLUDES CM_LIBRARIES PC_DEFINES PC_INCLUDES PC_LIBRARIES )
  set(${PC_DEFINES} "")
  set(${PC_INCLUDES} "")
  set(${PC_LIBRARIES} "")

  MESSAGE("Got cmlibs: ${CM_LIBRARIES}")
  MESSAGE("Got cmcflags: ${CM_INCLUDES}")
  MESSAGE("Got cmdefs: ${CM_DEFINES}")

  foreach( ITEM ${CM_LIBRARIES})
    if( ITEM )
      get_filename_component(PC_LIBPATH ${ITEM} PATH)
      get_filename_component(_PC_LIB ${ITEM} NAME_WE)
      string(REGEX REPLACE "^lib" "" PC_LIB ${_PC_LIB} )
      if (PC_LIBPATH)
	set( ${PC_LIBRARIES} "${${PC_LIBRARIES}} -L${PC_LIBPATH} -l${PC_LIB}")
      else()
	set( ${PC_LIBRARIES} "${${PC_LIBRARIES}} -l${PC_LIB}")
      endif()
    endif()
  endforeach()

  foreach( ITEM ${CM_INCLUDES})
    if( ITEM )
      set( ${PC_INCLUDES} "${${PC_INCLUDES}} -I${ITEM}")
    endif()
  endforeach()

  foreach( ITEM ${CM_DEFINES})
    if( ITEM )
      set( ${PC_DEFINES} "${${PC_DEFINES}} -D${ITEM}")
    endif()
  endforeach()

  MESSAGE("Computed pclibs: ${${PC_LIBRARIES}}")
  MESSAGE("Computed pccflags: ${${PC_INCLUDES}}")
  MESSAGE("Computed pcdefs: ${${PC_DEFINES}}")
endmacro()
