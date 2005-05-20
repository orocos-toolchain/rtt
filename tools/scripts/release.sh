#!/bin/sh

#
# cd export; ./release.sh 0.14.0

if [ x$1 == x ] ; then 
echo "Please provide version-string parameter"
exit 1
fi;

VERSION=$1 
unset ECOS_REPOSITORY

echo -e "This script builds a release on Peter's Orocos tree :\n
 0. DELETE old rm -rf orocos-$VERSION
 1. export a branch with the correct version
 2. make distcheck
 3. generate _only_ html docs in packages (set DODOCS to yes)
 4. create all epk packages and templates
 5. create the all-in-one downloadable package
 6. extract and build it
 7. upload files to server
"

# Set to no for testing, yes for release
# only  here for saving time.
# Set DODOCS to yes for release
# Set DOBUILD to yes for testing
# Set DOKERNEL to release the kernel-samples.
DODOCS=yes
DOBUILD=no
DOKERNEL=yes
DODISTCC=no
DOCHECKS=no

#compiler stuff :
if [ x$DODISTCC = xyes ]; then
CC="distcc"
CXX="distcc"
MAKEF="-j3"
else
CC="gcc"
CXX="gcc"
MAKEF=""
fi

echo "DODOCS is set to $DODOCS"
echo "DOBUILD is set to $DOBUILD"
echo "DOKERNEL is set to $DOKERNEL"
echo "DODISTCC is set to $DODISTCC"
echo "DOCHECKS is set to $DOCHECKS"

echo "Press d to copy files to server, any other key to build and Ctrl-C to abort..."
read -s -n1 x

if [ x$x != xd ] ;  then

#cleanup left-overs
mkdir -p orocos-$VERSION/build
chmod a+w orocos-$VERSION/build -R
rm -rf orocos-$VERSION
# MAKE SURE that you have the branch on disk
BRANCHVERSION=$(echo $VERSION | sed -e 's/\(.*\)\.\(.*\)\..*/\1.\2/g')
svn export ../orocos-branches/branch-$BRANCHVERSION ./orocos-$VERSION || exit 1
#svn export -rHEAD ../orocos-trunk ./orocos-$VERSION || exit 1
cd orocos-$VERSION || exit 1
./autogen.sh

#
# Build base package
#
mkdir -p build
cd build
../configure
if test x$DOCHECKS = xyes; then
make distcheck CC=$CC CXX=$CXX || exit 1
else
make dist || exit 1
fi
cd ..

#
# eCos Packages
#

# Build the package documentation (html files)
if test x$DODOCS != xno ; then
  echo "DODOCS was set to $DODOCS : building docs"
  cd packages
  make html-dist || exit 1
  cd ..
fi

# Build the ecos packages
cd packages
./ecospkggen.py -V $VERSION -t gnulinux
./ecospkggen.py -V $VERSION -t lxrt
./ecospkggen.py -V $VERSION -T corelib-os
./ecospkggen.py -V $VERSION -T kinematics-dynamics
./ecospkggen.py -V $VERSION -T control-framework
./ecospkggen.py -V $VERSION -T device-interface
./ecospkggen.py -V $VERSION -T device-drivers
./ecospkggen.py -V $VERSION -T execution
./ecospkggen.py -V $VERSION -T signal-processing
./ecospkggen.py -V $VERSION -T allpackages
./ecospkggen.py -V $VERSION -T minimal
./ecospkggen.py -V $VERSION -T dist-packages
./ecospkggen.py -V $VERSION -T control-services
./ecospkggen.py -V $VERSION geometry
#./ecospkggen.py -V $VERSION applications_testcase
mkdir -p epk
mv *.epk epk/
cd ..

# Build the i386linux package for orocos-installer.tcl
echo "Making the i386linux package..."
curdir=$(pwd)
rm -rf /tmp/oroinst
mkdir /tmp/oroinst

#copy specific packages 
#The user gets all the packages we have and some extra
# templates. dist-packages will be the default template
# for make all_packages
cp packages/epk/allpackages-$VERSION.epk /tmp/oroinst
cp packages/epk/minimal-$VERSION.epk /tmp/oroinst
cp packages/epk/dist-packages-$VERSION.epk /tmp/oroinst
cp packages/epk/control-services-$VERSION.epk /tmp/oroinst
cp packages/epk/gnulinux-$VERSION.epk /tmp/oroinst
cp packages/epk/lxrt-$VERSION.epk /tmp/oroinst
cp build/orocos-$VERSION.tar.gz /tmp/oroinst

cd /tmp/oroinst
tar -xzf orocos-$VERSION.tar.gz
cd orocos-$VERSION/packages/
touch ecos.db
for i in $(ls ../../*-$VERSION.epk); do ./ecosadmin.tcl add $i; done;
cd ../..
tar -cjf orocos-$VERSION.tar.bz2 orocos-$VERSION
mv orocos-$VERSION.tar.bz2 $curdir/build/
cd $curdir

#i386Linux package
cp tools/bin/* /tmp/oroinst
cd /tmp/oroinst/orocos-$VERSION
mkdir -p tools/bin
mv /tmp/oroinst/ecosconfig /tmp/oroinst/configtool tools/bin/
cd ..
tar -cjf orocos-$VERSION.i386linux.tar.bz2 orocos-$VERSION
mv orocos-$VERSION.i386linux.tar.bz2 $curdir/build/
cd $curdir

if test x$DOBUILD = xyes; then
# next unpack and make them :
rm -rf /tmp/orotst
mkdir /tmp/orotst
cd /tmp/orotst
tar -xjf $curdir/build/orocos-$VERSION.i386linux.tar.bz2
cd /tmp/orotst/orocos-$VERSION || exit 1
mkdir build ; cd build
../configure || exit 1

make new_packages || exit 1
if test x$DOCHECKS = xyes; then
make check-headers CC=$CC CXX=$CXX || exit 1
fi
make check CC=$CC CXX=$CXX $MAKEF || exit 1
cd $curdir
fi

echo "Seems all to be ok."
echo "Press a key to copy files to server, Ctrl-C to abort..."
read -s -n1

else
 cd orocos-$VERSION || exit 1
fi; # press d

# Base
ssh srv04 "mkdir -p pub_html/orocos/packages/$VERSION"

# EPKs
cd packages/epk
scp *-$VERSION.epk srv04:pub_html/orocos/packages/$VERSION || exit 1
cd ../..

# Release notes (optional)
cd build/doc
scp orocos-changes.html srv04:pub_html/orocos/packages/$VERSION/orocos-changes-$VERSION.html
cd ../..

# Orocos without tools
cd build
scp orocos-$VERSION.tar.bz2 srv04:pub_html/orocos/packages/$VERSION || exit 1
cd ..

# Orocos with i386linux tools
scp tools/installer/orocos-install.tcl tools/installer/orocos-install.db srv04:pub_html/orocos/packages
scp build/orocos-$VERSION.i386linux.tar.bz2 srv04:pub_html/orocos/packages/$VERSION || exit 1

if test x$DOKERNEL = xyes; then
#samples
cd ..
rm -rf ./kernel_sample-$VERSION
svn export ../orocos-apps/kernel_sample ./kernel_sample-$VERSION || exit 1
tar -czf kernel_sample-$VERSION.tar.gz kernel_sample-$VERSION

# if test x$DOBUILD = xyes; then
#   cd kernel_sample-$VERSION
#     for i in $(find . -name Makefile); do make all -C $(dirname $i) || exit 1; make clean -C $(dirname $i); done 
#   cd ..
# fi
ssh srv04 "mkdir -p pub_html/orocos/apps/kernel_sample/"
scp kernel_sample-$VERSION.tar.gz srv04:pub_html/orocos/apps/kernel_sample/ || exit 1
scp kernel_sample-$VERSION/README srv04:pub_html/orocos/apps/kernel_sample/README-$VERSION.txt || exit 1
scp kernel_sample-$VERSION/README srv04:pub_html/orocos/apps/kernel_sample/README.txt || exit 1
fi