%module cohml
%{
#include "cohml.hpp"
%}

class Cohml {
 public:
  Cohml(char* cn);
  void put(char* k, char* v);
  const char* getCString(char* k);
  ~Cohml();
};

// End of file
