// OCaml includes
extern "C" {
#include <caml/mlvalues.h>
#include <caml/memory.h>
#include <caml/alloc.h>
#include <caml/custom.h>
#include <caml/callback.h>
#include <caml/fail.h>
} //extern C

// basic includes
#include "coherence/lang.ns"
#include "coherence/net/CacheFactory.hpp"
#include "coherence/net/NamedCache.hpp"
#include "coherence/lang/Exception.hpp"

// includes for MapListener
#include "coherence/util/MapListener.hpp"
#include "coherence/util/MapEvent.hpp"

// includes for queries
#include "coherence/util/extractor/PofExtractor.hpp"
#include "coherence/util/ValueExtractor.hpp"
#include "coherence/util/Filter.hpp"
#include "coherence/util/filter/LessEqualsFilter.hpp"
#include "coherence/util/filter/GreaterEqualsFilter.hpp"
#include "coherence/util/filter/EqualsFilter.hpp"
#include "coherence/util/filter/LikeFilter.hpp"
#include "coherence/util/Iterator.hpp"

#include <iostream>
#include <sstream>
#include <cstdlib>

// NOTE: if DEBUG is required, it must be #defined *before* cohml.h is #included
//#define DEBUG
#include "cohml.h"

using std::endl;
using std::cerr;
using std::ostringstream;
using std::vector;

using namespace coherence::lang;
using coherence::net::CacheFactory;
using coherence::net::NamedCache;
using coherence::util::MapListener;
using coherence::util::MapEvent;
using coherence::util::ValueExtractor;
using coherence::util::Filter;
using coherence::util::filter::LessEqualsFilter;
using coherence::util::filter::GreaterEqualsFilter;
using coherence::util::filter::LikeFilter;
using coherence::util::filter::EqualsFilter;
using coherence::util::extractor::PofExtractor;
using coherence::util::Iterator;

#pragma GCC diagnostic ignored "-Wwrite-strings"

extern "C" {
  // add callbacks for the Message type - note different cbf names from addfilterlistener
  value caml_coh_addmessagelistener(value co, value query, value cbf_ins, value cbf_upd, value cbf_del) {
    CAMLparam5(co, query, cbf_ins, cbf_upd, cbf_del);
    Cohml* c = Cohml_val(co);
    
    int f = Int_val(Field(query, 0)); // see enums in MessageMapListener class - field 0-3
    int ft = Int_val(Field(query, 1)); // field_type 0-1
    int cond = Int_val(Field(query, 2)); //condition 0-3
    int sti = 0; char* stc = NULL; // int if field type is 0, else string
    
    switch (ft) {
    case MessageMapListener::INT:
      sti = atoi(String_val(Field(query, 3)));
      break;
    case MessageMapListener::STRING:
      stc = String_val(Field(query, 3));
      break;
    }
    
    value* cbf_i = caml_named_value(String_val(cbf_ins));
    value* cbf_u = caml_named_value(String_val(cbf_upd));
    value* cbf_d = caml_named_value(String_val(cbf_del));

    if ( (cbf_i == NULL) || (cbf_u == NULL) || (cbf_d == NULL)) {
      caml_raise_with_arg(*caml_named_value("Cohml_exception"), caml_copy_string("Cannot listen: callbacks not defined!"));
    } else {
      c->addMessageListener(f, ft, cond, sti, stc , cbf_i, cbf_u, cbf_d);  
    }
    
    CAMLreturn(Val_unit);
  }
}

// call the OCaml callback function with an int and a 4-tuple from message_to_tuple
void MessageMapListener::entryInserted(MapEvent::View vEvent) {
  // get the Message, allocate some OCaml storage, convert it and return that
  CAMLlocal1(mt);
  mt = caml_alloc_tuple(4);
  Managed<Message>::View vm = cast<Managed<Message>::View>(vEvent->getNewValue());
  Message* m;
  m = new Message(vm->getId(), vm->getPriority(), vm->getSubject(), vm->getBody());
  message_to_tuple(m, mt);
  int k = (cast<Integer32::View>(vEvent->getKey()))->getInt32Value();
  
  caml_callback2(*cbf_insert, Val_int(k), mt);
}

// call the OCaml function cbf_coh_update with the key, the previous value and the new value
void MessageMapListener::entryUpdated(MapEvent::View vEvent) {
  CAMLlocal2(omt, nmt);
  omt = caml_alloc_tuple(4);
  nmt = caml_alloc_tuple(4);
  
  Managed<Message>::View vm;
  Message* m;

  // old message
  vm = cast<Managed<Message>::View>(vEvent->getOldValue());
  m = new Message(vm->getId(), vm->getPriority(), vm->getSubject(), vm->getBody());
  message_to_tuple(m, omt);
  
  // new message
vm = cast<Managed<Message>::View>(vEvent->getNewValue());
  m = new Message(vm->getId(), vm->getPriority(), vm->getSubject(), vm->getBody());
  message_to_tuple(m, nmt);
  
  int k = (cast<Integer32::View>(vEvent->getKey()))->getInt32Value();

  caml_callback3(*cbf_update, k, omt, nmt);
}

// call the OCaml function cb_coh_delete with the key just removed from the cache
void MessageMapListener::entryDeleted(MapEvent::View vEvent) {
  caml_callback(*cbf_delete, Val_int((cast<Integer32::View>(vEvent->getKey()))->getInt32Value()));
}

// add a listener for the message type - see caml_coh_addmessagelistener()
void Cohml::addMessageListener(int f, int ft, int cond, int sti, char* stc, value* cbf_i, value* cbf_u, value* cbf_d) {
  CAMLlocal1(mt);
  TypedHandle<MessageMapListener> mml = MessageMapListener::create();
  mml->cbf_insert = cbf_i; 
  mml->cbf_update = cbf_u; 
  mml->cbf_delete = cbf_d;
  
  // from field, field_type, condition, and search term (int or string), 
  // construct a Filter using a PofExtractor

  // existing records in the cache will be processed using the insert callback
  // (or should they be discarded? a philosophical question)

  // this should be adding it to the continuous query, not to the cache handle
  hCache->addFilterListener(mml);
  DEBUG_MSG("listening");
}

// end of file 
