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

XMLDOCS=orocos-overview.xml   orocos-installation.xml orocos-manual.xml

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

EPSIMGS= $(patsubst %.dia,%.eps,$(DIAS))
PNGIMGS= $(patsubst %.dia,%.png,$(DIAS))


if HAVE_XMLPROCESSOR
DOC = docxml
else
DOC = dochtml
endif


docs: $(DOC)

docxml: dochtml docpdf docps

dochtml: pngimages $(HTMLDOCS)

docpdf: epsimages $(PDFDOCS)

docps: epsimages $(PSDOCS)

doctxt: $(TXTDOCS)

pngimages: $(EPSIMGS)

epsimages: $(PNGIMGS)
	cp @ECOS_REP@/eps/*.eps .

.sgml.html:
	docbook2html -u $<

.sgml.ps:
	docbook2ps $<

.sgml.pdf:
	docbook2pdf $<

.xml.html:
	$(XMLPROCESSOR) --xinclude /usr/share/sgml/docbook/stylesheet/xsl/ldp/ldp-html.xsl $< > $@

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
	$(RM) $(HTMLDOCS) $(PDFDOCS) $(PSDOCS) $(PDFDOCS) $(TXTDOCS)
