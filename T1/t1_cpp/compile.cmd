emcc -o ghost.js -O3 ghost.cpp -s EXPORT_ES6=1 -s MODULARIZE=1 -s EXPORT_NAME=stubLoader -s EXPORTED_FUNCTIONS="['_bocciShutUp','_malloc','_free']" -s EXPORTED_RUNTIME_METHODS="['cwrap']"
