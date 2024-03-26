import stubLoader from './ghost.js';
const wasm_module = await stubLoader();

function bocchiShutUp(a, b, c) {
    const ptr = wasm_module._malloc(b.length << 2);
    wasm_module.HEAP32.set(b, ptr >> 2);
    return wasm_module._bocciShutUp(a, ptr, c);
}

export {
    bocchiShutUp
}