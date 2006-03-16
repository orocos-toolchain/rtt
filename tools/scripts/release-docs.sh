#!/bin/sh

echo "This script builds docs on Peter's Orocos tree
1a. expects a directory orocos-VERSION and orocos-VERSION/build
1b. OR if version is 'latest' , do fresh check out from trunk 
2. make orocos-dist in packages
3. make doc in build/doc
4. tar the result
5. copy over all the doc-tar files
6. extract and move files on server
"


if [ x$1 == x ] ; then 
echo "Please provide version-string parameter"
exit 1
fi;
if [ x$2 != xdev ] ; then 
DEV=no
else
DEV=yes
fi;

VERSION=$1

unset ECOS_REPOSITORY

echo "VERSION is set to $VERSION (use 'latest' to install trunk on server)"
echo "DEV is set to $DEV (use 'dev' as arg2 to install in 'doc/latest' on server)"
echo "Press d to copy Documentation to server, any other key to build and Ctrl-C to abort..."
read -s -n1 x

if [ x$x != xd ] ;  then

#if latest, check out trunk
if test x$VERSION = xlatest; then
  rm -rf orocos-latest
  svn export ../orocos-trunk orocos-$VERSION
  cd orocos-$VERSION
  ./autogen.sh
  mkdir build
  cd build
  ## add the -r1234 tag to version number
  ../configure --enable-subversion
  cd ../..
fi

#all should be equal for LATEST and normal :
if  ! test -d orocos-$VERSION ; then
    echo "Could not find orocos-$VERSION !"
    exit 1
fi
cd orocos-$VERSION


  # Build Online documentation and prepare eps files for later
  cd packages
  make doc-dist || exit 1
  cd ../build
  make doxygen_headers
  make doxy-dist || exit 1
  cd ..

# Build base package
if ! test -d build ; then
  echo "No build directory !"
  exit 1
fi 
cd build
  #must do clean to force rebuild of orocos-manual
  make clean
  make docs
  cd doc
  make manuals
  tar -czf orocos-manual.tgz $(find . -name "*.png") *.pdf *.html  ||exit 1
  cd ..
cd ..

#cleanup leftovers
cd packages
  make doc-dist-clean
cd ..

echo "Press a key to copy Docs to server, Ctrl-C to abort..."
read -s -n1

else
 cd orocos-$VERSION || exit 1
fi; # press d

if test x$SSH_USER = x; then
 SSH_USER=psoetens
fi;

# Docs :
# Save in version subdir as tar, save latest in doc dir. (saves space).
cd packages
if test x$DEV != xyes ; then 
ssh $SSH_USER@srv04 "mkdir -p pub_html/orocos/packages/$VERSION/doc"
ssh $SSH_USER@srv04 "mkdir -p pub_html/orocos/doc/"
scp orocos-docs.tar $SSH_USER@srv04:pub_html/orocos/packages/$VERSION/doc || exit 1
scp ../build/doc/orocos-manual.tgz $SSH_USER@srv04:pub_html/orocos/packages/$VERSION/doc/orocos-manual.tgz || exit 1
scp ../build/orocos-api.tar.bz2 $SSH_USER@srv04:pub_html/orocos/ || exit 1
ssh $SSH_USER@srv04 "cd pub_html/orocos/doc && 
tar -xf ../packages/$VERSION/doc/orocos-docs.tar && 
cp ../packages/$VERSION/doc/*.tgz .
for i in \$(ls *.tgz); do tar -xzf \$i; rm -f \$i; done;
mv orocos-changes.html ../packages/$VERSION/orocos-changes-$VERSION.html
cd ..
rm -rf doc/api
tar -xjf orocos-api.tar.bz2
mv orocos-api.tar.bz2 doc/
 " || exit 1
#copy the images
scp -r ../doc/images $SSH_USER@srv04:pub_html/orocos/doc/
#copy CSS
scp -r ../doc/*.css $SSH_USER@srv04:pub_html/orocos/doc/
cd ..
scp NEWS $SSH_USER@srv04:pub_html/orocos/packages/$VERSION/NEWS.txt
else # dev :
ssh $SSH_USER@srv04 "mkdir -p pub_html/orocos/doc/latest"
scp orocos-docs.tar $SSH_USER@srv04:pub_html/orocos/doc/latest || exit 1
scp ../build/doc/orocos-manual.tgz $SSH_USER@srv04:pub_html/orocos/doc/latest/orocos-manual.tgz || exit 1
scp ../build/orocos-api.tar.bz2 $SSH_USER@srv04:pub_html/orocos/doc/latest || exit 1
ssh $SSH_USER@srv04 "cd pub_html/orocos/doc/latest
tar -xf orocos-docs.tar && for i in \$(ls *.tgz); do tar -xzf \$i; rm -f \$i; done;
rm -rf api && tar -xjf orocos-api.tar.bz2 && mv doc/api api && rmdir doc
 " || exit 1
#copy the images
scp -r ../doc/images $SSH_USER@srv04:pub_html/orocos/doc/latest
#copy CSS
scp -r ../doc/*.css $SSH_USER@srv04:pub_html/orocos/doc/
cd ..
fi

# copy latest news to packages directory :
scp NEWS $SSH_USER@srv04:pub_html/orocos/packages/NEWS.txt


