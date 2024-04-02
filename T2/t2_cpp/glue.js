import stubLoader from './rule.js';
const wasm_module = await stubLoader();

function mancalaResult(a, b, c) {
    const ptr = wasm_module._malloc(b.length << 2);
    wasm_module.HEAP32.set(b, ptr >> 2);
    return wasm_module._mancalaResult(a, ptr, c);
}

export {
    mancalaResult
}
