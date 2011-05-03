ifdef ROS_ROOT
EXTRA_CMAKE_FLAGS=-DENABLE_CORBA=ON -DCORBA_IMPLEMENTATION=OMNIORB
default: install
include $(shell rospack find mk)/cmake.mk
install: all
	cd build; ${MAKE} install
else
$(warning This Makefile only works with ROS rosmake. Without rosmake, create a build directory and run cmake ..)
endif
