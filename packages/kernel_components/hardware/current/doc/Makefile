HTML_STYLESHEET=orocos-html.xsl
FOP_STYLESHEET=orocos.xsl


dist: dochtml docpdf docps
	tar -czf orocos-hardware-components-doc.tgz \
	orocos-hardware-components.html \
	hardware-pattern.png \
	orocos-hardware-components.pdf

docxml: dochtml docpdf

dochtml: pngimages orocos-hardware-components.html

docpdf: epsimages orocos-hardware-components.pdf

docps: epsimages orocos-hardware-components.ps

doctxt: orocos-hardware-components.txt

pngimages: hardware-pattern.png

epsimages: hardware-pattern.eps


#orocos-control-kernel.html: 

%.html:%.xml
	xsltproc $(HTML_STYLESHEET) $< > $@

%.fo:%.xml
	xsltproc $(FOP_STYLESHEET) $< > $@

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
	fop $< $@

%.eps:%.dia
	dia -t eps --nosplash $<

%.png:%.dia
	dia -t png --nosplash $<

%.png:%.eps
	imagemagick $< $@

clean-local:
	rm orocos-hardware-components.html
