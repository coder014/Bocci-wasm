import stubLoader from './ghost.js'
var wasm_module = await stubLoader();

function mancalaBoard(a, b, c) {
    const ptrb = wasm_module._malloc(b.length << 2);
    wasm_module.HEAP32.set(b, ptrb >> 2);
    var retarr = wasm_module._mancala_board(a,ptrb, c)>>2
    return wasm_module.HEAP32.slice(retarr, retarr+15);
}

export {
    mancalaBoard
}