#!/bin/bash

echo "This script builds docs on Peter's Orocos tree
1a. expects a directory build/orocos-rtt-VERSION and build/orocos-rtt-VERSION/build
1b. OR if version is 'latest' , do fresh check out from trunk 
3. make docs in build
4. tar the result
5. copy over all the doc-tar files
6. extract and move files on server
"
set -ex

if [ x$1 == x ] ; then 
echo "Please provide version-string parameter"
exit 1
fi;
if [ x$2 != xdev ] ; then 
DEV=no
else
DEV=yes
fi;

DOLOCAL=yes

. release-config

if test $DOLOCAL = yes; then
  USER=$USER
  SERVER=localhost
  SPREFIX=src/export/upload
else
  if test x$DOOROCOSORG = xyes -a x$DEV = xno; then
    USER=bruyninckxh2
    SERVER=www.orocos.org
    SPREFIX=www.orocos.org
  else
    USER=orocos
    SERVER=ftp.mech.kuleuven.be
    SPREFIX=pub_html/pub
  fi
fi

if test x$DEV = xyes; then
BRANCH=devel
else
BRANCH=stable
fi

# define "1.0.0" and "v1.0.0"
VERSION=$1
if test $VERSION = master; then
VVERSION=master
else
VVERSION=v$1
fi

# i.e. 1.2.1 -> 1.2
BRANCHVERSION=$(echo $VERSION | sed -e 's/\(.*\)\.\(.*\)\..*/\1.\2/g')

topdir=$(pwd)

if test x$DOAUTO != xyes; then
    echo "VERSION is set to $VERSION (use 'master' to install trunk on server)"
    echo "DEV is set to $DEV (use 'dev' as arg2 to install in 'devel' on server)"
    echo "Press c to continue, any other key to upload files to server and Ctrl-C to abort..."
    read -s -n1 x
else
    x=c
fi

if [ x$x == xc ] ;  then

#if master, check out trunk
mkdir -p build; cd build
if test x$VERSION = xmaster -o x$DOCHECKOUT = xyes; then
  rm -rf orocos-toolchain-$VERSION/rtt
  cd $topdir/orocos-toolchain-rtt
  git archive --format=tar --prefix=orocos-toolchain-$VERSION/rtt/ HEAD | (cd $topdir/build && tar xf -)
  cd $topdir/build
  cd orocos-toolchain-$VERSION/rtt
  mkdir build
  cd build
  cmake ..
fi
cd $topdir/build

#all should be equal for MASTER and normal :
if  ! test -d orocos-toolchain-$VERSION/rtt ; then
    echo "Could not find orocos-toolchain-$VERSION/rtt !"
    exit 1
fi
cd orocos-toolchain-$VERSION/rtt

# Doxygen
mkdir -p build
cd build
  cmake ..
  make docapi
  cd doc
  tar -cjf orocos-rtt-$VERSION-api.tar.bz2 api
  mv orocos-rtt-$VERSION-api.tar.bz2 api ..
  #make install-docs # install rtt.tag file !
  cd ..
cd ..

# Build base package
cd build
  cd doc
  make docpdf dochtml -j12
  
  cp -a xml doc-xml
  rm -rf doc-xml/images/hires # not for distribution
  tar -cjf orocos-rtt-$VERSION-doc.tar.bz2 $(find doc-xml -name "*.png" -o -name "*.pdf" -o -name "*.html" -o -name "*css")  ||exit 1
  rm -rf doc-xml
  mv orocos-rtt-$VERSION-doc.tar.bz2 ..
  cd ..
cd ..

if test x$DOAUTO != xyes; then
    echo "Press a key to copy Docs to server, Ctrl-C to abort..."
    read -s -n1
fi

else
 cd $topdir/build/orocos-toolchain-$VERSION/rtt
fi; # press d

while [ 1 ]; do

echo -e "\n**** COPYING TO $SERVER: ****\n"

# Docs :
# Save in version subdir as tar, save master in doc dir. (saves space).
cd build
# Copy over tar.bz2 files
ssh $USER@$SERVER "mkdir -p $SPREFIX/$BRANCH/rtt/$VVERSION"
scp orocos-rtt-$VERSION-doc.tar.bz2 $USER@$SERVER:$SPREFIX/$BRANCH/rtt/$VVERSION
scp orocos-rtt-$VERSION-api.tar.bz2 $USER@$SERVER:$SPREFIX/$BRANCH/rtt/$VVERSION
# Install them in the 'documentation' dir:
# 'doc' is not physically existing, it is the drupal path to imported docs
# 'doc-xml' are the xml generated html/pdf files
# 'api' is the doxygen generated sources
if test x$DEV = xno; then
ssh $USER@$SERVER "mkdir -p $SPREFIX/$BRANCH/documentation/rtt/v$BRANCHVERSION.x/"
ssh $USER@$SERVER "cd $SPREFIX/$BRANCH/documentation/rtt/v$BRANCHVERSION.x/ &&
rm -rf doc api doc-xml &&
tar -xjf ../../../rtt/$VVERSION/orocos-rtt-$VERSION-doc.tar.bz2 && 
tar -xjf ../../../rtt/$VVERSION/orocos-rtt-$VERSION-api.tar.bz2 &&
rm -f ../../../rtt/$VVERSION/orocos-rtt-$VERSION-api.tar.bz2 ../../../rtt/$VVERSION/orocos-rtt-$VERSION-doc.tar.bz2 &&
cd .. &&
rm -f v2.x &&
ln -s v$BRANCHVERSION.x v2.x
"
else
ssh $USER@$SERVER "mkdir -p $SPREFIX/$BRANCH/documentation/rtt/$VVERSION"
ssh $USER@$SERVER "cd $SPREFIX/$BRANCH/documentation/rtt/$VVERSION &&
rm -rf doc api doc-xml &&
tar -xjf ../../../rtt/$VVERSION/orocos-rtt-$VERSION-doc.tar.bz2 && 
tar -xjf ../../../rtt/$VVERSION/orocos-rtt-$VERSION-api.tar.bz2 &&
rm -f ../../../rtt/$VVERSION/orocos-rtt-$VERSION-api.tar.bz2 ../../../rtt/$VVERSION/orocos-rtt-$VERSION-doc.tar.bz2 &&
"
fi
cd ..

# copy latest news to packages directory :
scp NEWS $USER@$SERVER:$SPREFIX/$BRANCH/rtt/NEWS.txt
scp README $USER@$SERVER:$SPREFIX/$BRANCH/rtt/README.txt

  if test x$DOOROCOSORG = xno -o x$DOLOCAL = xyes -o x$DEV = xyes; then
      echo "Completed succesfully."
      exit 0;
  fi
  # redo for making a copy on the mech server as well:
  USER=orocos
  SERVER=ftp.mech.kuleuven.be
  SPREFIX=pub_html/pub
  DOOROCOSORG=no

done; # while [ 1 ]

