SUBDIRS = .

XMLDOCS=orocos-overview.xml orocos-installation.xml
# the faq only renders correctly in html, the changes is only for online viewing.
EXTRAHTMLDOCS=orocos-faq.html orocos-changes.html

EXTRAXMLDOCS= $(shell find $(ECOS_REP) -name "orocos-*.xml" ) $(shell find $(DOC_DIR) -name "orocos-*.xml")
CLEANXMLDOCS= $(notdir $(EXTRAXMLDOCS))
#need jpg to include in pdf
# EXTRAJPG= $(shell find $(ECOS_REP) -path "doc/api" -prune -o -path "*jpg" )
EXTRAJPG=$(shell ls $(ECOS_REP)/jpg/*.jpg)
CLEANJPG=$(notdir $(EXTRAJPG))
# no need to copy png's the html will link to them
#EXTRAPNG= $(shell find $(ECOS_REP) -path "doc/api" -prune -o -path "*png" )

DOCBOOKS= applications-doc.sgml  deep-shallow-api.sgml      \
	geometry-doc.sgml       motion-api.sgml \
	components-doc.sgml    interpolation-api.sgml     \
	decoupling.sgml        general-dynamics-doc.sgml  \
	kindyn-doc.sgml       robot-api.sgml 

DIAS=ApplicationStack.dia

FOP_STYLESHEET := $(srcdir)/orocos.xsl
HTML_STYLESHEET := $(srcdir)/orocos-html.xsl

include $(srcdir)/docrules.mak

CHUNKSDOCS= $(patsubst %.xml,%.chunks,$(XMLDOCS)) # $(patsubst %.sgml,%.chunks,$(DOCBOOKS))
WEBSITEDOCS= $(patsubst %.xml,%.website,$(XMLDOCS)) # $(patsubst %.sgml,%.website,$(DOCBOOKS))

EXTRA_DIST = \
	CODING_STYLE.txt \
	docrules.mak \
	orocos.xsl \
	orocos-html.xsl \
	catalog.xml.in \
	orocos.ent \
	docrules.mak \
	$(XMLDOCS) \
	$(DOCBOOKS) \
	$(DIAS) \
	$(HTMLDOCS) \
	$(PDFDOCS)
# we do not distribute the ps files, they are to large,
# but i make them for the website


if HAVE_XMLPROCESSOR
DOC = docxml
else
DOC = dochtml
endif

.PHONY: docs docchunks manual control-manual

docs: $(DOC)

manual: pngimages jpgimages
	-cp -a $(EXTRAXMLDOCS) .
	-cp -a $(EXTRAJPG) .
	$(RM)  orocos-manual.html orocos-manual.pdf 
	$(MAKE) orocos-manual.html orocos-manual.pdf 
	$(RM) $(CLEANXMLDOCS)
	$(RM) $(CLEANJPG)

control-manual: pngimages jpgimages
	-cp -a $(EXTRAXMLDOCS) .
	-cp -a $(EXTRAJPG) .
	$(RM)  orocos-control-manual.html orocos-control-manual.pdf 
	$(MAKE) orocos-control-manual.html orocos-control-manual.pdf 
	$(RM) $(CLEANXMLDOCS)
	$(RM) $(CLEANJPG)

docchunks: $(CHUNKSDOCS)

docwebsite: $(WEBSITEDOCS)

.xml.chunks:
	$(XMLPROCESSOR) --xinclude  --stringparam base.dir ./$(subst orocos-,,$(basename $(notdir $<)))/  $(srcdir)/orocos-chunk.xsl $< 
	touch $@

.xml.website:
	$(XMLPROCESSOR) --xinclude  --stringparam base.dir ./$(subst orocos-,,$(basename $(notdir $<)))/  $(srcdir)/orocos-website.xsl $<  > $@

clean-local:
	$(RM) $(HTMLDOCS) $(PDFDOCS) $(PSDOCS) $(PDFDOCS) $(TXTDOCS) $(EPSIMGS) $(PNGIMGS) $(JPGIMGS) *.chunks *.website
	$(RM) -r $(subst orocos-,,$(basename $(XMLDOCS)))
	$(RM) $(CLEANXMLDOCS)
	$(RM) $(CLEANJPG)
	$(RM)  orocos-manual.html orocos-manual.ps orocos-manual.pdf 
	$(RM)  orocos-control-manual.html orocos-control-manual.ps orocos-control-manual.pdf 
