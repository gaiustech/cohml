// serializer for Message class 

#include "coherence/io/pof/SystemPofContext.hpp"
#include "cohml.h"

using namespace coherence::io::pof;

COH_REGISTER_MANAGED_CLASS(POF_TYPE_MESSAGE, Message);

template<> void serialize<Message>(PofWriter::Handle hOut, const Message& m) {
  hOut->writeInt32(0, m.getId());
  hOut->writeInt32(1, m.getPriority());
  hOut->writeString(2, m.getSubject());
  hOut->writeString(3, m.getBody());
}

template<> Message deserialize<Message>(PofReader::Handle hIn) {
  int i = hIn->readInt32(0);
  int p = hIn->readInt32(1);
  std::string s = hIn->readString(2);
  std::string b = hIn->readString(3);

  return Message(i, p, s, b);
}

// End of file
