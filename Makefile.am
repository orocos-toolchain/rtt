SUBDIRS = . doc

ECOS_REP=@abs_srcdir@/packages

EXTRA_DIST = \
	packages/ecosadmin.tcl packages/ecosdbgen.py packages/ecospkggen.py \
	packages/configure.ac packages/configure packages/autogen.sh \
	packages/aclocal.m4 packages/acinclude.m4 \
	packages/Makefile.in packages/Doxyfile.in \
	doc/CODING_STYLE.txt \
	packages/support/boost/current/boost.cdl.in \
	packages/support/comedi/current/comedi.cdl.in \
	packages/support/comedilib/current/comedilib.cdl.in \
	packages/support/xercesc/current/xercesc.cdl.in \
	packages/pkgconf/fixhtml.tcl \
        packages/pkgconf/rules.doc \
        packages/pkgconf/rules.mak \
        packages/pkgconf/ssa4.dsl \
        packages/pkgconf/ssletter.dsl \
        packages/pkgconf/stylesheet.dsl

packages/ecos.db:
	touch @abs_srcdir@/packages/ecos.db

new_packages:
	mkdir -p packages-@ECOS_TARGET@
	cd packages-@ECOS_TARGET@ ;\
	export ECOS_REPOSITORY=${ECOS_REP} ;\
	ecosconfig new @ECOS_TARGET@ minimal ;\
	for i in $$(find ${ECOS_REP}/support -name "*.cdl"); do \
	ecosconfig add support_$$(basename $$i .cdl); done ;\
	ecosconfig template allpackages

configure_packages:
	cd packages-@ECOS_TARGET@ ;\
	export ECOS_REPOSITORY=${ECOS_REP} ;\
	configtool ecos.ecc

all_packages:
	cd packages-@ECOS_TARGET@ ;\
	export ECOS_REPOSITORY=${ECOS_REP} ;\
	ecosconfig tree ;\
	make

clean_packages:
	cd packages-@ECOS_TARGET@ && \
	make clean

check_packages:
	cd packages-@ECOS_TARGET@ ;\
	export ECOS_REPOSITORY=${ECOS_REP} ;\
	ecosconfig template default ;\
	ecosconfig add applications_testcase ;\
	ecosconfig tree ;\
	make

mrproper:
	rm -rf packages-@ECOS_TARGET@

clean_install:
	@echo -e "\nThis command will erase the install directory !\n"
	cd packages-@ECOS_TARGET@ && \
	rm -rf packages-@ECOS_TARGET@/install

# KUL specific :

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


