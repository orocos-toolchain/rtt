INCLUDES=-I../include -I../../../../../build/packages-gnulinux/install/include

all: test_state

test_state.o: test_state.cpp 
	g++ -g -c $(INCLUDES) $< -o $@
test_state: test_state.o ../../../../../build/packages-gnulinux/install/lib/libtarget.a
	g++ -g test_state.o ../../../../../build/packages-gnulinux/install/lib/libtarget.a -lpthread -o $@

test_program: test_program.cpp ../../../../../build/packages-gnulinux/install/lib/libtarget.a
	g++ -g $(INCLUDES) $< ../../../../../build/packages-gnulinux/install/lib/libtarget.a -o $@
