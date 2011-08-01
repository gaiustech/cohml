// test connectivity from C++ client to Coherence via Proxy - based on example hellogrid.cpp

#include "coherence/lang.ns"
#include "coherence/net/CacheFactory.hpp"
#include "coherence/net/NamedCache.hpp"
#include <iostream>

using namespace coherence::lang;
using coherence::net::CacheFactory;
using coherence::net::NamedCache;

int main(int argc, char** argv) {
  try {
    String::View vsCacheName = "mycache";
    NamedCache::Handle hCache = CacheFactory::getCache(vsCacheName);
    
    String::View vsKey = "If you can see this, ";
    String::View vsVal = "your environment is basically sane.";
    hCache->put(vsKey, vsVal);
    
    String::View vsGet = cast<String::View>(hCache->get(vsKey));
    std::cout << "*** " << vsKey << vsGet << " ***" << std::endl;
  
    CacheFactory::shutdown();
    
    return 0;
  } catch (const std::exception& e) {
    std::cerr << "Error: " << e.what() << std::endl;
    return 1;
  }
}

// end of file
