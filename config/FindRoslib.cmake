    # rospack support

    IF ( ROSLIB_SUPPORT )
      # Catkin style of fetching ROS deps
      FIND_PACKAGE( ROS QUIET COMPONENTS rospack)

      IF(NOT ROS_FOUND) # pre-Fuerte
        # TODO: This block to get roslib is deprecated as of ROS Fuerte, but is left in for pre-Fuerte compatibility
        set(roslib_PACKAGE_PATH ${ROS_ROOT}/core/roslib) ### XXX hardcoded
	message("Falling back to roslib in: ${roslib_PACKAGE_PATH}")
        find_library(ROS_LIBRARIES roslib ${roslib_PACKAGE_PATH}/lib )
        if ( NOT ROS_LIBRARIES )
	  find_package( ROS COMPONENTS rospack ) # Yells at user (non-QUIET !)
	else()
	  set(ROS_FOUND TRUE)
	  set(ROS_INCLUDE_DIRS ${roslib_PACKAGE_PATH}/include)
	endif()
      ENDIF(NOT ROS_FOUND)
      
      IF(ROS_FOUND)
        include_directories( ${ROS_INCLUDE_DIRS} )
	add_definitions( -DHAS_ROSLIB )
      ENDIF(ROS_FOUND)

    ELSE( ROSLIB_SUPPORT )
	message("No roslib package support.")

    ENDIF( ROSLIB_SUPPORT )
