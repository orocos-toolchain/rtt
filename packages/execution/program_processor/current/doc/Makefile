
dist: dochtml docpdf docps
	tar -czf orocos-program-processor-doc.tgz \
	orocos-program-processor.html orocos-program-processor.pdf \
	ProgramStates.png ContextStates.png orocos-program-processor.ps \
	Functor.png Commands.png StateTransitions.png ProgramTransitions.png

docxml: dochtml docpdf

dochtml: pngimages orocos-program-processor.html

docpdf: epsimages orocos-program-processor.pdf

docps: epsimages orocos-program-processor.ps

doctxt: orocos-program-processor.txt

epsimages: ProgramStates.eps ContextStates.eps Functor.eps Commands.eps StateTransitions.eps ProgramTransitions.eps

pngimages: ProgramStates.png ContextStates.png Functor.png Commands.png StateTransitions.png ProgramTransitions.png

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

clean-local:
	rm orocos-program-processor.html
