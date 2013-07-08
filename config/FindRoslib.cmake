    # rospack support

    IF ( ROSLIB_SUPPORT )
      # Catkin style of fetching ROS deps
      FIND_PACKAGE( ROS QUIET COMPONENTS rospack)

      IF(ROS_FOUND)
        include_directories( ${ROS_INCLUDE_DIRS} )
	add_definitions( -DHAS_ROSLIB )
      ENDIF(ROS_FOUND)

    ELSE( ROSLIB_SUPPORT )
	message("No roslib package support.")

    ENDIF( ROSLIB_SUPPORT )
