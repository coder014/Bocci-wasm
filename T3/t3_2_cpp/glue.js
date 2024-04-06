import stubLoader from './game.js';
const wasm_module = await stubLoader();

function mancalaOperator(a, b) {
    const ptr = wasm_module._malloc(b.length << 2);
    wasm_module.HEAP32.set(b, ptr >> 2);
    return wasm_module._mancalaOperator(a, ptr);
}

export {
    mancalaOperator
}
