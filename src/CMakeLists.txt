INCLUDE (${PROJ_SOURCE_DIR}/config/DependentOption.cmake)

###
### List user options first before traversing sub-directories:
### All user options must be defined in this file, sub-directories can
### only read the options, not change them (Variable scope limits in cmake).
###

### OS Abstraction
OPTION(OS_AGNOSTIC "Do not use asm/system.h and asm/atomic.h global system heades, but use RTT implementations." ON)
OPTION(OS_HAVE_IOSTREAM "Use C++ iostream library." ON)
OPTION(OS_HAVE_STREAMS "Use C++ streams library." ON)
OPTION(OS_HAVE_MAIN "Provide main() function in rtt library, which sets up the OS. The user implements ORO_main()." ON)
SET(OS_MAX_THREADS 8 CACHE STRING  "Maximum number of threads in the application.")
OPTION(OS_THREAD_SCOPE "Enable to monitor thread execution times through ThreadScope API." OFF)
OPTION(CONFIG_FORCE_UP "Enable to optimise for single core/cpu systems." OFF)

### Embedded
OPTION(OS_EMBEDDED "Build/Customise for embedded Operating System" OFF)
DEPENDENT_OPTION(OS_NOEXCEPTIONS "Disable exceptions." ON "ORO_EMBEDDED" OFF)
DEPENDENT_OPTION(OS_EMBEDDED_SCRIPTING "Enable Scripting for embedded Operating System. (requires EXCEPTIONS)" OFF "NOT OROBLD_OS_NOEXCEPTIONS" OFF)

IF ( OS_NOEXCEPTIONS )
  SET( RTT_CXXFLAGS "${RTT_CXXFLAGS} -fno-exceptions" )
ENDIF ( OS_NOEXCEPTIONS )

IF ( OS_EMBEDDED )
  SET( RTT_CXXFLAGS "${RTT_CXXFLAGS} -ffunction-sections -fdata-sections" )
ENDIF ( OS_EMBEDDED )

### Logger
OPTION(OROBLD_DISABLE_LOGGING "Disable Logging Infrastructure" OFF)
DEPENDENT_OPTION(OROSEM_PRINTF_LOGGING "Logger uses printf()/fprintf() instead of std::iostream." OFF "NOT OROBLD_DISABLE_LOGGING" OFF)
DEPENDENT_OPTION(OROSEM_FILE_LOGGING "Logger logs to orocos.log file." ON "NOT OROBLD_DISABLE_LOGGING" OFF)
DEPENDENT_OPTION(OROSEM_REMOTE_LOGGING "Logger allows remote log retrieval." ON "NOT OROBLD_DISABLE_LOGGING" OFF)

IF (OROSEM_REMOTE_LOGGING AND NOT OROBLD_DISABLE_LOGGING)
  SET(ORONUM_LOGGING_BUFSIZE 1000 CACHE STRING "Maximum number of lines kept for remote log retrieval.")
ELSE (OROSEM_REMOTE_LOGGING AND NOT OROBLD_DISABLE_LOGGING)
  SET(ORONUM_LOGGING_BUFSIZE CACHE INTERNAL "")
ENDIF (OROSEM_REMOTE_LOGGING AND NOT OROBLD_DISABLE_LOGGING)

### Execution Engine
OPTION(OROPKG_EXECUTION_ADVANCED "Advanced Execution Engine configuration" OFF)
DEPENDENT_OPTION(OROPKG_EXECUTION_ENGINE_EVENTS "Enable Event Processing." ON "OROPKG_EXECUTION_ADVANCED" ON)
DEPENDENT_OPTION(OROPKG_EXECUTION_ENGINE_COMMANDS "Enable Command Processing." ON "OROPKG_EXECUTION_ADVANCED" ON)
DEPENDENT_OPTION(OROPKG_EXECUTION_ENGINE_PROGRAMS "Enable Program Processing." ON "OROPKG_EXECUTION_ADVANCED" ON)
DEPENDENT_OPTION(OROPKG_EXECUTION_ENGINE_STATEMACHINES "Enable Statemachine Processing." ON "OROPKG_EXECUTION_ADVANCED" ON)
IF (OROPKG_EXECUTION_ADVANCED)
  SET(ORONUM_EXECUTION_PROC_QUEUE_SIZE 16 CACHE STRING "Default Command Queue size.")
ELSE (OROPKG_EXECUTION_ADVANCED)
  SET(ORONUM_EXECUTION_PROC_QUEUE_SIZE 16 CACHE STRING "Default Command Queue size.")
ENDIF (OROPKG_EXECUTION_ADVANCED)

### Marshalling
OPTION(ENABLE_MARSHALLING "Enable reading and writing Orocos Properties to XML." ON)

###
#   Collect Sources:
#
### Subdirs must append srcs inhere.
SET(ENV{GLOBAL_LIBRARY_SRCS})

FILE( GLOB CPPS [^.]*.cpp )
FILE( GLOB HPPS [^.]*.hpp [^.]*.h [^.]*.inl)

GLOBAL_ADD_INCLUDE( rtt ${HPPS})
GLOBAL_ADD_INCLUDE( rtt rtt-config.h)

GLOBAL_ADD_SRC( ${CPPS})
ADD_SUBDIRECTORY( corba )
ADD_SUBDIRECTORY( scripting )
ADD_SUBDIRECTORY( dev )
ADD_SUBDIRECTORY( os )
ADD_SUBDIRECTORY( marsh )
ADD_SUBDIRECTORY( impl )
ADD_SUBDIRECTORY( dlib )
ADD_SUBDIRECTORY( boost )

#
# Set accumulated compilation flags:
#
IF (${RTT_CXXFLAGS})
SET_SOURCE_FILES_PROPERTIES( $ENV{GLOBAL_LIBRARY_SRCS} PROPERTIES
  COMPILE_FLAGS ${RTT_CXXFLAGS}
  )
ENDIF (${RTT_CXXFLAGS})

# Reduce debugging info in scripting dir.
# IF (NOT OS_EMBEDDED OR OS_EMBEDDED_SCRIPTING)
# SET_SOURCE_FILES_PROPERTIES( $ENV{GLOBAL_SCRIPTING_SRCS} PROPERTIES COMPILE_FLAGS "-g1" )
# ENDIF (NOT OS_EMBEDDED OR OS_EMBEDDED_SCRIPTING)


####
#   Build Libraries:
#
# Settings for building a static library (.a)
#
OPTION(BUILD_STATIC "Build Orocos RTT as a static library." OFF)
IF ( BUILD_STATIC )
  ADD_LIBRARY(orocos-rtt-static STATIC $ENV{GLOBAL_LIBRARY_SRCS})
  SET_TARGET_PROPERTIES(orocos-rtt-static
    PROPERTIES 
    OUTPUT_NAME orocos-rtt
    CLEAN_DIRECT_OUTPUT 1)
  INSTALL_TARGETS(/lib orocos-rtt-static)

  #
  # If corba is enabled, the orocos-rtt-corba helper library is built.
  #
  IF ( ENABLE_CORBA )
    ADD_DEPENDENCIES(orocos-rtt-static orocos-rtt-corba-static)
    INCLUDE_DIRECTORIES( ${PROJ_BINARY_DIR}/src/corba ) # generated files.
  ENDIF ( ENABLE_CORBA )
  
ENDIF ( BUILD_STATIC )

#
# Settings for building a dynamic library (.so/.dll)
#
OPTION(BUILD_DYNAMIC "Build Orocos RTT as a dynamic library (Default)." ON)
IF( BUILD_DYNAMIC)
  ADD_LIBRARY(orocos-rtt-dynamic SHARED $ENV{GLOBAL_LIBRARY_SRCS})
  SET_TARGET_PROPERTIES(orocos-rtt-dynamic
    PROPERTIES OUTPUT_NAME orocos-rtt
    CLEAN_DIRECT_OUTPUT 1
    SOVERSION ${VERSION} )
  INSTALL_TARGETS(/lib orocos-rtt-dynamic)

  #
  # If corba is enabled, the orocos-rtt-corba helper library is built.
  #
  IF ( ENABLE_CORBA )
    ADD_DEPENDENCIES(orocos-rtt-dynamic orocos-rtt-corba-dynamic)
    TARGET_LINK_LIBRARIES(orocos-rtt-dynamic orocos-rtt-corba)
    TARGET_LINK_LIBRARIES(orocos-rtt-dynamic orocos-rtt-corba-dynamic)
    #SET_TARGET_PROPERTIES(orocos-rtt-dynamic PROPERTIES LINK_FLAGS "-L${PROJ_BINARY_DIR}/src/corba" )
    INCLUDE_DIRECTORIES( ${PROJ_BINARY_DIR}/src/corba ) # generated files.
  ENDIF ( ENABLE_CORBA )
  
ENDIF ( BUILD_DYNAMIC )


### Generate rtt-config.h : HAS NO ACCESS TO SUBDIRECTORY VARIABLES !!!
CONFIGURE_FILE( ${PROJ_SOURCE_DIR}/src/rtt-config.h.in ${PROJ_BINARY_DIR}/src/rtt-config.h @ONLY)
