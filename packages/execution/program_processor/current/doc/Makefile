
dist: dochtml docpdf
	tar -czf orocos-program-processor-doc.tgz \
	orocos-program-processor.html orocos-program-processor.pdf \
	PPstates.png

docxml: dochtml docpdf

dochtml: pngimages orocos-program-processor.html

docpdf: pngimages orocos-program-processor.pdf

docps: epsimages orocos-program-processor.ps

doctxt: orocos-program-processor.txt

epsimages: PPstates.eps
pngimages: PPstates.png

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
