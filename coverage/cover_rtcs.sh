#!/bin/sh

if [ ! -d build-gcov ]; then
    mkdir build-gcov
fi
    cd build-gcov
if [ ! -f Makefile ]; then
    ../../configure --with-gnulinux --enable-gcov --enable-subversion
fi

#all set, build with gcov flags.
make control_services CFLAGS="-ftest-coverage -fprofile-arcs -D_REENTRANT"

# todo : use XmlOutputter of CppUnit to generate XML results.
make check CXXFLAGS="-ftest-coverage -fprofile-arcs"



