
dist: dochtml docpdf docps
	tar -czf orocos-os-doc.tgz \
	orocos-os.html orocos-os.pdf \
	FDIvsFOSI.png orocos-os.ps

docxml: dochtml docpdf

dochtml: pngimages orocos-os.html

docpdf: epsimages orocos-os.pdf

docps: epsimages orocos-os.ps

doctxt: orocos-os.txt

pngimages: FDIvsFOSI.png
epsimages: FDIvsFOSI.eps


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
	rm orocos-os.html
