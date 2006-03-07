SUBDIRS = .

XMLDOCS=orocos-overview.xml orocos-installation.xml
# the faq only renders correctly in html, the changes is only for online viewing.
EXTRAHTMLDOCS=orocos-faq.html orocos-changes.html orocos-html.css

EXTRAXMLDOCS= $(shell find $(ECOS_REP) -name "orocos-*.xml" ) $(shell find $(DOC_DIR) -name "orocos-*.xml")
CLEANXMLDOCS= $(notdir $(EXTRAXMLDOCS))
#need jpg to include in pdf
# EXTRAJPG= $(shell find $(ECOS_REP) -path "doc/api" -prune -o -path "*jpg" )
EXTRAJPG=$(shell ls $(ECOS_REP)/jpg/*.jpg)
CLEANJPG=$(notdir $(EXTRAJPG))
#need gif to include in pdf
EXTRAGIF=$(shell ls $(ECOS_REP)/gif/*.gif)
CLEANGIF=$(notdir $(EXTRAGIF))
# no need to copy png's the html will link to them
#EXTRAPNG= $(shell find $(ECOS_REP) -path "doc/api" -prune -o -path "*png" )

DOCBOOKS= applications-doc.sgml  deep-shallow-api.sgml      \
	geometry-doc.sgml       motion-api.sgml \
	components-doc.sgml    interpolation-api.sgml     \
	decoupling.sgml        general-dynamics-doc.sgml  \
	kindyn-doc.sgml       robot-api.sgml 

DIAS=ApplicationStack.dia
PNGS= ConfigTool.png ConfigToolAdmin.png \
      ConfigToolPackages.png ConfigToolFlags.png \
      svg/RealTimeToolkit.png svg/ComponentInterface.png \
      svg/ComponentImplementation.png

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
	orocos-html.css \
	catalog.xml.in \
	orocos.ent \
	docrules.mak \
	$(XMLDOCS) \
	$(DOCBOOKS) \
	$(DIAS) \
	$(HTMLDOCS) \
	$(PNGIMGS)

if HAVE_XMLPROCESSOR
DOC = docxml
else
DOC = dochtml
endif

.PHONY: docs docchunks manuals prepdocs

docs: prepdocs $(DOC)

prepdocs:
	mkdir -p svg

ORO_USER_MANUALS=orocos-user-manual orocos-components-manual orocos-devel-manual orocos-control-manual
ORO_MANUAL_TARGETS= $(addsuffix .pdf, $(ORO_USER_MANUALS) ) $(addsuffix .html, $(ORO_USER_MANUALS) )

manuals: pngimages gifimages
	-cp -a $(EXTRAXMLDOCS) .
	-cp -a $(EXTRAGIF) .
	$(RM) $(ORO_MANUAL_TARGETS)
	$(MAKE) $(ORO_MANUAL_TARGETS) XSL_PDF_PARAMS="--stringparam admon.graphics.path ../../doc/images/"
	$(RM) $(CLEANXMLDOCS)
	$(RM) $(CLEANGIF)

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
	$(RM) $(ORO_MANUAL_TARGETS)
	$(RM) svg
