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
    std::cout << "Connecting to grid... ";
    String::View vsCacheName = "mycache";
    NamedCache::Handle hCache = CacheFactory::getCache(vsCacheName);
    std::cout << "done! Cache name is " << hCache->getCacheName()  << std::endl;
    
    std::cout << "Inserting one key-value pair... ";
    String::View vsKey = "Name";
    String::View vsVal = "Gaius";
    hCache->put(vsKey, vsVal);
    std::cout << "done!" << std::endl;
    
    std::cout << "Querying the cache...";
    String::View vsGet = cast<String::View>(hCache->get(vsKey));
    std::cout << "done! Value was " << vsGet << std::endl;
  
    std::cout << "Disconnecting... ";
    CacheFactory::shutdown();
    std::cout << "done!" << std::endl;
    
    return 0;
  } catch (const std::exception& e) {
    std::cerr << "Error: " << e.what() << std::endl;
    return 1;
  }
}

// end of file
