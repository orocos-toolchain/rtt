
dist: dochtml docpdf docps
	tar -czf orocos-control-kernel-doc.tgz \
	$(HTMLDOCS) \
	$(PDFDOCS) \
	$(PSDOCS) \
	$(PNGIMGS)

XMLDOCS=orocos-control-kernel.xml

DIAS=kernel_component_interaction.dia component_life_cycle.dia ExtensionTimeLine.dia component_data_production.dia control-pattern.dia standard-control-kernel.dia reporting-ext.dia execution-ext.dia cascnames.dia property-ext.dia ExtensionAspect.dia KernelInterfacing.dia

HTMLDOCS= $(patsubst %.xml,%.html,$(XMLDOCS)) 
PDFDOCS= $(patsubst %.xml,%.pdf,$(XMLDOCS))
PSDOCS= $(patsubst %.xml,%.ps,$(XMLDOCS)) 
TXTDOCS= $(patsubst %.xml,%.txt,$(XMLDOCS))

EPSIMGS= $(patsubst %.dia,%.eps,$(DIAS))
PNGIMGS= $(patsubst %.dia,%.png,$(DIAS))

docxml: dochtml docpdf

dochtml: pngimages $(HTMLDOCS)

docpdf: epsimages $(PDFDOCS)

docps: epsimages $(PSDOCS)

doctxt: $(TXTDOCS)

epsimages: $(EPSIMGS)

pngimages: $(PNGIMGS)

%.html:%.xml
	xsltproc -o $@ /usr/share/sgml/docbook/stylesheet/xsl/ldp/ldp-html.xsl $<

#	saxoncat -X -o $@ $< /usr/share/sgml/docbook/stylesheet/xsl/ldp/ldp-html.xsl

#	xsltproc -o $@ /usr/share/xml/docbook/stylesheet/nwalsh/html/docbook.xsl $<

%.fo:%.xml
	xsltproc -o $@ orocos.xsl $<

%.ps:%.fo
	fop $< $@

%.pdf:%.ps
	ps2pdf $<

# PDF files rendered from Postscript files by ghostscript look better
# then those created directly by FOP.  So we're temporarly using
# ghostscript.
#.fo.pdf:
#	fop $< $@


%.txt:%.fo
	fop $< $@

%.eps:%.dia
	dia -t eps --nosplash $<

%.png:%.dia
	dia -t png --nosplash $<

%.png:%.eps
	imagemagick $< $@

clean-local:
	rm -f $(HTMLDOCS) $(PDFDOCS) $(PSDOCS) $(PNGIMGS) $(EPSIMGS) $(TXTDOCS)

