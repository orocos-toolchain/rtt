#!/bin/sh

echo "$0: creating aclocal.m4"
aclocal
echo "$0: creating configurescript"
autoconf
