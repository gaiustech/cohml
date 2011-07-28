# Makefile for COH*ML example

COHERENCE_HOME_CPP=/opt/coherence-cpp
CPP_OPTS=-g3 -Wall -ansi 
CPP_INCL=-I$(COHERENCE_HOME_CPP)/include -I.
CPP_LIB=-L$(COHERENCE_HOME_CPP)/lib

cpptest:	cpptest.cpp
	g++ $(CPP_OPTS) $< -o $@ $(CPP_INCL) $(CPP_LIB) -lcoherence

# end of file