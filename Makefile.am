SUBDIRS = .

ECOS_REP=@abs_srcdir@/packages

new_packages:
	mkdir -p packages-@ECOS_TARGET@
	cd packages-@ECOS_TARGET@ ;\
	export ECOS_REPOSITORY=${ECOS_REP} ;\
	ecosconfig new @ECOS_TARGET@ allpackages ;\
	for i in $$(find ${ECOS_REP}/support -name "*.cdl"); do \
	ecosconfig add support_$$(basename $$i .cdl); done

configure_packages:
	cd packages-@ECOS_TARGET@ ;\
	export ECOS_REPOSITORY=${ECOS_REP} ;\
	configtool ecos.ecc

make_packages:
	cd packages-@ECOS_TARGET@ ;\
	export ECOS_REPOSITORY=${ECOS_REP} ;\
	ecosconfig tree ;\
	make

clean_packages:
	@echo -e "\nThis command will erase the install directory !\n"
	cd packages-@ECOS_TARGET@ ;\
	rm -rf packages-@ECOS_TARGET@/install

check_packages:
	cd packages-@ECOS_TARGET@ ;\
	export ECOS_REPOSITORY=${ECOS_REP} ;\
	ecosconfig add applications_testcase ;\
	ecosconfig tree ;\
	make

# KUL specific :

new_drivers:
	mkdir -p drv-@ECOS_TARGET@
	cd drv-@ECOS_TARGET@ ;\
	export ECOS_REPOSITORY=${ECOS_REP} ;\
	ecosconfig -v new @ECOS_TARGET@ kuldrivers

configure_drivers:
	cd drv-@ECOS_TARGET@ ;\
	export ECOS_REPOSITORY=${ECOS_REP} ;\
	configtool ecos.ecc

make_drivers:
	cd drv-@ECOS_TARGET@ ;\
	export ECOS_REPOSITORY=${ECOS_REP} ;\
	ecosconfig -v tree ;\
	make

check_drivers:
	cd drv-@ECOS_TARGET@ ;\
	export ECOS_REPOSITORY=${ECOS_REP} ;\
	ecosconfig -v check

clean_drivers:
	rm -rf drv-@ECOS_TARGET@


