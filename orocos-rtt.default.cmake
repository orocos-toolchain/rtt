# Here we have a bunch of user defined commands that are being loaded at the
# very start of the makefile generation process. You can use this setup file to
# for example change compiler flags, orocos-target and other user specific options

#
# Add additional include paths for: 
# boost, xerces, Cppunit, etc.
#
# set(CMAKE_INCLUDE_PATH ${CMAKE_INCLUDE_PATH} "C:/local/Boost-1.34.1/include")

#
# An option for tests, to make it easy to turn off all tests. It requires Cppunit
#
option( ENABLE_TESTS "Turn me off to disable compilation of all tests" OFF )

#
# OFF: SHARED  (ON: SHARED AND STATIC) 
#
# set(FORCE_BUILD_STATIC OFF)

#
# Set the target operating system. One of [lxrt gnulinux xenomai macosx win32]
#
set( OROCOS_TARGET gnulinux CACHE STRING "The Operating System target. One of [lxrt gnulinux xenomai macosx win32]")

#
# Output goes to bin/ and libs/ unless specified otherwise by the user.
#
# set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/bin CACHE PATH "Bin directory")
# set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/libs CACHE PATH "Lib directory")

#
# And additional link directories.
#
# LINK_DIRECTORIES( ${CMAKE_CURRENT_SOURCE_DIR}/libs )






