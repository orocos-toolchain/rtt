HTML_STYLESHEET=orocos-html.xsl
FOP_STYLESHEET=orocos.xsl
XML_CATALOG_FILES=catalog.xml

HTMLDOCS= $(patsubst %.xml,%.html,$(XMLDOCS)) 
PDFDOCS= $(patsubst %.xml,%.pdf,$(XMLDOCS))
PSDOCS= $(patsubst %.xml,%.ps,$(XMLDOCS)) 
TXTDOCS= $(patsubst %.xml,%.txt,$(XMLDOCS))

JPGIMGS= $(patsubst %.dia,%.jpg,$(DIAS))
EPSIMGS= $(patsubst %.dia,%.eps,$(DIAS))
PNGIMGS= $(patsubst %.dia,%.png,$(DIAS))

.PHONY=doc-dist docxml dochtml docpdf docps doctxt epsimages pngimages

doc-dist: $(HTMLDOCS) $(PDFDOCS) $(PNGIMGS)
	$(MAKE) -C . dochtml docpdf
	tar -czf $(PKGNAME)-doc.tgz \
	$(HTMLDOCS) \
	$(PDFDOCS) \
	$(PNGIMGS)

docxml: dochtml docpdf

dochtml: pngimages $(HTMLDOCS)

docpdf: jpgimages $(PDFDOCS)

docps: jpgimages $(PSDOCS)

doctxt: $(TXTDOCS)

epsimages: $(EPSIMGS)

jpgimages: $(JPGIMGS)

pngimages: $(PNGIMGS)

%.html:%.xml
	XML_CATALOG_FILES=$(XML_CATALOG_FILES) \
	xsltproc -o $@ $(HTML_STYLESHEET) $<

#	saxoncat -X -o $@ $< $(HTML_STYLESHEET)
#	xsltproc -o $@ /usr/share/xml/docbook/stylesheet/nwalsh/html/docbook.xsl $<

%.fo:%.xml
	XML_CATALOG_FILES=$(XML_CATALOG_FILES) \
	xsltproc -o $@ $(FOP_STYLESHEET) $<

%.ps:%.fo
	fop $< $@

# %.pdf:%.ps
# 	ps2pdf $<

.fo.pdf:
	fop $< $@

%.txt:%.fo
	fop $< $@

%.eps:%.dia
	dia -t eps --nosplash $<

%.png:%.dia
	dia -t png --nosplash $<

%.jpg:%.png
	convert -quality 100 $< $@

doc-clean:
	rm -f $(HTMLDOCS) $(PDFDOCS) $(PSDOCS) $(PNGIMGS) $(JPGIMGS) $(EPSIMGS) $(TXTDOCS)

