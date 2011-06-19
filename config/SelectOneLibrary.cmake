# As some libraries are actually a list of libraries (e.g. "debug;xxx-d.lib;optimized;xxx.lib")
# and we can't deal with that in pkgconfig, we take
# 1) the library for this build type, or
# 2) the library if only one is listed, or
# 3) the release version if available and the build type is one of the release types, or
# 4) we error out.
#
# EXPECTED USAGE
#
#  SELECT_ONE_LIBRARY("Boost_THREAD_LIBRARY" BOOST_THREAD_LIB)
#  LIST(APPEND OROCOS-RTT_USER_LINK_LIBS ${BOOST_THREAD_LIB})
#
macro( SELECT_ONE_LIBRARY NAME RETURN)

  set(${RETURN} "")

  STRING(TOUPPER "${CMAKE_BUILD_TYPE}" CMAKE_BUILD_TYPE_UPPER)
  SET(NAME_U "${NAME}_${CMAKE_BUILD_TYPE_UPPER}")
  if (DEFINED ${NAME_U})

	set(${RETURN} ${${NAME_U}})

  else (DEFINED ${NAME_U})

	LIST(LENGTH ${NAME} COUNT)
	if (1 EQUAL COUNT)

	  set(${RETURN} ${${NAME}}})

	else (1 EQUAL COUNT)

	  # found nothing particular to this build type, but
	  # for all release-related types, use release (if available)

	  # these two if's have to be done separately for some reason ... :-(
	  if (CMAKE_BUILD_TYPE_UPPER MATCHES "RELWITHDEBINFO|RELEASE|MINSIZEREL")
		if (DEFINED ${NAME}_RELEASE)
		  MESSAGE(STATUS "Defaulting to release library for ${CMAKE_BUILD_TYPE_UPPER}")
		  set(${RETURN} ${${NAME}_RELEASE})
		endif ()
	  endif ()

	endif (1 EQUAL COUNT)

  endif (DEFINED ${NAME_U})

  if ("" STREQUAL ${RETURN})
	MESSAGE(FATAL_ERROR "Found multiple ${NAME} libraries, but not one specific to, or related to, the current build type '${CMAKE_BUILD_TYPE_UPPER}'.")
  endif()

endmacro()
