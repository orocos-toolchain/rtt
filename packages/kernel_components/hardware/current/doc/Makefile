
dist: dochtml docpdf docps
	tar -czf orocos-hardware-components-doc.tgz \
	orocos-hardware-components.html \
	orocos-hardware-components.pdf

docxml: dochtml docpdf

dochtml: pngimages orocos-hardware-components.html

docpdf: epsimages orocos-hardware-components.pdf

docps: epsimages orocos-hardware-components.ps

doctxt: orocos-hardware-components.txt

pngimages:

epsimages:


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
	fop $< $@

%.eps:%.dia
	dia -t eps --nosplash $<

%.png:%.dia
	dia -t png --nosplash $<

%.png:%.eps
	imagemagick $< $@

clean-local:
	rm orocos-hardware-components.html
