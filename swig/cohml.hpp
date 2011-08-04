#ifndef COHML_H
#define COHML_H

#include "coherence/lang.ns"
#include "coherence/net/CacheFactory.hpp"
#include "coherence/net/NamedCache.hpp"
#include <iostream>

using namespace coherence::lang;
using coherence::net::CacheFactory;
using coherence::net::NamedCache;

class Cohml {
private:
  NamedCache::Handle hCache;
  String::View vsRet; // to keep the C string in scope
public:
  Cohml(char* cn);
  void put(char* k, char* v);
  const char* getCString(char* k);
  ~Cohml();
};

#endif
// End of file
