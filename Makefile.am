SUBDIRS = . tests doc

# These are not distributed by default (adds to many megs)
# The i386linux package will copy them.
#	tools/bin/ecosconfig tools/bin/configtool 

EXTRA_DIST = \
	packages/ecosadmin.tcl packages/ecosdbgen.py packages/ecospkggen.py \
	packages/configure.ac packages/configure bootstrap.sh \
	packages/aclocal.m4 packages/acinclude.m4 \
	packages/Makefile.in packages/Doxyfile.in \
	doc/CODING_STYLE.txt \
	packages/support/common/current/support.cdl.in \
	packages/support/boost/current/boost.cdl.in \
	packages/support/comedi/current/comedi.cdl.in \
	packages/support/comedilib/current/comedilib.cdl.in \
	packages/support/xercesc/current/xercesc.cdl.in \
	packages/support/readline/current/readline.cdl.in \
	packages/support/rtai/current/rtai.cdl.in \
	packages/pkgconf/fixhtml.tcl \
        packages/pkgconf/rules.doc \
        packages/pkgconf/rules.mak \
        packages/pkgconf/ssa4.dsl \
        packages/pkgconf/ssletter.dsl \
        packages/pkgconf/stylesheet.dsl

# EXTRA_DIST = \
# 	packages

DISTCHECK_CONFIGURE_FLAGS = --disable-tests

docs:
	$(MAKE) -C doc docs

packages/ecos.db:
	touch @abs_srcdir@/packages/ecos.db

clean-local: clean_packages

uninstall-local: clean_install

all-local: test_new_packages test_all_packages

distclean-local:
	@rm -f .new-pkg-stamp packages/.all-stamp packages/.configure-stamp

test_new_packages: .new-pkg-stamp

.new-pkg-stamp:
	$(MAKE) -C . new_packages

# This sets up a new packages build directory.
# You only need to do this once.
new_packages:
	@rm -f .new-pkg-stamp
	mkdir -p packages
	@echo "Setting up packages..."
	@cd packages &&\
	export ECOS_REPOSITORY=@ECOS_REP@;\
	ecosconfig new @ECOS_TARGET@ minimal ;\
	if test -f ecos.ecc; then \
	ecosconfig add support ; \
	for i in $$(find @ECOS_REP@/support -name "*.cdl" -a -not -name "support.cdl"); do \
	ecosconfig add support_$$(basename $$i .cdl) >/dev/null ; done ;\
	{ ecosconfig template dist-packages || ecosconfig template corelib-os; };\
	echo -e "\n This is now your new configuration :\n" ;\
	ecosconfig check ;\
	rm -f .configure-stamp .all-stamp ; else touch .configure-stamp .all-stamp ;\
	fi
	@touch .new-pkg-stamp

# If this file does not exist, packages are present
.configure-stamp:
	$(MAKE) -C . configure_packages

# This configures the selected packages with
# a graphical tool.
configure_packages: test_new_packages
	cd packages &&\
	export ECOS_REPOSITORY=@ECOS_REP@ &&\
	ln -sf ecos.ecc orocos.ecc &&\
	configtool orocos.ecc

test_all_packages: .all-stamp

#Only executed if new_packages did not create this stamp.
.all-stamp:
	$(MAKE) -C . all_packages

# This makes the ecos makefiles in the packages
# build directory and makes the library
all_packages: test_new_packages 
	cd packages &&\
	if test -f ecos.ecc; then \
	export ECOS_REPOSITORY=@ECOS_REP@ &&\
	ecosconfig tree && \
	$(MAKE) -C . ;\
	fi

precompile_headers:
	$(MAKE) headers -C packages
	cd packages/install/include &&\
	rm -f all.h &&\
	echo "#ifdef __cplusplus" >> all.h &&\
	for i in $$(find . -path ./geometry -prune -not -type d -o -path ./os -prune -not -type d -o -path ./can -prune -not -type d -o -name '*.hpp' -o -name '*.h' | grep -v NSControlKernel.hpp |grep -v BaseKernel.hpp |grep -v all.h); do echo "#include \"$$i\"" >> all.h; done &&\
	echo "#endif" >> all.h &&\
	g++-3.4 -O2 -c -x c++ all.h -o all.h.gch -I. 
	echo "#error all.h.gch is not used. You must use CC=gcc-3.4" > all.h

# This cleans out the object files but leaves
# the headers and library
clean_packages: test_new_packages
	if test -f ecos.ecc; then \
	$(MAKE) -C packages clean ; fi

test_check_packages: .check-stamp

.check-stamp:
	$(MAKE) -C . check_packages

# This checks the configuration
check_packages:
	cd packages ;\
	export ECOS_REPOSITORY=@ECOS_REP@ ;\
	ecosconfig -v check

# This removes the whole ! packages build directory
mrproper:
	rm -rf packages

# This cleans the install directory, meaning
# the install/include, install/lib, install/modules
# and install/bin directories.
clean_install:
	@echo -e "\nThis command will erase the install directory !\n"
	cd packages && \
	rm -rf packages/install

########################################################################
# KUL specific :
########################################################################
new_drivers:
	mkdir -p drv-@ECOS_TARGET@
	cd drv-@ECOS_TARGET@ ;\
	export ECOS_REPOSITORY=@ECOS_REP@ ;\
	ecosconfig -v new @ECOS_TARGET@ kuldrivers > new_drivers.log ; \
	for i in $$(find @ECOS_REP@/support -name "*.cdl"); do \
	ecosconfig add support_$$(basename $$i .cdl) > support.log; done

configure_drivers:
	cd drv-@ECOS_TARGET@ ;\
	export ECOS_REPOSITORY=@ECOS_REP@ ;\
	configtool ecos.ecc

all_drivers:
	cd drv-@ECOS_TARGET@ ;\
	export ECOS_REPOSITORY=@ECOS_REP@ ;\
	ecosconfig -v tree
	$(MAKE) -C drv-@ECOS_TARGET@

check_drivers:
	cd drv-@ECOS_TARGET@ ;\
	export ECOS_REPOSITORY=@ECOS_REP@ ;\
	ecosconfig -v check

clean_drivers:
	$(MAKE) -C drv-@ECOS_TARGET@ clean

#######################################################################
# LVD Specific :
#######################################################################
lvd_new:
	mkdir -p lvd-@ECOS_TARGET@
	cd lvd-@ECOS_TARGET@ ;\
	export ECOS_REPOSITORY=@ECOS_REP@ ;\
	ecosconfig -v new @ECOS_TARGET@ minimal > new_drivers.log ; \
	for i in $$(find @ECOS_REP@/support -name "*.cdl"); do \
	ecosconfig add support_$$(basename $$i .cdl) > support.log; done ;\
	ecosconfig template lvd

lvd_config:
	cd lvd-@ECOS_TARGET@ ;\
	export ECOS_REPOSITORY=@ECOS_REP@ ;\
	configtool ecos.ecc

lvd:
	cd lvd-@ECOS_TARGET@ ;\
	export ECOS_REPOSITORY=@ECOS_REP@ ;\
	ecosconfig -v tree
	$(MAKE) lvd-@ECOS_TARGET@

lvd_clean:
	$(MAKE) -C lvd-@ECOS_TARGET@ clean
