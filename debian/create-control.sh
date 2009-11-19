#!/bin/bash

echo "This scripts creates the debian/control and debian/*.install files from several template files."

major=$(head -1 changelog | sed "s/.*(\([0-9]\+\.[0-9]\+\).*/\1/g")

echo "Detected RTT Major version: $major"

if test x$1 = x; then
    targets="gnulinux"
else
    targets="$@"
fi

echo "Building scripts for targets: $targets"

# Prepare control file:
rm -f control control.targets

echo "Creating control ..."
for t in $targets; do 
    # append control-template.in to control file
    if test $t = xenomai; then tdev=", xenomai-dev | libxenomai-dev"; bdeps="$tdev"; fi
    if test $t = lxrt; then tdev=", librtai-dev"; bdeps="$tdev"; fi
    cat control-template.in | sed -e"s/@TARGET@/$t/g;s/@TARGET-DEV@/$tdev/g;s/@LIBVER@/$major/g" >> control.targets
    cat control-$t.in >> control.targets
done
# generate final control file = common + targets
cat control.common | sed -e"s/@LIBVER@/$major/g;s/@BUILD_DEPS@/$bdeps/g" > control
cat control.targets >> control

# Prepare *.install files:
for i in $(ls *template*install); do
    for t in $targets; do
	# Replace contents and write to *-target.install files:
	fname=$(echo "$i" | sed -e"s/template/$t$major/g")
	echo "Creating $fname ..."
	cat $i | sed -e"s/@TARGET@/$t/g" > $fname
    done
done

# Prepare *common-dev.install files:
for i in $(ls *common-dev*install); do
    # Replace contents and write to *-target.install files:
    fname=$(echo "$i" | sed -e"s/common/common$major/g")
    echo "Creating $fname..."
    cat $i > $fname
done

echo "Creating rules files..."
# Prepare rules files:
rm -f rules.targets
for t in $targets; do
    CONFIGURE_TARGETS="$CONFIGURE_TARGETS configure-$t"
    BUILD_TARGETS="$BUILD_TARGETS build-$t"
    INSTALL_TARGETS="$INSTALL_TARGETS install-$t"
    INCLUDES=$(echo -e "$INCLUDES\ninclude debian/rules.$t")
done
cat >> rules.targets <<EOF
CONFIGURE_TARGETS=$CONFIGURE_TARGETS
BUILD_TARGETS=$BUILD_TARGETS
INSTALL_TARGETS=$INSTALL_TARGETS

$INCLUDES
EOF

echo "Done!"