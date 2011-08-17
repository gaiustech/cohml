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
#include <vector>

using namespace coherence::lang;
using coherence::net::CacheFactory;
using coherence::net::NamedCache;
using coherence::util::MapListener;
using coherence::util::MapEvent;

#ifdef DEBUG
#define DEBUG_MSG(x) msg << __FILE__ << ":" << __func__ << ":" << __LINE__ <<": "<< x; debug(msg.str().c_str()); msg.str(""); 
#else
#define DEBUG_MSG(x) //x
#endif 

#define Cohml_val(v)   (*((Cohml**)       Data_custom_val(v)))

extern "C" {
  void debug(const char* msg);
  void raise_caml_exception(Exception::View ce);
}

#define POF_TYPE_MESSAGE 1001

class Message {
 private:
  int msg_id;
  int msg_priority;
  std::string msg_subject;
  std::string msg_body;
 public:
  Message(int i, int p, const std::string& s, const std::string& b); 
  Message(const Message& that); 
  Message(); 

  int getId() const;
  int getPriority() const;
  std::string getSubject() const;
  std::string getBody() const;
};

bool operator==(const Message& m1, const Message& m2);
std::ostream& operator<<(std::ostream& out, const Message& m);
size_t hash_value(const Message& m);

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
  void put_message(Message& m);
  const Message* get_message(int k);
  std::vector<Message*>* query_message_pri(int k);
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

#endif
// End of file
