// Message class and OCaml integration - based on documentation chapter 3

extern "C" {
#include <caml/mlvalues.h>
#include <caml/memory.h>
#include <caml/alloc.h>
#include <caml/custom.h>
#include <caml/callback.h>
#include <caml/fail.h>
} //extern C

#include <vector>
#include "cohml.h"

using std::vector;

// constructors
Message::Message(int i, int p, const std::string& s, const std::string& b) : msg_id(i), msg_priority(p), msg_subject(s), msg_body(b) {}

// copy constructor required by Coherence - note tho' that there is
// no Coherence-specific code at all in this class
Message::Message(const Message& that) : msg_id(that.msg_id), msg_priority(that.msg_priority), msg_subject(that.msg_subject), msg_body(that.msg_body) {}

// zero-parameter constructor required by Coherence
Message::Message() : msg_id(0), msg_priority(0), msg_subject(""), msg_body("") {}

// date accessors - could have just made these public I guess...
// const to guarantee that these are read-only
int Message::getId() const { 
  return msg_id; 
}

int Message::getPriority() const {
  return msg_priority;
}

std::string Message::getSubject() const {
  return msg_subject;
}

std::string Message::getBody() const {
  return msg_body;
}

// operators required by Coherence (but useful anyway!)
bool operator==(const Message& m1, const Message& m2) {
  return ((m1.getSubject() == m2.getSubject()) && 
	  (m1.getBody() == m2.getBody()) && 
	  (m1.getPriority() == m2.getPriority()));
}

std::ostream& operator<<(std::ostream& out, const Message& m) {
  out << "id=" << m.getId() << ", priority=" << m.getPriority() << ", subject='" << m.getSubject() << "', body='" << m.getBody() <<"'";
  return out;
}
  
// return a unique identifier for this message - assume that these are
// generated somewhere!
size_t hash_value(const Message& m) {
  return (size_t)m.getId();
}

// OCaml interface
extern "C" {
  // create and store an object of type Message in the cache
  value caml_put_message(value co, value msg) {
    CAMLparam2(co, msg);
    Cohml* c = Cohml_val(co);

    // unmarshal the record from OCaml
    int i = Int_val(Field(msg, 0));
    int p = Int_val(Field(msg, 1));
    char* s = String_val(Field(msg, 2));
    char* b = String_val(Field(msg, 3));

    // create an unmanaged Message object on the stack
    Message m(i, p, s, b);
#ifdef DEBUG
    std::cout <<__func__ <<": " << m << std::endl;
#endif
    // send this off to the Cohml class for processing - this could have been in the Message
    // class but I wanted to keep it "pure". 
    c->put_message(m);
    
    CAMLreturn(Val_unit);
  }
  
  // fetch an object of type Message from the cache and return it to OCaml as a record
  value caml_get_message(value co, value msg_id) {
    CAMLparam2(co, msg_id);
    Cohml* c = Cohml_val(co);
    int i = Int_val(msg_id);
    CAMLlocal1(mt);

    // get an unmanaged, heap-allocated message back from Cohml
    const Message* m = c->get_message(i);

    // marshal this for OCaml
    mt = caml_alloc_tuple(4);
    Store_field(mt, 0, Val_int(m->getId()));
    Store_field(mt, 1, Val_int(m->getPriority()));
    Store_field(mt, 2, caml_copy_string((char*)m->getSubject().c_str()));
    Store_field(mt, 3, caml_copy_string((char*)m->getBody().c_str()));

    // clean up the returned object - don't need it anymore in C
    delete m;

    // return to OCaml
    CAMLreturn(mt);
  }

  // fetch all objects from the cache with a priority <= some threshold
  value caml_query_message_pri(value co, value msg_pri) {
    CAMLparam2(co, msg_pri);
    Cohml* c = Cohml_val(co);
    int p = Int_val(msg_pri);
    CAMLlocal3(msgs, mt, cons);

    // get a stl::vector containing all the matching objects back from Coherence
    vector<Message*>* msgv = c->query_message_pri(p);
    
    // alloc an OCaml list large enough for them (this bit based on oci_select.c)
    msgs = Val_emptylist;
    
    // iterate over the vector packing each one - could use an iterator but need the index anyway
    for (int i = 0; i < msgv->size(); i++) {
      mt = caml_alloc_tuple(4);
      cons = caml_alloc(2,0);
      Message* m = msgv->at(i);
      
      Store_field(mt, 0, Val_long(m->getId()));
      Store_field(mt, 1, Val_long(m->getPriority()));
      Store_field(mt, 2, caml_copy_string((char*)m->getSubject().c_str()));
      Store_field(mt, 3, caml_copy_string((char*)m->getBody().c_str()));

      Store_field(cons, 0, mt);
      Store_field(cons, 1, msgs);
      msgs = cons;
    }
    
    // clean up the vector - don't need it anymore in C
    delete msgv;
    
    // return it to OCaml
    CAMLreturn(msgs);
  }
}

// End of file
