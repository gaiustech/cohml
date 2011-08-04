#include <caml/mlvalues.h>
#include <caml/memory.h>
#include <caml/alloc.h>
#include <caml/custom.h>
#include <caml/callback.h>
#include <caml/fail.h>

#include "coherence/lang.ns"
#include "coherence/net/CacheFactory.hpp"
#include "coherence/net/NamedCache.hpp"
#include <iostream>
#include "cohml.h"

using namespace coherence::lang;
using coherence::net::CacheFactory;
using coherence::net::NamedCache;
using std::endl;
using std::cout;
using std::string;

#define Cohml_val(v)   (*((Cohml**)       Data_custom_val(v)))
#define DEBUG

// Using C to interact with OCaml
#ifdef __cplusplus 
extern "C" {
#endif

/* write a timestamped log message {C} for C code */
void debug(char* msg) {
  char datebuf[32];
  time_t t = time(NULL);
  strftime((char*)&datebuf, 31, "%a %b %e %T %Y", (gmtime(&t)));
  std::cerr << datebuf << ": " << msg <<endl;
}

/* kick an error back into OCaml-land */
void raise_caml_exception(int exception_code, char* exception_string) {
  CAMLlocal1(e);
  e = caml_alloc_tuple(2);
  Store_field(e, 0, Val_long(exception_code));
  Store_field(e, 1, caml_copy_string(exception_string));
  caml_raise_with_arg(*caml_named_value("Coh_exception"), e);
}  

#ifdef __cplusplus 
}
#endif


// Using C++ to interact with Coherence
Cohml::Cohml(char* cn) {
  String::View vsCacheName = cn;
  hCache = CacheFactory::getCache(vsCacheName);
}

void Cohml::put(char* k, char* v) {
  String::View vsKey = k; String::View vsVal = v;
  hCache->put(vsKey, vsVal);
#ifdef DEBUG
  cout << __FILE__ << ": Put key=" << vsKey << " value=" << vsVal << endl;
#endif
}
  
const char* Cohml::getCString(char* k) {
  String::View vsKey = k;
  vsRet = cast<String::View>(hCache->get(vsKey));
#ifdef DEBUG
  cout << __FILE__ << ": Get key=" << vsKey << " value=" << vsRet << endl;
#endif
  return vsRet->getCString();
}

Cohml::~Cohml() {
#ifdef DEBUG
  cout << __FILE__ << ": Disconnecting from grid" << endl;
#endif
  CacheFactory::shutdown();
}

// End of file
