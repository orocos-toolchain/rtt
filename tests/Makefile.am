SUBDIRS = .

#EXTRA_DIST =

#global includes
INCLUDES = -I$(top_builddir)/packages/install/include $(TARGET_FLAGS)
LDADD = $(CPPUNIT_LIBS) $(top_builddir)/packages/install/lib/libtarget.a -lboost_signals -lpthread $(TARGET_LIBS)
AM_CXXFLAGS= $(CPPUNIT_CFLAGS) -g

# Rules for the test code (use `make check` to execute)
if DOTESTS
TESTS = core-test parser-test
else
TESTS = 
endif

if DOPROFTESTS
PROFTESTS=event-prof tasks-prof mtasks-prof buffers-prof \
	state-prof dispatch-prof function-prof types-prof program-prof
else
PROFTESTS =
endif

check_PROGRAMS = $(TESTS) $(PROFTESTS)
EXTRA_PROGRAMS = core-test parser-test event-prof tasks-prof mtasks-prof buffers-prof \
	state-prof dispatch-prof function-prof types-prof program-prof

#LD_ADDS= $(CPPUNIT_LIBS) $(top_builddir)/packages/install/lib/libtarget.a -lboost_signals -lpthread -L/usr/realtime/lib
#CXX_FLAGS= $(CPPUNIT_CFLAGS) -g

core_test_SOURCES = test-runner.cpp \
	event_test.cpp event_test.hpp \
	tasks_test.cpp tasks_test.hpp \
	tasks_multiple_test.cpp tasks_multiple_test.hpp \
	buffers_test.cpp buffers_test.hpp
#core_test_LDADD = $(LD_ADDS)
#core_test_CXXFLAGS = $(CXX_FLAGS)

event_prof_SOURCES = test-runner.cpp \
	event_test.cpp event_test.hpp
event_prof_CXXFLAGS = -g -pg
event_prof_LDFLAGS = -g -pg
tasks_prof_SOURCES = test-runner.cpp \
	tasks_test.cpp tasks_test.hpp
tasks_prof_CXXFLAGS = -g -pg
tasks_prof_LDFLAGS = -g -pg
mtasks_prof_SOURCES = test-runner.cpp \
	tasks_multiple_test.cpp tasks_multiple_test.hpp 
mtasks_prof_CXXFLAGS = -g -pg
mtasks_prof_LDFLAGS = -g -pg
buffers_prof_SOURCES = test-runner.cpp \
	buffers_test.cpp buffers_test.hpp
buffers_prof_CXXFLAGS = -g -pg
buffers_prof_LDFLAGS = -g -pg

state_prof_SOURCES = test-runner.cpp \
	state_test.cpp state_test.hpp
state_prof_CXXFLAGS = -g -pg
state_prof_LDFLAGS = -g -pg
dispatch_prof_SOURCES = test-runner.cpp \
	dispatch_test.cpp dispatch_test.hpp
dispatch_prof_CXXFLAGS = -g -pg
dispatch_prof_LDFLAGS = -g -pg
function_prof_SOURCES = test-runner.cpp \
	function_test.cpp function_test.hpp
function_prof_CXXFLAGS = -g -pg
function_prof_LDFLAGS = -g -pg
types_prof_SOURCES = test-runner.cpp \
	types_test.cpp types_test.hpp
types_prof_CXXFLAGS = -g -pg
types_prof_LDFLAGS = -g -pg
program_prof_SOURCES = test-runner.cpp \
	program_test.cpp program_test.hpp
program_prof_CXXFLAGS = -g -pg
program_prof_LDFLAGS = -g -pg

parser_test_SOURCES = test-runner.cpp \
	state_test.cpp state_test.hpp \
	dispatch_test.cpp dispatch_test.hpp \
	function_test.cpp function_test.hpp \
	types_test.cpp types_test.hpp \
	program_test.cpp program_test.hpp
#parser_test_LDADD = $(LD_ADDS)
#parser_test_CXXFLAGS = $(CXX_FLAGS)
