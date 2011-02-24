ifdef ROS_ROOT
default: install
include $(shell rospack find mk)/cmake.mk
install: all
	cd build; ${MAKE} install
else
$(warning This Makefile only works with ROS rosmake. Without rosmake, create a build directory and run cmake ..)
endif
