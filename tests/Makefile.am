SUBDIRS = .

#EXTRA_DIST =

INCLUDES = -I$(top_builddir)/packages/install/include

# Rules for the test code (use `make check` to execute)
if DOTESTS
TESTS = core-test parser-test
else
TESTS = 
endif

check_PROGRAMS = $(TESTS)
EXTRA_PROGRAMS = core-test parser-test

core_test_SOURCES = test-runner.cpp \
	event_test.cpp event_test.hpp \
	tasks_test.cpp tasks_test.hpp \
	tasks_multiple_test.cpp tasks_multiple_test.hpp \
	buffers_test.cpp buffers_test.hpp
core_test_LDADD = $(CPPUNIT_LIBS) $(top_builddir)/packages/install/lib/libtarget.a -lboost_signals -lpthread -L/usr/realtime/lib
core_test_CXXFLAGS = $(CPPUNIT_CFLAGS) -g

parser_test_SOURCES = test-runner.cpp \
	function_test.cpp function_test.hpp \
	types_test.cpp types_test.hpp \
	program_test.cpp program_test.hpp \
	dispatch_test.cpp dispatch_test.hpp
parser_test_LDADD = $(CPPUNIT_LIBS) $(top_builddir)/packages/install/lib/libtarget.a -lboost_signals -lpthread -L/usr/realtime/lib
parser_test_CXXFLAGS = $(CPPUNIT_CFLAGS) -g
