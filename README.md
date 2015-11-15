# Qttp Server

<b>QttpServer</b> is a fork from [node.native](https://github.com/d5/node.native) with some additional contributions from [tojocky](https://github.com/tojocky/node.native).  Intended as an alternative to [QHttpServer](https://github.com/nikhilm/qhttpserver), this is ideal for those who want the benefits of libuv with basic Qt libraries.

### Start

```bash
git clone https://github.com/supamii/QttpServer
```

Git submodules/dependencies:
```base
git submodule update --init
```

Generate build files and compile to ./out/ folder
```base
./build.py
make -C out
```

Launch `qttp.pro` with Qt Creator

### Notes
build.py will try to download build dependencies (gyp) if missing.
If you prefer to download manually you can do:
```bash
$ git clone https://chromium.googlesource.com/external/gyp.git build/gyp
OR
$ svn co http://gyp.googlecode.com/svn/trunk build/gyp
```
by default will generate for make file. if you want to generate for a specific build tool use `-f <buildtool>`. e.x:
```bash
./build.py -f ninja
ninja -C out/Debug/
```
alternatively you can set custom paths to http-parser and libuv if you dont want to use the submodules.
If it is build with make in debug mode, then executables are saved to out/Debug dir.

