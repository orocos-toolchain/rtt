SUBDIRS = . doc

ECOS_REP=@abs_srcdir@/packages

# These are not distributed by default (adds to many megs)
# The i386linux package will copy them.
#	tools/bin/ecosconfig tools/bin/configtool 

EXTRA_DIST = \
	packages/ecosadmin.tcl packages/ecosdbgen.py packages/ecospkggen.py \
	packages/configure.ac packages/configure bootstrap.sh \
	packages/aclocal.m4 packages/acinclude.m4 \
	packages/Makefile.in packages/Doxyfile.in \
	doc/CODING_STYLE.txt \
	packages/templates/minimal/current.ect \
	packages/support/boost/current/boost.cdl.in \
	packages/support/boost_graph/current/boost_graph.cdl.in \
	packages/support/comedi/current/comedi.cdl.in \
	packages/support/comedilib/current/comedilib.cdl.in \
	packages/support/xercesc/current/xercesc.cdl.in \
	packages/pkgconf/fixhtml.tcl \
        packages/pkgconf/rules.doc \
        packages/pkgconf/rules.mak \
        packages/pkgconf/ssa4.dsl \
        packages/pkgconf/ssletter.dsl \
        packages/pkgconf/stylesheet.dsl

docs:
	$(MAKE) -C doc docs

packages/ecos.db:
	touch @abs_srcdir@/packages/ecos.db

# This sets up a new packages build directory.
# You only need to do this once.
new_packages:
	mkdir -p packages-@ECOS_TARGET@
	@echo "Setting up packages-@ECOS_TARGET@..."
	@cd packages-@ECOS_TARGET@ &&\
	export ECOS_REPOSITORY=${ECOS_REP};\
	ecosconfig new @ECOS_TARGET@ minimal ;\
	for i in $$(find ${ECOS_REP}/support -name "*.cdl"); do \
	ecosconfig add support_$$(basename $$i .cdl); done ;\
	{ ecosconfig template allpackages || ecosconfig template corelib-os; };\
	echo -e "\n This is now your new configuration :\n" ;\
	ecosconfig check


# This configures the selected packages with
# a graphical tool.
configure_packages:
	cd packages-@ECOS_TARGET@ &&\
	export ECOS_REPOSITORY=${ECOS_REP} &&\
	configtool ecos.ecc

# This makes the ecos makefiles in the packages
# build directory and makes the library
all_packages:
	cd packages-@ECOS_TARGET@ &&\
	export ECOS_REPOSITORY=${ECOS_REP} &&\
	ecosconfig template allpackages &&\
	ecosconfig tree &&\
	make

# This cleans out the object files but leaves
# the headers and library
clean_packages:
	cd packages-@ECOS_TARGET@ && \
	make clean && \
	rm install/lib/libtarget.a

# This checks the configuration
check_packages:
	cd packages-@ECOS_TARGET@ ;\
	export ECOS_REPOSITORY=${ECOS_REP} ;\
	ecosconfig -v check

# This removes the whole ! packages build directory
mrproper:
	rm -rf packages-@ECOS_TARGET@

# This cleans the install directory, meaning
# the install/include, install/lib, install/modules
# and install/bin directories.
clean_install:
	@echo -e "\nThis command will erase the install directory !\n"
	cd packages-@ECOS_TARGET@ && \
	rm -rf packages-@ECOS_TARGET@/install

########################################################################
# KUL specific :
########################################################################
new_drivers:
	mkdir -p drv-@ECOS_TARGET@
	cd drv-@ECOS_TARGET@ ;\
	export ECOS_REPOSITORY=${ECOS_REP} ;\
	ecosconfig -v new @ECOS_TARGET@ kuldrivers > new_drivers.log ; \
	for i in $$(find ${ECOS_REP}/support -name "*.cdl"); do \
	ecosconfig add support_$$(basename $$i .cdl) > support.log; done

configure_drivers:
	cd drv-@ECOS_TARGET@ ;\
	export ECOS_REPOSITORY=${ECOS_REP} ;\
	configtool ecos.ecc

all_drivers:
	cd drv-@ECOS_TARGET@ ;\
	export ECOS_REPOSITORY=${ECOS_REP} ;\
	ecosconfig -v tree ;\
	make

check_drivers:
	cd drv-@ECOS_TARGET@ ;\
	export ECOS_REPOSITORY=${ECOS_REP} ;\
	ecosconfig -v check

clean_drivers:
	cd drv-@ECOS_TARGET@ ;\
	make clean

#######################################################################
# LVD Specific :
#######################################################################
lvd_new:
	mkdir -p lvd-@ECOS_TARGET@
	cd lvd-@ECOS_TARGET@ ;\
	export ECOS_REPOSITORY=${ECOS_REP} ;\
	ecosconfig -v new @ECOS_TARGET@ minimal > new_drivers.log ; \
	for i in $$(find ${ECOS_REP}/support -name "*.cdl"); do \
	ecosconfig add support_$$(basename $$i .cdl) > support.log; done ;\
	ecosconfig template lvd

lvd_config:
	cd lvd-@ECOS_TARGET@ ;\
	export ECOS_REPOSITORY=${ECOS_REP} ;\
	configtool ecos.ecc

lvd:
	cd lvd-@ECOS_TARGET@ ;\
	export ECOS_REPOSITORY=${ECOS_REP} ;\
	ecosconfig -v tree ;\
	make

lvd_clean:
	cd lvd-@ECOS_TARGET@ ;\
	make clean
