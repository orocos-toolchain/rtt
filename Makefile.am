SUBDIRS = . tests doc

# These are not distributed by default (adds to many megs)
# The i386linux package will copy them.
#	tools/bin/ecosconfig tools/bin/configtool 

EXTRA_DIST = \
	packages/ecosadmin.tcl packages/ecosdbgen.py packages/ecospkggen.py \
	packages/configure.ac packages/configure \
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
        packages/pkgconf/stylesheet.dsl \
	packages/ecos.db.in

# EXTRA_DIST = \
# 	packages

DISTCHECK_CONFIGURE_FLAGS = --disable-tests

PATHHEADERS=$$(cd packages/install/include; find . -type f )
PATHEXISTS =$$( if test -d packages/install/include; echo yes; fi )

.PHONY: docs myinstall myuninstall new_packages configure_packages all_packages dist_packages clean_packages init_packages inst-headers check-headers

docs:
	$(MAKE) -C doc docs

packages/ecos.db:
	touch $(abs_srcdir)/packages/ecos.db

inst-headers:
	${MAKE} -C packages headers

check-headers: clean_headers init_packages inst-headers
	@cd packages/install/include ;\
	for i in $$(find . -name *.h -o -name *.hpp); do \
	echo  "Checking $$i..." ;\
	echo -e "#include <$$i>\n" > /tmp/tmp_hpp_chk.cpp ;\
	${CC} -c /tmp/tmp_hpp_chk.cpp -I. ;\
	done

clean-local: clean_packages

uninstall-local:
	@for i in ${PATHHEADERS}; do rm -fv $(prefix)/include/$$i; done
	@rm -fv $(prefix)/lib/liborocos-$(ECOS_TARGET).a
	@rm -fv $(prefix)/lib/libtarget.a

install-exec-local:
	install -m 755 -d $(prefix)/include
	for i in ${PATHHEADERS}; do install -p -m 644 -D packages/install/include/$$i $(prefix)/include/$$i; echo "install $$i $(prefix)/include/$$i"; done
	install -m 755 -d $(prefix)/lib
	install -m 644 packages/install/lib/libtarget.a $(prefix)/lib/liborocos-$(ECOS_TARGET).a
	ranlib  $(prefix)/lib/liborocos-$(ECOS_TARGET).a
	ln -sf  $(prefix)/lib/liborocos-$(ECOS_TARGET).a $(prefix)/lib/libtarget.a
	libtool --mode=finish $(prefix)/lib

all-local: test_new_packages test_all_packages

distclean-local: mrproper
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
	export ECOS_REPOSITORY=$(ECOS_REP);\
	ecosconfig new $(ECOS_TARGET) minimal 2>&1 > /dev/null ;\
	if test -f ecos.ecc; then \
	echo "Setting up support..." ; \
	ecosconfig add support 2>&1 > /dev/null ; \
	for i in $$(find $(ECOS_REP)/support -name "*.cdl" -a -not -name "support.cdl"); do \
	ecosconfig add support_$$(basename $$i .cdl) 2>/dev/null ; done ;\
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
	@cd packages &&\
	export ECOS_REPOSITORY=$(ECOS_REP) &&\
	ln -sf ecos.ecc orocos.ecc &&\
	configtool orocos.ecc &&\
	ecosconfig export .upgrade.ecc


test_all_packages: .all-stamp

#Only executed if new_packages did not create this stamp.
.all-stamp:
	$(MAKE) -C . packages

all_packages: packages

# make all dist-packages
# it will exit if no ecos.ecc file is found.
dist_packages: 
	if test -f packages/ecos.ecc; then \
	cd packages; \
	export ECOS_REPOSITORY=$(ECOS_REP) ;\
	ecosconfig template dist-packages;\
	ecosconfig check ; fi
	$(MAKE) -C . packages

# make all packages from current template
# in packages dir, then export the config info.
packages: init_packages
	cd packages &&\
	{ test -f ecos.ecc || exit 0; } &&\
	{ $(MAKE) -C . || exit -1; } &&\
	ECOS_REPOSITORY=$(ECOS_REP) ecosconfig export .upgrade.ecc &&\
	cd install/lib && ln -sf libtarget.a liborocos-$(ECOS_TARGET).a

init_packages: test_new_packages
	@cd packages &&\
	{ test -f ecos.ecc || exit 0; } &&\
	export ECOS_REPOSITORY=$(ECOS_REP) &&\
	{ ecosconfig tree || { echo -e "\necosconfig tree failed.\n  Try 'make upgrade_packages' or contact the Orocos-dev mailinglist.\n"; exit -1; } }

precompile_headers:
	$(MAKE) headers -C packages
	cd packages/install/include &&\
	rm -f all.h &&\
	echo "#ifdef __cplusplus" >> all.h &&\
	for i in $$(find . -path ./geometry -prune -not -type d -o -path ./os -prune -not -type d -o -path ./can -prune -not -type d -o -name '*.hpp' -o -name '*.h' | grep -v NSControlKernel.hpp |grep -v BaseKernel.hpp |grep -v all.h); do echo "#include \"$$i\"" >> all.h; done &&\
	echo "#endif" >> all.h &&\
	g++-3.4 -O2 -c -x c++ all.h -o all.h.gch -I. 
	echo "#error all.h.gch is not used. You must use CC=gcc-3.4" > all.h

# Reset the template to build the coreservices
core_packages: test_new_packages
	@if test -f packages/ecos.ecc; then \
	cd packages; \
	export ECOS_REPOSITORY=$(ECOS_REP) ;\
	ecosconfig template controlservices;\
	ecosconfig check ; fi
	$(MAKE) -C . packages

# This cleans out the object files but leaves
# the headers and library
clean_packages: test_new_packages
	if test -f packages/ecos.ecc; then \
	$(MAKE) -C packages clean ; fi

clean_headers: test_new_packages
	if test -f packages/ecos.ecc; then \
	$(RM) -r packages/install/include ; fi

test_check_packages: .check-stamp

.check-stamp:
	$(MAKE) -C . check_packages

# This checks the configuration
check_packages:
	cd packages ;\
	export ECOS_REPOSITORY=$(ECOS_REP) ;\
	ecosconfig -v check

# This removes the whole ! packages build directory
mrproper:
	@rm -rvf packages

# This cleans the install directory, meaning
# the install/include, install/lib, install/modules
# and install/bin directories.
clean_install: clean-local
	@echo -e "\nThis command will erase the install directory !\n"
	@rm -rvf packages/install

# use this to *try* to fix your .ecc file in an upgrade
upgrade_packages:
	export ECOS_REPOSITORY=$(ECOS_REP) ; cd packages ;\
	$(MAKE) -C .. new_packages &&\
	ecosconfig import .upgrade.ecc

########################################################################
# KUL specific :
########################################################################
new_drivers:
	mkdir -p drv-$(ECOS_TARGET)
	cd drv-$(ECOS_TARGET) ;\
	export ECOS_REPOSITORY=$(ECOS_REP) ;\
	ecosconfig -v new $(ECOS_TARGET) kuldrivers > new_drivers.log ; \
	for i in $$(find $(ECOS_REP)/support -name "*.cdl"); do \
	ecosconfig add support_$$(basename $$i .cdl) > support.log; done

configure_drivers:
	cd drv-$(ECOS_TARGET) ;\
	export ECOS_REPOSITORY=$(ECOS_REP) ;\
	configtool ecos.ecc

all_drivers:
	cd drv-$(ECOS_TARGET) ;\
	export ECOS_REPOSITORY=$(ECOS_REP) ;\
	ecosconfig -v tree
	$(MAKE) -C drv-$(ECOS_TARGET)

check_drivers:
	cd drv-$(ECOS_TARGET) ;\
	export ECOS_REPOSITORY=$(ECOS_REP) ;\
	ecosconfig -v check

clean_drivers:
	$(MAKE) -C drv-$(ECOS_TARGET) clean

