
dist: dochtml docpdf docps
	tar -czf orocos-control-kernel-doc.tgz \
	orocos-control-kernel.html kernel_component_interaction.png control-pattern.png \
	component_life_cycle.png ExtensionTimeLine.png component_data_production.png \
	control-pattern.png standard-control-kernel.png \
	orocos-control-kernel.pdf \
	orocos-control-kernel.ps

docxml: dochtml docpdf

dochtml: pngimages orocos-control-kernel.html

docpdf: epsimages orocos-control-kernel.pdf

docps: epsimages orocos-control-kernel.ps

doctxt: orocos-control-kernel.txt

epsimages: kernel_component_interaction.eps component_life_cycle.eps ExtensionTimeLine.eps component_data_production.eps control-pattern.eps standard-control-kernel.eps

pngimages: kernel_component_interaction.png component_life_cycle.png ExtensionTimeLine.png component_data_production.png control-pattern.png standard-control-kernel.png

#orocos-control-kernel.html: 

%.html:%.xml
	xsltproc -o $@ /usr/share/sgml/docbook/stylesheet/xsl/ldp/ldp-html.xsl $<

%.fo:%.xml
	xsltproc -o $@ orocos.xsl $<
	cp $@ $@.bak

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
