# webasm-test

emsdk experimentation. currently outputs a newton fractal of the polynomial `z^7 - z^5 + z^3 - z^2 + z - 1`.

# compilation

install emsdk and run:
```
emcc ./main.cc -o output.js -s EXPORTED_FUNCTIONS="['_render']" -O3
```

view with running `python -m SimpleHTTPServer 3003` and navigating to `http://localhost:3003/main.html`.