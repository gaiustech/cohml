// Message class and OCaml integration - based on documentation chapter 3

extern "C" {
#include <caml/mlvalues.h>
#include <caml/memory.h>
#include <caml/alloc.h>
#include <caml/custom.h>
#include <caml/callback.h>
#include <caml/fail.h>
} //extern C

#include "cohml.h"

// constructors
Message::Message(int i, int p, const std::string& s, const std::string& b) : msg_id(i), msg_priority(p), msg_subject(s), msg_body(b) {}

// copy constructor required by Coherence - note tho' that there is
// no Coherence-specific code in this file
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

}

// End of file
