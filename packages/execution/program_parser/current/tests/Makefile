INCLUDES=-I../include -I../../../../../build/packages/install/include

all: test_state

test_state.o: test_state.cpp 
	g++ -g -c $(INCLUDES) $< -o $@
test_state: test_state.o ../../../../../build/packages/install/lib/libtarget.a
	g++ -g test_state.o ../../../../../build/packages/install/lib/libtarget.a -lpthread -lboost_signals -o $@

test_program: test_program.cpp ../../../../../build/packages/install/lib/libtarget.a
	g++ -g $(INCLUDES) $< ../../../../../build/packages/install/lib/libtarget.a -lpthread -lboost_signals -o $@
