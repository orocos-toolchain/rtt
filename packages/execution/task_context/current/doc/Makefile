
dist: dochtml docpdf docps
	tar -czf orocos-task-context-doc.tgz \
	orocos-task-context.html orocos-task-context.pdf \
	orocos-task-context.ps ATaskContext.png TaskPeers.png TaskFactories.png

docxml: dochtml docpdf

dochtml: pngimages orocos-task-context.html

docpdf: epsimages orocos-task-context.pdf

docps: epsimages orocos-task-context.ps

doctxt: orocos-task-context.txt

epsimages: ATaskContext.eps TaskPeers.eps TaskFactories.eps

pngimages: ATaskContext.png TaskPeers.png TaskFactories.png

%.html:%.xml
	xsltproc /usr/share/sgml/docbook/stylesheet/xsl/ldp/ldp-html.xsl $< > $@

%.fo:%.xml
	xsltproc orocos.xsl $< > $@

%.ps:%.fo epsimages
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

