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

XMLDOCS=orocos-overview.xml   orocos-installation.xml orocos-manual.xml orocos-changes.xml orocos-faq.xml

DOCBOOKS= applications-doc.sgml  deep-shallow-api.sgml      \
	geometry-doc.sgml       motion-api.sgml \
	components-doc.sgml    interpolation-api.sgml     \
	decoupling.sgml        general-dynamics-doc.sgml  \
	kindyn-doc.sgml       robot-api.sgml 

DIAS=orocos-core.dia properties.dia \
	simple-overview.dia  heartbeat-tree.dia

HTMLDOCS= $(patsubst %.xml,%.html,$(XMLDOCS)) $(patsubst %.sgml,%.html,$(DOCBOOKS))
PDFDOCS= $(patsubst %.xml,%.pdf,$(XMLDOCS))  #$(patsubst %.sgml,%.pdf,$(DOCBOOKS))
PSDOCS= $(patsubst %.xml,%.ps,$(XMLDOCS))  #$(patsubst %.sgml,%.ps,$(DOCBOOKS))
TXTDOCS= $(patsubst %.xml,%.txt,$(XMLDOCS)) # $(patsubst %.sgml,%.txt,$(DOCBOOKS))
CHUNKSDOCS= $(patsubst %.xml,%.chunks,$(XMLDOCS)) # $(patsubst %.sgml,%.chunks,$(DOCBOOKS))

EPSIMGS= $(patsubst %.dia,%.eps,$(DIAS))
PNGIMGS= $(patsubst %.dia,%.png,$(DIAS))


if HAVE_XMLPROCESSOR
DOC = docxml
else
DOC = dochtml
endif

.PHONY: docs docxml dochtml docpdf docps doctxt pngimages epsimages docchunks

docs: $(DOC)

docxml: dochtml docpdf docps

dochtml: pngimages $(HTMLDOCS)

docpdf: epsimages $(PDFDOCS)

docps: epsimages $(PSDOCS)

doctxt: $(TXTDOCS)

docchunks: $(CHUNKSDOCS)

pngimages: $(PNGIMGS)

epsimages: $(EPSIMGS)
	cp @ECOS_REP@/eps/*.eps .

.sgml.html:
	docbook2html -u $<

.sgml.ps:
	docbook2ps $<

.sgml.pdf:
	docbook2pdf $<

.xml.html:
	$(XMLPROCESSOR) --xinclude $(srcdir)/orocos-html.xsl $< > $@

.xml.chunks:
	$(XMLPROCESSOR) --xinclude  --stringparam base.dir ./$(subst orocos-,,$(basename $(notdir $<)))/  $(srcdir)/orocos-chunk.xsl $< 
	touch $@

.xml.fo:
	$(XMLPROCESSOR) --xinclude $(srcdir)/orocos.xsl $< > $@

.fo.ps:
	$(FOP) -fo $< -ps $@

.ps.pdf:
	ps2pdf $<

# PDF files rendered from Postscript files by ghostscript look better
# then those created directly by FOP.  So we're temporarly using
# ghostscript.
#.fo.pdf:
#	$(FOP) $< $@


.fo.txt:
	$(FOP) $< $@

.dia.eps:
	$(DIA) -t eps --nosplash $<
	if test $(srcdir) != .;then mv -f $(srcdir)/$@ .; fi

.dia.png:
	$(DIA) -t png --nosplash $<
	if test $(srcdir) != .;then mv -f $(srcdir)/$@ .; fi

.eps.png:
	$(IMAGEMAGICK) $< $@

clean-local:
	$(RM) $(HTMLDOCS) $(PDFDOCS) $(PSDOCS) $(PDFDOCS) $(TXTDOCS) $(EPSIMGS) $(PNGIMGS) *.chunks
	$(RM) -r $(subst orocos-,,$(basename $(XMLDOCS)))
