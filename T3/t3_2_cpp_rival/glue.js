// import stubLoader from './ghost.js';
import stubLoader from './ghost.js'
var wasm_module = await stubLoader();

function mancalaOperator(a, b) {
    var inputarr = new Uint8Array(b);
    var ret = wasm_module.ccall("mancala_operator", "number",["number", "array"],[a,inputarr]);
    return ret;
}

export {
    mancalaOperator
}