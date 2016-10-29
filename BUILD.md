# Build QttpServer 1.0.0 beta with **gyp**

## Getting started on Mac & Linux

### Prerequisites

1. [git](http://git-scm.com/)
2. [python 2.x](https://www.python.org/)
3. [qt installer](http://www.qt.io/download/) or [build](http://doc.qt.io/qt-5/linux-building.html) from [source](http://download.qt.io/official_releases/qt/5.5/5.5.1/single/)

   ```bash
   # Building from source is a breeze - works well for Ubuntu 12 & 14 LTS
   sudo apt-get install libssl-dev
   wget http://download.qt.io/official_releases/qt/5.5/5.5.1/single/qt-everywhere-opensource-src-5.5.1.tar.gz
   tar -xvf qt-everywhere-opensource-src-5.5.1.tar.tz
   cd qt-everywhere-opensource-src-5.5.1
   ./configure -nomake examples -opensource -skip qtwebkit -skip qtwebchannel -skip qtquick1 -skip qtdeclarative -openssl-linked
   make
   sudo make install
   ```
   
4. perl

### Build

```bash
git clone https://github.com/supamii/QttpServer.git
cd QttpServer
```

Git submodules/dependencies automatically pulls in mongodb-drivers, boost, libuv, http-parser
```bash
git submodule update --init
```

Generate build files and compile to ./build/out/ folder
```bash
./build.py
make -C build/out
```

Generate and build or launch `qttp.pro` with Qt Creator
```bash
qmake CONFIG+=debug qttp.pro
make
```

If you want to run a quick sample application:
```bash
qmake CONFIG+=debug CONFIG+=SAMPLEAPP qttp.pro
make
./qttpserver
```

If you want to compile a static library:
```bash
qmake CONFIG+=debug CONFIG+=QTTP_LIBRARY qttp.pro
```

If you want to compile a shared library:
```bash
qmake CONFIG+=debug CONFIG+=QTTP_LIBRARY CONFIG+=QTTP_EXPORT qttp.pro
```

## Optional components

##### Build Redis client

Pretty straight forward stuff here - just be mindful of the extra environment variable set to DESTDIR:

```bash
cd lib/qredisclient
git submodule update --init
qmake CONFIG+=debug DESTDIR=. qredisclient.pro
make
```

## Getting started with MSVC 2015 (Windows 8 & 10)

The MSVC 2012 and 2013 compilers don't support C++1y well enough so QttpServer is limited to  Windows 8+ with Visual Studio 2015.

Using prebuilt installers for [Qt 5.6+](http://blog.qt.io/blog/2015/12/18/qt-5-6-beta-released/) is recommended but there is also an older guide for [Qt 5.5 here](./WINBUILD.md).

The following guide below was developed and tested using Windows 10.

#### Prerequisites
1. Visual Studio 2015 community (MSVC 2015 tool-chain)
2. [Qt 5.6+](http://download.qt.io/development_releases/qt/5.6/5.6.0-beta/)
3. [git](http://git-scm.com/)
4. [python 2.x](https://www.python.org/)
5. [strawberry perl](http://strawberryperl.com/) - As a precaution

#### Building QttpServer on Windows

```bash
git clone https://github.com/supamii/QttpServer
cd QttpServer
```

Git submodules/dependencies automatically pulls in mongodb-drivers, boost, libuv, http-parser
```bash
git submodule update --init
```

Use Python to generate build files and compile to ./build/out folder
```batch
build.py
```

Open solution build/all.sln with VS2015 - or execute from the developer console
```batch
cd build
msbuild.exe all.sln /p:Configuration=Debug
```

Launch `qttp.pro` in QtCreator and build.

As a side note, if you want to run a quick sample application you can add `CONFIG+=SAMPLEAPP` to the additional arguments section located in QtCreator under `Projects > Build & Run > Build Steps > qmake > Additional arguments`

Building DLLs will require additional parameters into qmake: `CONFIG+=QTTP_LIBRARY CONFIG+=QTTP_EXPORT`

If you want to compile a static library:
```bash
qmake CONFIG+=debug CONFIG+=QTTP_LIBRARY qttp.pro
```

