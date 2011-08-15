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

extern "C" {
  void debug(const char* msg);
  void raise_caml_exception(Exception::View ce);
}

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
  void addFilterListener(value* cbf_i, value* cbf_u, value* cbf_d);
  ~Cohml();
};

class CohmlMapListener:public class_spec<CohmlMapListener, extends<Object>, implements<MapListener> > {
  friend class factory<CohmlMapListener>;
public:
  value* cbf_insert;
  value* cbf_update;
  value* cbf_delete;
  virtual void entryInserted(MapEvent::View vEvent);
  virtual void entryUpdated(MapEvent::View vEvent);
  virtual void entryDeleted(MapEvent::View vEvent);
};

class Message {
 private:
  const int msg_id;
  const int msg_priority;
  const std::string msg_subject;
  const std::string msg_body;
 public:
  Message(int i, int p, const std::string& s, const std::string& b); 
  Message(const Message& that); 
  Message(); 

  int getId() const;
  int getPriority() const;
  std::string getSubject() const;
  std::string getBody() const;
};

#endif
// End of file
