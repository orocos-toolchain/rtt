
dist: dochtml docpdf docps
	tar -czf orocos-motioncontrol-doc.tgz \
	orocos-motioncontrol.html \
	orocos-motioncontrol.pdf \
	orocos-motioncontrol.ps

docxml: dochtml docpdf

dochtml: pngimages orocos-motioncontrol.html

docpdf: epsimages orocos-motioncontrol.pdf

docps: epsimages orocos-motioncontrol.ps

doctxt: orocos-motioncontrol.txt

epsimages:
pngimages:

#orocos-motioncontrol.html: 

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
	rm orocos-control-kernel.html
