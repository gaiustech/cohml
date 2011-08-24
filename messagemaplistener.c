// OCaml includes
extern "C" {
#include <caml/mlvalues.h>
#include <caml/memory.h>
#include <caml/alloc.h>
#include <caml/custom.h>
#include <caml/callback.h>
#include <caml/fail.h>

#if OCAML_VERSION_MINOR >= 12
#include <caml/threads.h>
#else
#include <caml/signals.h>
#endif 

/* from threads.h in 3.12 only */
#ifndef caml_acquire_runtime_system
#define caml_acquire_runtime_system caml_leave_blocking_section
#define caml_release_runtime_system caml_enter_blocking_section
#endif
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

#include "coherence/net/cache/ContinuousQueryCache.hpp"

#include <iostream>
#include <sstream>
#include <cstdlib>

// NOTE: if DEBUG is required, it must be #defined *before* cohml.h is #included
#define DEBUG
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
using coherence::net::cache::ContinuousQueryCache;

#pragma GCC diagnostic ignored "-Wwrite-strings"

extern "C" {
  // add callbacks for the Message type - note different cbf names from addfilterlistener
  value caml_coh_addmessagelistener(value co, value query, value cbf_ins, value cbf_upd, value cbf_del) {
    CAMLparam5(co, query, cbf_ins, cbf_upd, cbf_del);
    Cohml* c = Cohml_val(co);
    
    int f = Int_val(Field(query, 0)); // see enums in MessageMapListener class - field 0-3
    int ft = Int_val(Field(query, 1)); // field_type 0-1
    int cond = Int_val(Field(query, 2)); //condition 0-3
    bool exist = Bool_val(Field(query, 4));
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
      c->addMessageListener(f, ft, cond, sti, stc , exist, cbf_i, cbf_u, cbf_d);  
    }
    
    CAMLreturn(Val_unit);
  }
}

// call the OCaml callback function with an int and a 4-tuple from message_to_tuple
void MessageMapListener::entryInserted(MapEvent::View vEvent) {
  DEBUG_MSG("entered, cache state=" << hCQC->getState() << " active=" << hCQC->isActive());
  // get the Message, allocate some OCaml storage, convert it and return that
  CAMLlocal1(mt);
  
  mt = caml_alloc_tuple(4);
  Managed<Message>::View vm = cast<Managed<Message>::View>(vEvent->getNewValue());
  Message* m;
  m = new Message(vm->getId(), vm->getPriority(), vm->getSubject(), vm->getBody());
  message_to_tuple(m, mt);
  int k = (cast<Integer32::View>(vEvent->getKey()))->getInt32Value();

  caml_acquire_runtime_system();
  caml_callback2(*cbf_insert, Val_int(k), mt);
  caml_release_runtime_system();

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
  caml_acquire_runtime_system();
  caml_callback3(*cbf_update, k, omt, nmt);
  caml_release_runtime_system();
}

// call the OCaml function cb_coh_delete with the key just removed from the cache
void MessageMapListener::entryDeleted(MapEvent::View vEvent) {
  caml_acquire_runtime_system();
  caml_callback(*cbf_delete, Val_int((cast<Integer32::View>(vEvent->getKey()))->getInt32Value()));
  caml_release_runtime_system();
}

// add a listener for the message type - see caml_coh_addmessagelistener()
void Cohml::addMessageListener(int f, int ft, int cond, int sti, char* stc, bool exist, value* cbf_i, value* cbf_u, value* cbf_d) {
  CAMLlocal1(mt);
  TypedHandle<MessageMapListener> mml = MessageMapListener::create();
  mml->cbf_insert = cbf_i; 
  mml->cbf_update = cbf_u; 
  mml->cbf_delete = cbf_d;
  
  // from field, field_type, condition, and search term (int or string), 
  // construct a Filter using a PofExtractor
  ValueExtractor::Handle hEx;
  
  switch (ft) {
  case MessageMapListener::INT:
    hEx = PofExtractor::create(typeid(int32_t), f);
    DEBUG_MSG("PofExtractor of type int32");
    break;
  case MessageMapListener::STRING:
    hEx = PofExtractor::create(typeid(void), f);
    break;
  }

  // there must be a better way to do this, if I could convert from Comparable to String/Integer32 via virtual base...
  switch (cond) {
  case MessageMapListener::LESS_THAN:
    switch (ft) {
    case MessageMapListener::INT:
      hFil = LessEqualsFilter::create(hEx, Integer32::valueOf(sti));
      DEBUG_MSG("LESS_THAN filter on f=" << f <<" sti=" << sti);
      break;
    case MessageMapListener::STRING:
      caml_raise_with_arg(*caml_named_value("Cohml_exception"), caml_copy_string("Cannot use less than comparator with strings"));
      break;
    }
    break;
  case MessageMapListener::EQUAL_TO:
    switch (ft) {
    case MessageMapListener::INT:
      hFil = EqualsFilter::create(hEx, Integer32::valueOf(sti));
      break;
    case MessageMapListener::STRING:
      hFil = EqualsFilter::create(hEx, String::create(stc));
      break;
    }
    break;
  case MessageMapListener::GREATER_THAN:
    switch (ft) {
    case MessageMapListener::INT:
      hFil = GreaterEqualsFilter::create(hEx, Integer32::valueOf(sti));
      break;
    case MessageMapListener::STRING:
      caml_raise_with_arg(*caml_named_value("Cohml_exception"), caml_copy_string("Cannot use greater than comparator with strings"));
      break;
    }
    break;
  case MessageMapListener::LIKE:
    switch (ft) {
    case MessageMapListener::INT:
      caml_raise_with_arg(*caml_named_value("Cohml_exception"), caml_copy_string("Cannot use like comparator with integers"));
      break;
    case MessageMapListener::STRING:
      hFil = LikeFilter::create(hEx, String::create(stc));
      break;
    }
    break;
  }

  // existing records in the cache will be processed using the insert callback
  // (or should they be discarded? a philosophical question)
  if (exist) {
      hCQC = ContinuousQueryCache::create(hCache, hFil, false, mml);
  } else {
    hCQC = ContinuousQueryCache::create(hCache, hFil);
    DEBUG_MSG("Clearing CQ cache on " << hCache->getCacheName());
    hCache->clear();
    hCQC->clear();
    hCQC->addFilterListener(mml);
  }

  mml->hCQC = hCQC;
  
  hCQC->setReconnectInterval(20); // milliseconds
  DEBUG_MSG("reconnect interval=" << hCQC->getReconnectInterval() << "ms");

  hCQC->setCacheValues(false);
  DEBUG_MSG("local cache=" << hCQC->isCacheValues());
  DEBUG_MSG("CQ listening");
}

// end of file 
