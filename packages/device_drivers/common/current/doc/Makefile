
dist: dochtml docpdf docps
	tar -czf orocos-device-drivers-doc.tgz \
	orocos-device-drivers.html orocos-device-drivers.pdf \
	orocos-device-drivers.ps

docxml: dochtml docpdf

dochtml: pngimages orocos-device-drivers.html

docpdf: epsimages orocos-device-drivers.pdf

docps: epsimages orocos-device-drivers.ps

doctxt: orocos-device-drivers.txt

pngimages: 
epsimages: 


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
	rm -f orocos-device-drivers.html orocos-device-drivers.pdf
