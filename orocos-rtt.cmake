################################################################################

# if build type not set then use env. var. if set, otherwise use default
IF (NOT CMAKE_BUILD_TYPE)
  SET(CM_BUILD_TYPE "$ENV{CMAKE_BUILD_TYPE}")
  IF (NOT CM_BUILD_TYPE)
	SET(CM_BUILD_TYPE "RelWithDebInfo")
	MESSAGE("Using default build type: ${CM_BUILD_TYPE}")
  ENDIF()
  SET(CMAKE_BUILD_TYPE "${CM_BUILD_TYPE}" CACHE STRING "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel" FORCE)
ENDIF()

# for omniORB
OPTION(ENABLE_CORBA "" ON)
SET(CORBA_IMPLEMENTATION "OMNIORB" CACHE STRING "" FORCE)

################################################################################
# project-specific

OPTION(OS_RT_MALLOC "" ON)
OPTION(OS_RT_MALLOC_SBRK "" OFF)
OPTION(OS_RT_MALLOC_MMAP "" OFF)
OPTION(OS_RT_MALLOC_STATS "" ON)
OPTION(OS_RT_MALLOC_DEBUG "" ON)

OPTION(OROSEM_LOG4CPP_LOGGING "" ON)

OPTION(ENABLE_TESTS "" OFF)
OPTION(ENABLE_TEST_INSTALL "Enable installing tests." OFF)
