SUBDIRS = .

XMLDOCS=orocos-overview.xml orocos-installation.xml \
	orocos-overview-devel.xml \
	orocos-user-manual.xml orocos-components-manual.xml orocos-devel-manual.xml \
	orocos-kdl-plugin.xml orocos-ocl-intro.xml

# the faq only renders correctly in html, the changes is only for online viewing.
EXTRAHTMLDOCS=orocos-faq.html orocos-changes.html

#need jpg to include in pdf
# EXTRAJPG= $(shell find $(ECOS_REP) -path "doc/api" -prune -o -path "*jpg" )
EXTRAJPG=$(shell ls $(srcdir)/images/*.jpg)
CLEANJPG=$(notdir $(EXTRAJPG))
#need gif to include in pdf
EXTRAGIF=$(shell ls $(srcdir)/images/*.gif)
CLEANGIF=$(notdir $(EXTRAGIF))
# no need to copy png's the html will link to them
#EXTRAPNG= $(shell find $(ECOS_REP) -path "doc/api" -prune -o -path "*png" )

PNGS= images/ConfigTool.png images/ConfigToolAdmin.png \
      images/ConfigToolPackages.png images/ConfigToolFlags.png \
      images/RealTimeToolkit.png images/ComponentInterface.png \
      images/ComponentImplementation.png images/ApplicationStack.png \
      images/fdi.png images/DataObjectBuffer.png images/TaskEventSerialisation.png \
      images/CommandExecution.png images/execution-sequence.png \
      images/ReactiveState.png images/AutomaticState.png \
      images/CoreVsContext.png images/DeploymentLevels.png \
      images/DataFlowDisconnect.png images/DataFlowTopology.png \
      images/ConnectPeers.png images/EventInterface.png \
      images/CommandInterface.png images/PropertyInterface.png \
      images/MethodInterface.png images/DataFlowInterface.png \
      images/TaskContextExecution.png images/ATaskContext.png \
      images/HelloWorld.png images/TaskPeers.png \
      images/ProgramTransitions.png images/ProgramStates.png \
      images/StateTransitions.png images/MachineStates.png \
      images/TaskCommandSerialisation.png images/Functor.png \
      images/FrameworkOverview.png

FOP_STYLESHEET := $(srcdir)/../xsl/orocos.xsl
HTML_STYLESHEET := $(srcdir)/../xsl/orocos-html.xsl

include $(srcdir)/../docrules.mak

EXTRA_DIST = \
	$(XMLDOCS) \
	$(HTMLDOCS) \
	$(PNGIMGS) \
	catalog.xml.in \
	orocos.ent

if HAVE_XMLPROCESSOR
DOC = docxml
else
DOC = dochtml
endif

.PHONY: docs

docs: prepdocs $(DOC)

all-local: prepdocs

prepdocs:
	mkdir -p images/icons
	mkdir -p images/pdf

clean-local:
	$(RM) $(HTMLDOCS) $(PDFDOCS) $(PSDOCS) $(PDFDOCS) $(TXTDOCS) $(EPSIMGS)
	$(RM) -rf images orocos-html.css
