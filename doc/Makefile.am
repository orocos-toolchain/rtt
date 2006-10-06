SUBDIRS = . xml

EXTRA_DIST = \
	CODING_STYLE.txt \
	xsl/orocos.xsl \
	xsl/orocos-html.xsl \
	orocos-html.css \
	docrules.mak

.PHONY:

clean-local:

docs:
	${MAKE} -C xml docs

api:
	${MAKE} -C .. doxy-dist

