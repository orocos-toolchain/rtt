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

core_test_SOURCES = test-runner.cpp \
	event_test.cpp event_test.hpp \
	tasks_test.cpp tasks_test.hpp \
	tasks_multiple_test.cpp tasks_multiple_test.hpp
core_test_LDADD = $(CPPUNIT_LIBS) -L$(top_builddir)/packages/install/lib -ltarget -lboost_signals -lpthread -L/usr/realtime/lib
core_test_CXXFLAGS = $(CPPUNIT_CFLAGS) -g

parser_test_SOURCES = test-runner.cpp \
	program_test.cpp program_test.hpp
parser_test_LDADD = $(CPPUNIT_LIBS) -L$(top_builddir)/packages/install/lib -ltarget -lboost_signals -lpthread -L/usr/realtime/lib
parser_test_CXXFLAGS = $(CPPUNIT_CFLAGS) -g
