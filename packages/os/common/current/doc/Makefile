HTML_STYLESHEET=orocos-html.xsl
FOP_STYLESHEET=orocos.xsl


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
	imagemagick $< $@

clean-local:
	rm orocos-os.html
