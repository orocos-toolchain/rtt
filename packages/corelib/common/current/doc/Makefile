
dist: dochtml docpdf docps
	tar -czf orocos-corelib-doc.tgz \
	execution-collaboration.png execution-sequence.png \
	io-overview.png tasks_threads-conceptual.png \
	orocos-corelib.html orocos-corelib.pdf orocos-corelib.ps

docxml: dochtml docpdf

dochtml: pngimages orocos-corelib.html

docpdf: epsimages orocos-corelib.pdf

docps: epsimages orocos-corelib.ps

doctxt: orocos-corelib.txt

pngimages: execution-collaboration.png execution-sequence.png  io-overview.png tasks_threads-conceptual.png

epsimages: execution-collaboration.eps execution-sequence.eps  io-overview.eps tasks_threads-conceptual.eps


#orocos-control-kernel.html: 

%.html:%.xml
	xsltproc /usr/share/sgml/docbook/stylesheet/xsl/ldp/ldp-html.xsl $< > $@

%.fo:%.xml
	xsltproc orocos.xsl $< > $@

%.ps:%.fo
	fop -fo $< -ps $@

%.pdf:%.ps
	ps2pdf $<

# PDF files rendered from Postscript files by ghostscript look better
# then those created directly by FOP.  So we're temporarly using
# ghostscript.
#.fo.pdf:
#	fop $< $@


%.txt:%.fo
	fop $< -txt $@

%.eps:%.dia
	dia -t eps --nosplash $<

%.png:%.dia
	dia -t png --nosplash $<

%.png:%.eps
	imagemagick $< $@

clean-local:
	rm orocos-corelib.html
