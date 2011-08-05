# Makefile for COH*ML based on Makefile for OCI*ML

COH_HOME_CPP=/opt/coherence-cpp
CCFLAGS=-ccopt -I/usr/lib/ocaml -ccopt -I$(COH_HOME_CPP)/include -ccopt -Wall
COBJS=cohml.o
MLOBJS=cohml.cmo

cohmlsh:	$(COBJS) $(MLOBJS)
	ocamlmktop -g -custom -o $@ -ccopt -L$(COH_HOME_CPP)/lib -ccopt -lcoherence unix.cma $(MLOBJS) $(COBJS)
	mkdir -p log

%.o:	%.c %.h
	ocamlc -ccopt -xc++ -ccopt -g3 -c -o $@ -ccopt -I$(COH_HOME_CPP)/include -ccopt -L$(COH_HOME_CPP)/lib -ccopt -lcoherence $<

%.cmo: %.ml
	ocamlc -c -g unix.cma $<

clean:
	rm -f *.o *.cm* *.so *.a cohml

# End of file