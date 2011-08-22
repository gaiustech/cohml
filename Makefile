# Makefile for COH*ML based on Makefile for OCI*ML

COH_HOME_CPP=/opt/coherence-cpp
CCFLAGS=-ccopt -I/usr/lib/ocaml -ccopt -I$(COH_HOME_CPP)/include -ccopt -Wall
COBJS=message.o messagemaplistener.o messageserializer.o cohml.o
MLOBJS=cohml.cmo log_message.cmo

OCAML_VERSION_MAJOR = `ocamlopt -version | cut -f1 -d.`
OCAML_VERSION_MINOR = `ocamlopt -version | cut -f2 -d.`
OCAML_VERSION_POINT = `ocamlopt -version | cut -f3 -d.`


all:	cohmlsh subscriber publisher message listener

cohmlsh:	$(COBJS) $(MLOBJS) 
	ocamlmktop -g -custom -o $@ -ccopt -L$(COH_HOME_CPP)/lib -ccopt -lcoherence unix.cma $(MLOBJS) $(COBJS)
	mkdir -p log

subscriber:	$(COBJS) $(MLOBJS) subscriber.ml
	ocamlc -g -annot -custom -o $@ -cclib -L$(COH_HOME_CPP)/lib -cclib -lcoherence unix.cma $(MLOBJS) subscriber.ml $(COBJS)

publisher:	$(COBJS) $(MLOBJS) publisher.ml
	ocamlc -g -annot -custom -o $@ -cclib -L$(COH_HOME_CPP)/lib -cclib -lcoherence unix.cma $(MLOBJS) publisher.ml $(COBJS)

message:	$(COBJS) $(MLOBJS) message.ml
	ocamlc -g -custom -o $@ -cclib -L$(COH_HOME_CPP)/lib -cclib -lcoherence unix.cma $(MLOBJS) message.ml $(COBJS)


listener:	$(COBJS) $(MLOBJS) listener.ml
	ocamlc -g -annot -custom -o $@ -cclib -L$(COH_HOME_CPP)/lib -cclib -lcoherence unix.cma $(MLOBJS) listener.ml $(COBJS)

%.o:	%.c
	ocamlc -ccopt -xc++ -ccopt -g3 -c -o $@ -ccopt -I$(COH_HOME_CPP)/include -ccopt -L$(COH_HOME_CPP)/lib -ccopt -lcoherence -ccopt -DOCAML_VERSION_MINOR=$(OCAML_VERSION_MINOR) $<

%.cmo: %.ml
	ocamlc -c -g -annot unix.cma $<

clean:
	rm -f *.o *.cm* *.so *.a cohmlsh listener message subscriber publisher *~ 

# End of file