#!/bin/sh

echo "Updating packages to LGPL license ..."
cd packages
for m in $(for i in $(find . -name "*.c" -o -name "*.h" -o -name "*.hpp" -o -name "*.cxx"); do grep -L "Free Software" $i; done); do
   echo $m
   ../lgpl_header_gen.sh "FMTC" peter.soetens@fmtc.be $m
done
cd ..

echo "Updating tests to GPL license ..."
cd tests
for m in $(for i in $(find . -name "*.hpp" -o -name "*.cpp"); do grep -L "Free Software" $i; done); do
   echo $m
   ../header_gen.sh "FMTC" peter.soetens@fmtc.be $m
done
cd ..
