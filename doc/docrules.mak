#only set these if not set before
HTML_STYLESHEET   ?= orocos-html.xsl
FOP_STYLESHEET    ?= orocos.xsl
XML_CATALOG_FILES ?= catalog.xml
#XSL_PDF_PARAMS also available.
#XSL_HTML_PARAMS also available.

XMLPROCESSOR      ?= xsltproc
DIA               ?= dia
FOP               ?= fop
CONVERT           ?= convert
JPGQUALITY        ?= 30
PNGS              ?=
#
# Rules and stuff below
#


HTMLDOCS= $(patsubst %.xml,%.html,$(XMLDOCS)) $(patsubst %.sgml,%.html,$(DOCBOOKS)) $(EXTRAHTMLDOCS)
PDFDOCS= $(patsubst %.xml,%.pdf,$(XMLDOCS))
PSDOCS= $(patsubst %.xml,%.ps,$(XMLDOCS)) 
TXTDOCS= $(patsubst %.xml,%.txt,$(XMLDOCS))

# convert pngs only only to jpg, gif reduces too much quality of screenshots.
JPGIMGS= $(patsubst %.png,%.jpg,$(PNGS))
EPSIMGS= $(patsubst %.dia,%.eps,$(DIAS))
PNGIMGS= $(patsubst %.dia,%.png,$(DIAS)) $(patsubst %.png,%.png,$(PNGS))
TIFFIMGS=$(patsubst %.dia,%.tiff,$(DIAS))
GIFIMGS=$(patsubst %.dia,%.gif,$(DIAS))

.PHONY=doc-dist docxml dochtml docpdf docps doctxt epsimages pngimages

doc-dist: $(PKGNAME)-doc.tgz

$(PKGNAME)-doc.tgz: $(XMLDOCS) $(DIAS)
	$(MAKE) -C . dochtml docpdf
	tar -czf $(PKGNAME)-doc.tgz \
	$(HTMLDOCS) \
	$(PDFDOCS) \
	$(PNGIMGS)

docxml: dochtml docpdf

dochtml: pngimages $(XMLDOCS)
	${MAKE} $(HTMLDOCS)

docpdf: gifimages jpgimages $(XMLDOCS)
	${MAKE} $(PDFDOCS)

docps: jpgimages $(XMLDOCS)
	${MAKE} $(PSDOCS)

doctxt: $(TXTDOCS)

epsimages: $(EPSIMGS)

jpgimages: $(JPGIMGS)

pngimages: $(PNGIMGS)

tiffimages: $(TIFFIMGS)

gifimages: $(GIFIMGS)

.sgml.html:
	docbook2html -u $<

.sgml.ps:
	docbook2ps $<

.sgml.pdf:
	docbook2pdf $<

%.html:%.xml
	XML_CATALOG_FILES=$(XML_CATALOG_FILES) \
	$(XMLPROCESSOR) $(XSL_HTML_PARAMS) --xinclude $(HTML_STYLESHEET) $< > $@

# %.html:%.xml
# 	XML_CATALOG_FILES=$(XML_CATALOG_FILES) \
# 	xsltproc -o $@ $(HTML_STYLESHEET) $<

#	saxoncat -X -o $@ $< $(HTML_STYLESHEET)
#	xsltproc -o $@ /usr/share/xml/docbook/stylesheet/nwalsh/html/docbook.xsl $<

%.fo:%.xml
	XML_CATALOG_FILES=$(XML_CATALOG_FILES) \
	$(XMLPROCESSOR) $(XSL_PDF_PARAMS) --xinclude $(FOP_STYLESHEET) $< > $@

%.ps:%.fo
	$(FOP) $< $@

%.pdf:%.fo
	$(FOP) $< $@

%.txt:%.fo
	$(FOP) $< $@

#if $(srcdir) is set and points elsewhere, mv file from there to here.
%.eps:%.dia
	$(DIA) -t eps --nosplash $<
	if test x$(srcdir) != x -a ! $(srcdir)/. -ef . ;then mv -f $(srcdir)/$@ .; fi

%.png:%.dia
	$(DIA) -t png --nosplash $<
	if test x$(srcdir) != x -a ! $(srcdir)/. -ef . ;then mv -f $(srcdir)/$@ .; fi

%.jpg:%.png
	$(CONVERT) -quality $(JPGQUALITY) $< $@

%.tiff:%.png
	$(CONVERT) $< $@

%.gif:%.png
	$(CONVERT) $< $@

doc-clean:
	rm -f $(HTMLDOCS) $(PDFDOCS) $(PSDOCS) $(PNGIMGS) $(JPGIMGS) $(EPSIMGS) $(GIFIMGS) $(TXTDOCS)

