#!/bin/sh

echo -e \
"This script is meant to be executed by users downloading 
Orocos packages from the net. It does not hurt running this script 
multiple times and it is intended to be run if you installed 
an extra support (comedi, boost, readline,...) library to
be used with Orocos.

Press any key to continue...
"
read -s -n1

cd packages
## Repeat these two steps if you have installed
## new libraries which Orocos can use...
# Detect installed software :
./configure
# Safe the results in the ecos.db file :
make db

cd ..

echo -e \
" Next do something like : 
     mkdir build 
     cd build 
     ../configure --with-lxrt 
     make new_packages  
     make all_packages 
     .... (see manual)
"

echo "Have fun !"
