SUBDIRS = .


EXTRA_DIST = orocos-installation.xml orocos-overview.xml orocos-core.dia properties.dia simple-overview.dia io-overview.dia \
	heartbeat-tree.dia CODING_STYLE.txt demonstrateProps.cxx \
	orocos-overview.html orocos-installation.html  \
        rtai-24.1.11.patch

if HAVE_XMLPROCESSOR
DOC = dochtml
else
DOC = dochtml
endif


docs: $(DOC)

docxml: dochtml docpdf

dochtml: orocos-installation.html orocos-overview.html

docpdf: pngimages orocos-installation.pdf orocos-overview.pdf

docps: epsimages orocos-installation.ps orocos-overview.ps

doctxt: orocos-installation.txt orocos-overview.txt

.xml.html:
	$(XMLPROCESSOR) /usr/share/sgml/docbook/stylesheet/xsl/nwalsh/html/docbook.xsl $< > $@

.xml.fo:
	$(XMLPROCESSOR) /usr/share/sgml/docbook/stylesheet/xsl/nwalsh/fo/docbook.xsl $< > $@

.fo.ps:
	$(FOP) $< $@

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
	$(RM) orocos-installation.html orocos-overview.html
	$(RM) -rf api
