

docxml: dochtml docpdf

dochtml: pngimages orocos-control-kernel.html

docpdf: pngimages orocos-control-kernel.pdf

docps: epsimages orocos-control-kernel.ps

doctxt: orocos-control-kernel.txt

epsimages: kernel_component_interaction.eps component_life_cycle.eps ExtensionTimeLine.eps
pngimages: kernel_component_interaction.png component_life_cycle.png ExtensionTimeLine.png

#orocos-control-kernel.html: 

.xml.html: 
	xsltproc /usr/share/sgml/docbook/stylesheet/xsl/ldp/ldp-html.xsl $< > $@

.xml.fo:
	xsltproc /usr/share/sgml/docbook/stylesheet/xsl/nwalsh/fo/docbook.xsl $< > $@

.fo.ps:
	fop $< $@

.ps.pdf:
	ps2pdf $<

# PDF files rendered from Postscript files by ghostscript look better
# then those created directly by FOP.  So we're temporarly using
# ghostscript.
#.fo.pdf:
#	fop $< $@


.fo.txt:
	fop $< $@

.dia.eps:
	dia -t eps --nosplash $<

.dia.png:
	dia -t png --nosplash $<

.eps.png:
	imagemagick $< $@

clean-local:
	rm orocos-control-kernel.html
