## random LED effects


### compilation
```shell
rm effect2.wasm
emcc --target=wasm32 -O3 -s STANDALONE_WASM  -s EXPORTED_FUNCTIONS='["_tick"]' --js-library emscripten_compilation_hack.js -o colortrains.wasm colortrains.cpp --no-entry
```

### emulator
You should probably just use https://soulja-boy-told.me/public/lightning.html instead
Note: requires python3.10 since wasmer hasn't published a wheel for 3.11 yet

```shell
pip install wasmer wasmer_compiler_cranelift raylib
python3.10 emulator.py
```
