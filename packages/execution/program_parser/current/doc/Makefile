
dist: dochtml docpdf docps
	tar -czf orocos-program-parser-doc.tgz \
	orocos-program-parser.html orocos-program-parser.pdf \
	orocos-program-parser.ps

docxml: dochtml docpdf

dochtml: pngimages orocos-program-parser.html

docpdf: pngimages orocos-program-parser.pdf

docps: epsimages orocos-program-parser.ps

doctxt: orocos-program-parser.txt

epsimages: 
pngimages: 

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
	rm orocos-control-kernel.html
