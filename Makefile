ifdef ROS_ROOT
default: install
include $(shell rospack find mk)/cmake.mk
install: all
	cd build; CMAKE_INSTALL_PREFIX=$PWD make install
else
$(warning This Makefile only works with ROS rosmake, without rosmake do the normal cmake stuff)
endif
