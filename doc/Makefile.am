SUBDIRS = .


EXTRA_DIST = orocos-installation.xml orocos-overview.xml orocos-core.dia properties.dia simple-overview.dia io-overview.dia \
	heartbeat-tree.dia CODING_STYLE.txt demonstrateProps.cxx \
	execution-collaboration.dia execution-sequence.dia \
    tasks_threads-conceptual.dia fdi.dia FDIvsFOSI.dia orocos-core.html  \
    execution-collaboration.png execution-sequence.png \
    tasks_threads-conceptual.png fdi.png FDIvsFOSI.png rtai-24.1.11.patch

if HAVE_XMLPROCESSOR
DOC = dochtml
else
DOC = dochtml
endif


docs: $(DOC)

docxml: dochtml docpdf

dochtml: orocos-installation.html orocos-overview.html #orocos-realtime.html pngimages  

docpdf: pngimages orocos-core.pdf orocos-overview.pdf #orocos-realtime.pdf

docps: epsimages orocos-core.ps orocos-overview.ps #orocos-realtime.ps

doctxt: orocos-core.txt orocos-core.txt #orocos-realtime.txt

epsimages: execution-collaboration.eps execution-sequence.eps tasks_threads-conceptual.eps fdi.eps FDIvsFOSI.eps
pngimages: execution-collaboration.png execution-sequence.png tasks_threads-conceptual.png fdi.png FDIvsFOSI.png

#orocos-core.html: 

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
	$(RM) orocos-core.html orocos-overview.html #orocos-realtime.html
	$(RM) -rf api
