HTML_STYLESHEET=orocos-html.xsl
FOP_STYLESHEET=orocos.xsl


dist: dochtml docpdf docps
	tar -czf orocos-program-parser-doc.tgz \
	orocos-program-parser.html orocos-program-parser.pdf \
	orocos-program-parser.ps

docxml: dochtml docpdf

dochtml: pngimages orocos-program-parser.html

docpdf: epsimages orocos-program-parser.pdf

docps: epsimages orocos-program-parser.ps

doctxt: orocos-program-parser.txt

epsimages: 

pngimages: 

%.html:%.xml
	xsltproc $(HTML_STYLESHEET) $< > $@

%.fo:%.xml
	xsltproc $(FOP_STYLESHEET) $< > $@

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
	convert $< $@

clean-local:
	rm orocos-control-kernel.html
