SUBDIRS = .


EXTRA_DIST = \
	CODING_STYLE.txt demonstrateProps.cxx \
	orocos.xsl \
        rtai-24.1.11.patch \
	$(XMLDOCS) \
	$(DIAS) \
	$(HTMLDOCS) \
	$(PDFDOCS)
# we do not distribute the ps files, they are to large,
# but i make them for the website

XMLDOCS=orocos-overview.xml   orocos-installation.xml
	applications-doc.xml  deep-shallow-api.xml      \
	geometry-doc.xml       motion-api.xml \
	components-doc.xml    interpolation-api.xml     \
	decoupling.xml        general-dynamics-doc.xml  \
	kindyn-doc.xml       robot-api.xml 

DIAS=orocos-core.dia properties.dia \
	simple-overview.dia  heartbeat-tree.dia

HTMLDOCS= $(patsubst %.xml,%.html,$(XMLDOCS))
PDFDOCS= $(patsubst %.xml,%.pdf,$(XMLDOCS))
PSDOCS= $(patsubst %.xml,%.ps,$(XMLDOCS))
TXTDOCS= $(patsubst %.xml,%.txt,$(XMLDOCS))

EPSIMGS= $(patsubst %.dia,%.eps,$(DIAS))
PNGIMGS= $(patsubst %.dia,%.png,$(DIAS))


if HAVE_XMLPROCESSOR
DOC = docxml
else
DOC = dochtml
endif


docs: $(DOC)

docxml: dochtml docpdf docps

dochtml: $(HTMLDOCS)

docpdf: pngimages $(PDFDOCS)

docps: epsimages $(PSDOCS)

doctxt: $(TXTDOCS)

pngimages: $(EPSIMGS)

epsimages: $(PNGIMGS)

.xml.html:
	$(XMLPROCESSOR)  /usr/share/sgml/docbook/stylesheet/xsl/ldp/ldp-html.xsl $< > $@

.xml.fo:
	$(XMLPROCESSOR) $(srcdir)/orocos.xsl $< > $@

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
