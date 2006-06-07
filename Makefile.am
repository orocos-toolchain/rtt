SUBDIRS = . tests doc

# These are not distributed by default (adds to many megs)
# The i386linux package will copy them.
#	tools/bin/ecosconfig tools/bin/configtool 

EXTRA_DIST = \
	packages/ecosadmin.tcl packages/ecosdbgen.py packages/ecospkggen.py \
	packages/configure.ac packages/configure \
	packages/aclocal.m4 packages/acinclude.m4 \
	packages/Makefile.in Doxyfile.in \
	doc/CODING_STYLE.txt \
	packages/support/common/current/support.cdl.in \
	packages/support/boost/current/boost.cdl.in \
	packages/support/boost/current/include \
	packages/support/comedi/current/comedi.cdl.in \
	packages/support/xercesc/current/xercesc.cdl.in \
	packages/support/readline/current/readline.cdl.in \
	packages/support/rtai/current/rtai.cdl.in \
	packages/support/xenomai/current/xenomai.cdl.in \
	packages/support/gcc/current/gcc.cdl.in \
	packages/support/corba/current/corba.cdl.in \
	packages/pkgconf/fixhtml.tcl \
        packages/pkgconf/rules.doc \
        packages/pkgconf/rules.mak \
        packages/pkgconf/ssa4.dsl \
        packages/pkgconf/ssletter.dsl \
        packages/pkgconf/stylesheet.dsl \
	packages/ecos.db.in \
	packages/catalog.xml

# EXTRA_DIST = \
# 	packages

DISTCHECK_CONFIGURE_FLAGS = --disable-tests

PATHHEADERS=$$(cd packages/install/include && find . -type f )
PATHEXISTS =$$( if test -d packages/install/include; echo yes; fi )

.PHONY: docs myinstall myuninstall new_packages configure_packages all_packages dist_packages clean_packages init_packages inst-headers check-headers

docs:
	$(MAKE) -C doc docs

packages/ecos.db:
	touch $(abs_srcdir)/packages/ecos.db

inst-headers:
	${MAKE} -C packages headers

check-headers: clean_headers init_packages inst-headers
	cd packages/install/include ;\
	tname=$$(tempfile -p check -s .cpp) ;\
	for i in $$(find . -maxdepth 2 -name "*.h" -o -name "*.hpp"); do \
	echo  "Checking $$i..." ;\
	echo -e "#include <$$i>\n" > $$tname ;\
	${CC} -pipe -Wall -S $$tname -I. ;\
	done ; rm -f $$tname $$(basename $$tname cpp)s

clean-local: clean_packages

uninstall-local:
	@for i in ${PATHHEADERS}; do rm -fv $(DESTDIR)$(prefix)/include/$$i; done
	@rm -fv $(DESTDIR)$(prefix)/lib/liborocos-$(ECOS_TARGET).a
	@rm -fv $(DESTDIR)$(prefix)/lib/libtarget.a
	@rm -rf $(DESTDIR)$(prefix)/modules/

install-exec-local:
	@echo "Installing new headers..."
	@install -m 755 -d $(DESTDIR)$(prefix)/include
	@for i in ${PATHHEADERS};\
	do if test packages/install/include/$$i -nt $(DESTDIR)$(prefix)/include/$$i ;\
	then  install -p -m 644 -D packages/install/include/$$i $(DESTDIR)$(prefix)/include/$$i; echo "install $$i $(DESTDIR)$(prefix)/include/$$i";fi; done
	@echo "Installing new library..."
	@install -m 755 -d $(DESTDIR)$(prefix)/lib
	@for i in $$( ls packages/install/lib/libtarget.a ); do \
	if test packages/install/lib/libtarget.a -nt $(DESTDIR)$(prefix)/lib/liborocos-$(ECOS_TARGET).a ; then \
	install -m 644 packages/install/lib/libtarget.a $(DESTDIR)$(prefix)/lib/liborocos-$(ECOS_TARGET).a ;\
	ranlib  $(DESTDIR)$(prefix)/lib/liborocos-$(ECOS_TARGET).a ;\
	ln -sf  $(DESTDIR)$(prefix)/lib/liborocos-$(ECOS_TARGET).a $(DESTDIR)$(prefix)/lib/libtarget.a ; fi; done
	@libtool --mode=finish $(DESTDIR)$(prefix)/lib
	@echo "Installing new modules"
	if test -d packages/install/modules; then install -m 755 -d $(DESTDIR)$(prefix)/modules ; fi
	@for i in $$( find packages/install/modules/ -type f ); do \
	install -p -m 644 $$i $(DESTDIR)$(prefix)/modules/; echo "install $$i $(DESTDIR)$(prefix)/modules"; done
	@echo "Installing tests..."
	mkdir -p $(DESTDIR)$(prefix)/tests
	@for i in $$( find tests/ -maxdepth 1 -type f); do \
	if test -x $$i; then install -p -m 755 $$i $(DESTDIR)$(prefix)/tests/; echo "install $$i $(DESTDIR)$(prefix)/tests"; fi; \
	done

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
	cd packages &&\
	export ECOS_REPOSITORY=$(ECOS_REP);\
	$(ECOSCONFIG) new $(ECOS_TARGET) minimal 2>&1  ;\
	if test -f ecos.ecc; then \
	echo "Setting up support..." ; \
	$(ECOSCONFIG) add support 2>&1 > /dev/null ; \
	for i in $$(find $(ECOS_REP)/support -name "*.cdl" -a -not -name "support.cdl"); do \
	$(ECOSCONFIG) add support_$$(basename $$i .cdl) 2>&1 >/dev/null ; done ;\
	{ $(ECOSCONFIG) template dist-packages || $(ECOSCONFIG) template corelib-os; };\
	echo -e "\n This is now your new configuration :\n" ;\
	$(ECOSCONFIG) check ;\
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
	$(CONFIGTOOL) orocos.ecc &&\
	$(ECOSCONFIG) export .upgrade.ecc


test_all_packages: .all-stamp

#Only executed if new_packages did not create this stamp.
.all-stamp:
	$(MAKE) -C . packages

all_packages: packages

cross_packages: cross

# make all dist-packages
# it will exit if no ecos.ecc file is found.
dist_packages:
	if test -f packages/ecos.ecc; then \
	cd packages; \
	export ECOS_REPOSITORY=$(ECOS_REP) ;\
	$(ECOSCONFIG) template dist-packages;\
	$(ECOSCONFIG) check ; fi
	$(MAKE) -C . packages

# make all packages from current template
# in packages dir, then export the config info.
packages: init_packages
	cd packages &&\
	{ test -f ecos.ecc || exit 0; } &&\
	{ $(MAKE) -C . CC=$(CC) || exit -1; } &&\
	ECOS_REPOSITORY=$(ECOS_REP) $(ECOSCONFIG) export .upgrade.ecc &&\
	cd install/lib && ln -sf libtarget.a liborocos-$(ECOS_TARGET).a

# make all packages from current template with ecosconfig compiler.
# in packages dir, then export the config info.
cross: init_packages
	cd packages &&\
	{ test -f ecos.ecc || exit 0; } &&\
	{ $(MAKE) -C . || exit -1; } &&\
	ECOS_REPOSITORY=$(ECOS_REP) $(ECOSCONFIG) export .upgrade.ecc &&\
	cd install/lib && ln -sf libtarget.a liborocos-$(ECOS_TARGET).a

init_packages: test_new_packages
	@cd packages &&\
	{ test -f ecos.ecc || exit 0; } &&\
	export ECOS_REPOSITORY=$(ECOS_REP) &&\
	{ $(ECOSCONFIG) tree || { echo -e "\n$(ECOSCONFIG) tree failed.\n  Try 'make upgrade_packages' or contact the Orocos-dev mailinglist.\n"; exit -1; } }

precompile_headers:
	$(MAKE) headers -C packages
	cd packages/install/include &&\
	rm -f all.h &&\
	echo "#ifdef __cplusplus" >> all.h &&\
	for i in $$(find . -path ./geometry -prune -not -type d -o -path ./os -prune -not -type d -o -path ./can -prune -not -type d -o -name '*.hpp' -o -name '*.h' | grep -v NSControlKernel.hpp |grep -v BaseKernel.hpp |grep -v all.h); do echo "#include \"$$i\"" >> all.h; done &&\
	echo "#endif" >> all.h &&\
	g++-3.4 -O2 -c -x c++ all.h -o all.h.gch -I. 
	echo "#error all.h.gch is not used. You must use CC=gcc-3.4" > all.h

# Reset the template to build the controlservices
control_services: test_new_packages
	@if test -f packages/ecos.ecc; then \
	cd packages; \
	export ECOS_REPOSITORY=$(ECOS_REP) ;\
	$(ECOSCONFIG) template control-services;\
	$(ECOSCONFIG) check ; fi
	$(MAKE) -C . packages

#Build most of Orocos, except device drivers.
robot_control: test_new_packages dist_packages

doxygen_headers: test_new_packages
	@if test -f packages/ecos.ecc; then \
	cd packages; \
	export ECOS_REPOSITORY=$(ECOS_REP) ;\
	$(ECOSCONFIG) template doxygen;\
	$(ECOSCONFIG) resolve;\
	$(ECOSCONFIG) tree ;fi
	$(MAKE) -C . inst-headers

doxy-dist: test_new_packages 
	@echo "Running Doxygen... (warnings in doxygen.warnings)"
	@rm -rf doc/api
	@mkdir -p doc/api	
	@@DOXYGEN@ || (echo "doxygen failed !" ; exit 1)
	@if [ -d doc ] ; then echo "Tarring API docs..." ;\
	tar -cjf orocos-api.tar.bz2 doc/api; fi

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
	$(ECOSCONFIG) -v check

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
	@if test -f packages/ecos.ecc; then \
	export ECOS_REPOSITORY=$(ECOS_REP) ; cd packages ;\
	$(ECOSCONFIG) new $(ECOS_TARGET) minimal ;\
	$(ECOSCONFIG) import .upgrade.ecc ;\
	$(ECOSCONFIG) resolve || echo -e "\nUpgrade failed. Start with a new configuration or manually remove the offending option from the packages/orocos.ecc file.\n" ;\
	echo "" ; $(ECOSCONFIG) check ;\
	fi

new_idl:
	for i in $(find $(ECOS_REP) -name "*.idl"); do \
	echo "Generating source files from idl..." ;\
	cd $(dirname $i) ;\
	$(TAO_IDL) -I. -I$TAO_ROOT -I$ACE_ROOT/include -I$TAO_ROOT/orbsvcs $(basename $i) ;\
	cd - ;\
	done


########################################################################
# KUL specific :
########################################################################
new_drivers:
	mkdir -p drv-$(ECOS_TARGET)
	cd drv-$(ECOS_TARGET) ;\
	export ECOS_REPOSITORY=$(ECOS_REP) ;\
	$(ECOSCONFIG) -v new $(ECOS_TARGET) kuldrivers > new_drivers.log ; \
	for i in $$(find $(ECOS_REP)/support -name "*.cdl"); do \
	$(ECOSCONFIG) add support_$$(basename $$i .cdl) > support.log; done

configure_drivers:
	cd drv-$(ECOS_TARGET) ;\
	export ECOS_REPOSITORY=$(ECOS_REP) ;\
	$(CONFIGTOOL) ecos.ecc

all_drivers:
	cd drv-$(ECOS_TARGET) ;\
	export ECOS_REPOSITORY=$(ECOS_REP) ;\
	$(ECOSCONFIG) -v tree
	$(MAKE) -C drv-$(ECOS_TARGET)

check_drivers:
	cd drv-$(ECOS_TARGET) ;\
	export ECOS_REPOSITORY=$(ECOS_REP) ;\
	$(ECOSCONFIG) -v check

clean_drivers:
	$(MAKE) -C drv-$(ECOS_TARGET) clean

