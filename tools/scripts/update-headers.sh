#!/bin/sh

if [ "$1" = "" -o "$2" = "" ]; then
    echo Generate Free Software Header
    echo Usage: $0 \"Copyright holder\" EMail-Address
    exit 1
fi

echo "Updating sources to GPL + exception license ..."

for m in $(for i in $(find rtt -name "*.c" -o -name "*.h" -o -name "*.hpp" -o -name "*.cpp"); do grep -L "Free Software" $i; done); do
   echo $m
   tools/scripts/header_gen.sh "$1" "$2" $m
done

echo "Updating tests to GPL license ..."
for m in $(for i in $(find tests -name "*.hpp" -o -name "*.cpp"); do grep -L "Free Software" $i; done); do
   echo $m
   tools/scripts/gpl_header_gen.sh "$1" "$2" $m
done

