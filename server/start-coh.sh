#!/bin/bash
#
# Script to start a Coherence cache server followed by a proxy server
# for C++ clients to connect to. Based on supplied example scripts.
#
# 28-JUL-2011 Gaius Initial version

# location of JDK - this is on Debian
JAVA_HOME=/usr/lib/jvm/java-6-sun

# location of Coherence Java and C++ installations
COHERENCE_HOME=/opt/coherence
COHERENCE_HOME_CPP=/opt/coherence-cpp

CLASSPATH=$CLASSPATH:$COHERENCE_HOME/lib/coherence.jar
DIR=`pwd`
mkdir -p $DIR/log

# start the cache
echo -n Starting cache... 
$JAVA_HOME/bin/java -cp $CLASSPATH -Dtangosol.coherence.log=$DIR/log/cache-server.log com.tangosol.net.DefaultCacheServer &
CACHE_PID=$!
echo done!
# give it a chance to stabilize
echo Sleeping...
sleep 1

# start the proxy
echo -n Starting proxy... 
$JAVA_HOME/bin/java -cp $CLASSPATH -Dtangosol.coherence.cacheconfig=$DIR/coherence-proxy-config.xml -Dtangosol.coherence.extend.enabled=true -Dtangosol.coherence.log=$DIR/log/cache-proxy.log com.tangosol.net.DefaultCacheServer &
PROXY_PID=$!
echo done! 

echo "kill $CACHE_PID $PROXY_PID" >stop-coh.sh
chmod ug+x stop-coh.sh

echo Wrote stop-coh.sh to kill Cache and Proxy

#echo Starting console. Enter \"bye\" to exit
#$JAVA_HOME/bin/java -cp $CLASSPATH -Dtangosol.coherence.log=$DIR/log/console.log -Dtangosol.coherence.cacheconfig=server-cache-config.xml -jar $COHERENCE_HOME/lib/coherence.jar

# End of file
