# makefile for COH*ML (suppress unnecessary warnings)

COHERENCE_HOME_CPP=/opt/coherence-cpp
CPP_INCL=-I$(COHERENCE_HOME_CPP)/include
CPP_LIB=-L$(COHERENCE_HOME_CPP)/lib

cohml:	cohml.o cohml_wrap.cxx.o swig.cmo cohml.cmo
	ocamlmktop -custom -ccopt $(CPP_LIB) -ccopt -lcoherence cohml.o cohml_wrap.cxx.o swig.cmo cohml.cmo -o $@

cohml.cmo:	cohml.cmi
	ocamlc -w -11 -c cohml.ml

cohml.cmi:	cohml_wrap.cxx.o
	ocamlc -c cohml.mli

cohml_wrap.cxx.o: cohml_wrap
	ocamlc -c -ccopt -xc++ -ccopt $(CPP_INCL) -ccopt -w cohml_wrap.cxx.c

swig.cmi:	swig.mli
	ocamlc -c $<

swig.cmo:	swig.cmi swig.ml
	ocamlc -w -20 -c swig.ml

cohml_wrap:	cohml.i
	swig -c++ -ocaml cohml.i
	cp cohml_wrap.cxx cohml_wrap.cxx.c

swig.mli:
	swig -ocaml -co $@

swig.ml:
	swig -ocaml -co $@

cohml.o:	cohml.cpp
	g++ -g3 -fPIC -c $(CPP_INCL) -I. $(CPP_LIB) -lcoherence $<

clean:
	rm -f *.o *.cmo *.cmi *.cxx.c *.ml* *_wrap.cxx cohml

# end of file