#include "coherence/lang.ns"
#include "coherence/net/CacheFactory.hpp"
#include "coherence/net/NamedCache.hpp"
#include <iostream>
#include "cohml.hpp"

using namespace coherence::lang;
using coherence::net::CacheFactory;
using coherence::net::NamedCache;
using std::endl;
using std::cout;
using std::string;

#define DEBUG

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
