
dist: dochtml docpdf
	tar -czf orocos-corelib-doc.tgz \
	execution-collaboration.png execution-sequence.png \
	io-overview.png tasks_threads-conceptual.png \
	orocos-corelib.html orocos-corelib.pdf

docxml: dochtml docpdf

dochtml: pngimages orocos-corelib.html

docpdf: pngimages orocos-corelib.pdf

docps: epsimages orocos-corelib.ps

doctxt: orocos-corelib.txt

pngimages: events.png execution-collaboration.png execution-sequence.png  io-overview.png tasks_threads-conceptual.png
epsimages: events.eps execution-collaboration.eps execution-sequence.eps  io-overview.eps tasks_threads-conceptual.eps


#orocos-control-kernel.html: 

%.html:%.xml
	xsltproc /usr/share/sgml/docbook/stylesheet/xsl/ldp/ldp-html.xsl $< > $@

%.fo:%.xml
	xsltproc /usr/share/sgml/docbook/stylesheet/xsl/nwalsh/fo/docbook.xsl $< > $@

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
	rm orocos-corelib.html
