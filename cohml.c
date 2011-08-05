extern "C" {
#include <caml/mlvalues.h>
#include <caml/memory.h>
#include <caml/alloc.h>
#include <caml/custom.h>
#include <caml/callback.h>
#include <caml/fail.h>
} //extern C

#include "coherence/lang.ns"
#include "coherence/net/CacheFactory.hpp"
#include "coherence/net/NamedCache.hpp"
#include "coherence/lang/Exception.hpp"
#include <iostream>
#include <sstream>
#include "cohml.h"

using namespace coherence::lang;
using coherence::net::CacheFactory;
using coherence::net::NamedCache;
using std::endl;
using std::cerr;
using std::ostringstream;

#define Cohml_val(v)   (*((Cohml**)       Data_custom_val(v)))
//#define DEBUG
#pragma GCC diagnostic ignored "-Wwrite-strings"

// Using C to interact with OCaml
extern "C" {
  /* write a timestamped log message {C} for C code */
  void debug(const char* msg) {
    char datebuf[32];
    time_t t = time(NULL);
    strftime((char*)&datebuf, 31, "%a %b %e %T %Y", (gmtime(&t)));
    cerr << datebuf << ": " << msg << endl;
  }
  
  /* kick an error back into OCaml-land */
  void raise_caml_exception(Exception::View ce) {
    CAMLlocal1(e);
    e = caml_alloc_tuple(2);
    Store_field(e, 0, caml_copy_string((ce->getName())->getCString()));
    Store_field(e, 1, caml_copy_string((ce->getMessage())->getCString()));
    caml_raise_with_arg(*caml_named_value("Coh_exception"), e);
  }

  static struct custom_operations coh_custom_ops = {"coh_custom_ops", NULL, NULL, NULL, NULL, NULL};

  // connect to Coherence and open a named cache
  value caml_coh_getcache(value cn) {
    CAMLparam1(cn);
    char* cache_name = String_val(cn);
    Cohml* c;
    try {
      c = new Cohml(cache_name);
    } catch (Exception::View ce) {
      raise_caml_exception(ce);
    }
    value v = caml_alloc_custom(&coh_custom_ops, sizeof(Cohml*), 0, 1);
    Cohml_val(v) = c;
    CAMLreturn(v);
  }

  // disconnect entirely - will need a method to do this for a single cache at a time
  value caml_coh_shutdown(value co) {
    CAMLparam1(co);
    Cohml* c = Cohml_val(co);
    delete (c);
    CAMLreturn(Val_unit);
  }

  value caml_coh_put(value co, value k, value v) {
    CAMLparam3(co, k, v);
    Cohml* c = Cohml_val(co);
    char* key = String_val(k);
    char* val = String_val(v);

    c->put(key, val);

    CAMLreturn(Val_unit);
  }

  value caml_coh_get(value co, value k) {
    CAMLparam2(co, k);
    Cohml* c = Cohml_val(co);
    char* key = String_val(k);
    
    try {
      const char* val = c->getCString(key);
      CAMLreturn(caml_copy_string(val));
    } catch (Exception::View ce) {
      raise_caml_exception(ce);
    }
    CAMLreturn(caml_copy_string("")); //should never get here
  }

} // extern C

// Using C++ to interact with Coherence
Cohml::Cohml(char* cn) {
  String::View vsCacheName = cn;
  hCache = CacheFactory::getCache(vsCacheName);
}

void Cohml::put(char* k, char* v) {
  String::View vsKey = k; String::View vsVal = v;
  hCache->put(vsKey, vsVal);
#ifdef DEBUG
  msg << __func__ << ": Put key=" << vsKey << " value=" << vsVal;
  debug(msg.str().c_str());
#endif
}
  
const char* Cohml::getCString(char* k) {
  String::View vsKey = k;
  vsRet = cast<String::View>(hCache->get(vsKey));
#ifdef DEBUG
  msg << __func__ << ": Get key=" << vsKey << " value=" << vsRet;
  debug(msg.str().c_str());
#endif
  return vsRet->getCString();
}

Cohml::~Cohml() {
#ifdef DEBUG
  msg << __func__ << ": Disconnecting from Coherence";
  debug(msg.str().c_str());
#endif  
  CacheFactory::shutdown();
}

// End of file
