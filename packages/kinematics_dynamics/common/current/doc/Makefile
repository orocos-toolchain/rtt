
dist: dochtml docpdf docps
	tar -czf orocos-kinematics-doc.tgz \
	orocos-kinematics.html \
	orocos-kinematics.pdf \
	orocos-kinematics.ps

docxml: dochtml docpdf

dochtml: pngimages orocos-kinematics.html

docpdf: epsimages orocos-kinematics.pdf

docps: epsimages orocos-kinematics.ps

doctxt: orocos-kinematics.txt

epsimages:
pngimages:

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
	rm orocos-control-kernel.html
