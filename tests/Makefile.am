SUBDIRS = .

#EXTRA_DIST =

INCLUDES = -I$(top_builddir)/packages/install/include

# Rules for the test code (use `make check` to execute)
TESTS = test-runner
check_PROGRAMS = $(TESTS)

test_runner_SOURCES = test-runner.cpp \
	event_test.cpp event_test.hpp \
	tasks_test.cpp tasks_test.hpp \
	tasks_multiple_test.cpp tasks_mulitple_test.hpp
test_runner_LDADD = $(CPPUNIT_LIBS) -L$(top_builddir)/packages/install/lib -ltarget -lboost_signals -lpthread
test_runner_CXXFLAGS = $(CPPUNIT_CFLAGS) -g
