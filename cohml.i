%module cohml
%{
#include "coherence/lang.ns"
#include "coherence/net/CacheFactory.hpp"
#include "coherence/net/NamedCache.hpp"
#include <iostream>
#include "cohml.hpp"

using namespace coherence::lang;
using coherence::net::CacheFactory;
using coherence::net::NamedCache;
%}
class Cohml {
 public:
  Cohml(char* cn);
  void put(char* k, char* v);
  const char* getCString(char* k);
  ~Cohml();
};

// End of file
