#ifndef COHML_H
#define COHML_H

extern "C" {
#include <caml/mlvalues.h>
}

#include "coherence/lang.ns"
#include "coherence/net/CacheFactory.hpp"
#include "coherence/net/NamedCache.hpp"
#include "coherence/util/MapListener.hpp"
#include "coherence/util/MapEvent.hpp"
#include <iostream>

using namespace coherence::lang;
using coherence::net::CacheFactory;
using coherence::net::NamedCache;
using coherence::util::MapListener;
using coherence::util::MapEvent;

class Cohml {
private:
  NamedCache::Handle hCache;
  String::View vsRet; // to keep the C string in scope
  std::ostringstream msg; // debug messages
public:
  Cohml(char* cn);
  void put(char* k, char* v);
  void remove(char* k);
  const char* getCString(char* k);
  void addFilterListener();
  ~Cohml();
};

class CohmlMapListener:public class_spec<CohmlMapListener, extends<Object>, implements<MapListener> > {
  friend class factory<CohmlMapListener>;
public:
  virtual void entryInserted(MapEvent::View vEvent);
  virtual void entryUpdated(MapEvent::View vEvent);
  virtual void entryDeleted(MapEvent::View vEvent);
};

#endif
// End of file
