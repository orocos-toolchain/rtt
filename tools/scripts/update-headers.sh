#!/bin/sh

echo "Updating sources to GPL + exception license ..."

for m in $(for i in $(find src -name "*.c" -o -name "*.h" -o -name "*.hpp" -o -name "*.cpp"); do grep -L "Free Software" $i; done); do
   echo $m
   tools/scripts/header_gen.sh "FMTC" peter.soetens@fmtc.be $m
done
cd ..

echo "Updating tests to GPL license ..."
for m in $(for i in $(find tests -name "*.hpp" -o -name "*.cpp"); do grep -L "Free Software" $i; done); do
   echo $m
   tools/scripts/gpl_header_gen.sh "FMTC" peter.soetens@fmtc.be $m
done
cd ..

