

docxml: dochtml docpdf

dochtml: pngimages orocos-control-kernel.html

docpdf: pngimages orocos-control-kernel.pdf

docps: epsimages orocos-control-kernel.ps

doctxt: orocos-control-kernel.txt

epsimages: kernel_component_interaction.eps component_life_cycle.eps ExtensionTimeLine.eps
pngimages: kernel_component_interaction.png component_life_cycle.png ExtensionTimeLine.png

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
	rm orocos-control-kernel.html
