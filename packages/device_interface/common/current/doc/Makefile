
dist: dochtml docpdf docps
	tar -czf orocos-device-interface-doc.tgz \
	orocos-device-interface.html orocos-device-interface.pdf \
	fdi.png orocos-device-interface.ps

docxml: dochtml docpdf

dochtml: pngimages orocos-device-interface.html

docpdf: epsimages orocos-device-interface.pdf

docps: epsimages orocos-device-interface.ps

doctxt: orocos-device-interface.txt

pngimages: fdi.png

epsimages: fdi.eps


#orocos-control-kernel.html: 

%.html:%.xml
	xsltproc /usr/share/sgml/docbook/stylesheet/xsl/ldp/ldp-html.xsl $< > $@

%.fo:%.xml
	xsltproc orocos.xsl $< > $@

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
	rm -f orocos-device-interface.html
