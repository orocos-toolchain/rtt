SUBDIRS = .

EXTRA_DIST = \
	CODING_STYLE.txt \
	orocos.xsl \
	$(XMLDOCS) \
	$(DOCBOOKS) \
	$(DIAS) \
	$(HTMLDOCS) \
	$(PDFDOCS)
# we do not distribute the ps files, they are to large,
# but i make them for the website

XMLDOCS=orocos-overview.xml orocos-installation.xml orocos-changes.xml orocos-faq.xml
XML_CATALOG_FILES="catalog.xml"

EXTRAXMLDOCS= $(shell find $(ECOS_REP) -name "orocos-*.xml" ) $(shell find $(DOC_DIR) -name "orocos-*.xml")
CLEANXMLDOCS= $(notdir $(EXTRAXMLDOCS))
#need eps to include in pdf
# EXTRAEPS= $(shell find $(ECOS_REP) -path "doc/api" -prune -o -path "*eps" )
EXTRAEPS=$(shell ls $(ECOS_REP)/eps/*.eps)
CLEANEPS=$(notdir $(EXTRAEPS))
# no need to copy png's the html will link to them
#EXTRAPNG= $(shell find $(ECOS_REP) -path "doc/api" -prune -o -path "*png" )

DOCBOOKS= applications-doc.sgml  deep-shallow-api.sgml      \
	geometry-doc.sgml       motion-api.sgml \
	components-doc.sgml    interpolation-api.sgml     \
	decoupling.sgml        general-dynamics-doc.sgml  \
	kindyn-doc.sgml       robot-api.sgml 

DIAS=ApplicationStack.dia

HTMLDOCS= $(patsubst %.xml,%.html,$(XMLDOCS)) $(patsubst %.sgml,%.html,$(DOCBOOKS))
PDFDOCS= $(patsubst %.xml,%.pdf,$(XMLDOCS))  #$(patsubst %.sgml,%.pdf,$(DOCBOOKS))
PSDOCS= $(patsubst %.xml,%.ps,$(XMLDOCS))  #$(patsubst %.sgml,%.ps,$(DOCBOOKS))
TXTDOCS= $(patsubst %.xml,%.txt,$(XMLDOCS)) # $(patsubst %.sgml,%.txt,$(DOCBOOKS))
CHUNKSDOCS= $(patsubst %.xml,%.chunks,$(XMLDOCS)) # $(patsubst %.sgml,%.chunks,$(DOCBOOKS))
WEBSITEDOCS= $(patsubst %.xml,%.website,$(XMLDOCS)) # $(patsubst %.sgml,%.website,$(DOCBOOKS))

EPSIMGS= $(patsubst %.dia,%.eps,$(DIAS))
PNGIMGS= $(patsubst %.dia,%.png,$(DIAS))
JPGIMGS= $(patsubst %.dia,%.jpg,$(DIAS))
TIFFIMGS= $(patsubst %.dia,%.tiff,$(DIAS))


if HAVE_XMLPROCESSOR
DOC = docxml
else
DOC = dochtml
endif

.PHONY: docs docxml dochtml docpdf docps doctxt pngimages epsimages docchunks manual control-manual

docs: $(DOC)

manual: $(CLEANEPS) $(CLEANXMLDOCS) pngimages epsimages
	-cp -a $(EXTRAXMLDOCS) .
	-cp -a $(EXTRAEPS) .
	$(RM)  orocos-manual.html orocos-manual.ps orocos-manual.pdf 
	$(MAKE) orocos-manual.html orocos-manual.ps orocos-manual.pdf 
	$(RM) $(CLEANXMLDOCS)
	$(RM) $(CLEANEPS)

control-manual: $(CLEANEPS) $(CLEANXMLDOCS) pngimages epsimages
	-cp -a $(EXTRAXMLDOCS) .
	-cp -a $(EXTRAEPS) .
	$(RM)  orocos-control-manual.html orocos-control-manual.ps orocos-control-manual.pdf 
	$(MAKE) orocos-control-manual.html orocos-control-manual.ps orocos-control-manual.pdf 
	$(RM) $(CLEANXMLDOCS)
	$(RM) $(CLEANEPS)

docxml: dochtml docpdf docps

dochtml: pngimages $(HTMLDOCS)

docpdf: tiffimages $(PDFDOCS)

docps: jpgimages $(PSDOCS)

doctxt: $(TXTDOCS)

docchunks: $(CHUNKSDOCS)

docwebsite: $(WEBSITEDOCS)

pngimages: $(PNGIMGS)

epsimages: $(EPSIMGS)

jpgimages: $(JPGIMGS)

tiffimages: $(TIFFIMGS)

.sgml.html:
	docbook2html -u $<

.sgml.ps:
	docbook2ps $<

.sgml.pdf:
	docbook2pdf $<

.xml.html:
	XML_CATALOG_FILES=$(XML_CATALOG_FILES) \
	$(XMLPROCESSOR)  --xinclude $(srcdir)/orocos-html.xsl $< > $@

.xml.chunks:
	$(XMLPROCESSOR) --xinclude  --stringparam base.dir ./$(subst orocos-,,$(basename $(notdir $<)))/  $(srcdir)/orocos-chunk.xsl $< 
	touch $@

.xml.website:
	$(XMLPROCESSOR) --xinclude  --stringparam base.dir ./$(subst orocos-,,$(basename $(notdir $<)))/  $(srcdir)/orocos-website.xsl $<  > $@

.xml.fo:
	XML_CATALOG_FILES=$(XML_CATALOG_FILES) \
	$(XMLPROCESSOR) --xinclude $(srcdir)/orocos.xsl $< > $@

.fo.ps:
	$(FOP) -c $(DOC_DIR)/fopconfig.xml -fo $< -ps $@

# .ps.pdf:
# 	ps2pdf $<

# PDF files rendered from Postscript files by ghostscript look better
# then those created directly by FOP.  So we're temporarly using
# ghostscript.
.fo.pdf:
	$(FOP) -d -c $(DOC_DIR)/fopconfig.xml $< $@


.fo.txt:
	$(FOP) $< $@

.dia.eps:
	$(DIA) -t eps --nosplash $<
	if test $(srcdir) != .;then mv -f $(srcdir)/$@ .; fi

.eps.png:
	convert $< $@

.eps.jpg:
	convert $< $@

.eps.tiff:
	convert $< $@

.dia.png:
	$(DIA) -t png --nosplash $<
	if test $(srcdir) != .;then mv -f $(srcdir)/$@ .; fi

clean-local:
	$(RM) $(HTMLDOCS) $(PDFDOCS) $(PSDOCS) $(PDFDOCS) $(TXTDOCS) $(EPSIMGS) $(PNGIMGS) *.chunks *.website
	$(RM) -r $(subst orocos-,,$(basename $(XMLDOCS)))
	$(RM) $(CLEANXMLDOCS)
	$(RM) $(CLEANEPS)
