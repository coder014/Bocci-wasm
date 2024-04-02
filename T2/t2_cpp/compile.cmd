emcc -o rule.js -O3 rule.cpp -std=c++11 -s EXPORT_ES6=1 -s MODULARIZE=1 -s EXPORT_NAME=stubLoader -s EXPORTED_FUNCTIONS="['_mancalaResult','_malloc','_free']" -s EXPORTED_RUNTIME_METHODS="['cwrap']"
