# **Qt 5.5 or earlier**

## Getting started with MSVC 2015 (Windows 8 & 10)

The MSVC 2012 and 2013 compilers don't support C++1y well enough so QttpServer is limited to  Windows 8+ with Visual Studio 2015.

Use [Qt 5.6](http://blog.qt.io/blog/2015/12/18/qt-5-6-beta-released/) for support with MSVC 2015 or continue below to build Qt from source.

The following guide below was developed and tested using Windows 10.

#### Prerequisites
1. Visual Studio 2013 express (MSVC 2013 tool-chain) - C++ tools must be activated
2. Visual Studio 2015 community (MSVC 2015 tool-chain
3. Qt 5.x (Initially configure to match with MSVC 2013)
4. [git](http://git-scm.com/)
5. [python 2.x](https://www.python.org/)
6. [strawberry perl](http://strawberryperl.com/) - As a precaution

#### Building Qt 5.5 (or less) from source
1. [Building](http://doc.qt.io/qt-5/windows-building.html) from [zip file](http://download.qt.io/official_releases/qt/5.5/5.5.0/single/) - I didn't bother with Qt's Git components - [Stackoverflow](http://stackoverflow.com/questions/32894097/how-do-i-use-qt-in-my-visual-studio-2015-projects) always helps
2. Extract to `C:\qt-5.5.0` - beware, zip is massive so don't use default windows extractor.  I used cygwin's unzip or jar to extract contents
3. Create file `c:\qt-5.5.0\qt5vars.cmd` with the contents below:

    ```batch
    REM Set up \Microsoft Visual Studio 2013, where <arch> is \c amd64, \c x86, etc.
    CALL "C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\vcvarsall.bat"
    SET _ROOT=C:\qt-5.5.0
    SET PATH=%_ROOT%\qtbase\bin;%_ROOT%\gnuwin32\bin;C:\Python27;C:\Strawberry\perl\bin;%PATH%
    REM Uncomment the below line when using a git checkout of the source repository
    REM SET PATH=%_ROOT%\qtrepotools\bin;%PATH%
    SET QMAKESPEC=win32-msvc2015
    SET _ROOT=
    ```
   I didn't specify an arch value in qt5vars.cmd since MSVC2015 is x86 anyway (Strange, I know).  Also make sure to add python and perl to the PATH.  Lastly and most importantly - be sure to update QMAKESPEC to `win32-msvc2015`
4. Launch MSVC 2015 developer console, `cd c:\qt-5.5.0\` and execute `qt5vars.cmd` to load environment variables
5. In the same MSVC developer console, execute the configuration script.  Below are configuration options that worked well on my machine against MSVC 2015:

   To include [OpenSSL](https://code.google.com/p/openssl-for-windows/downloads/list), download and extract to `C:\openssl-0.9.8k_WIN32`.  Also add the lib and bin paths to the global %PATH% environment variable or equivalent.  This is **recommended** in general and also helps support the qredis library.  Tip: If you get stuck on qtwebsockets and don't intend on using it, you may also pass in "-skip qtwebsockets"
    ```batch 
    configure.bat -platform win32-msvc2015 -debug -nomake examples -opensource -skip qtwebkit -skip qtwebchannel -skip qtquick1 -skip qtdeclarative -openssl-linked OPENSSL_LIBS="-lssleay32 -llibeay32" -I C:\openssl-0.9.8k_WIN32\include -L C:\openssl-0.9.8k_WIN32\lib -l Gdi32 -l User32
    ```

    If you really don't want OpenSSL support just run this:
    ```batch
    configure.bat -platform win32-msvc2015 -debug -nomake examples -opensource -skip qtwebkit -skip qtwebchannel -skip qtquick1 -skip qtdeclarative
    ```
6. Build with `nmake` or `jom`
7. Add the MSVC 2015 tool-chain to QtCreator:

   * Create an empty project and load it in QtCreator
   * Go to the `Projects > Manage Kits.. > Build & Run > Qt Versions > Add...` and select C:\qt-5.5.0\qtbase\bin\qmake and include somewhere in the label MSVC2015.
   * Then within `Projects > Manage Kits.. > Build & Run > Kits > Add` a new kit named "Qt 5.5.0 MSVC2015 32bit", choose the compiler that matches MSVC 2015 (Compiler v14), and lastly choose the Qt version that we recently created.

#### Finally building QttpServer

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

Launch `qttp.pro` in QtCreator, build and...

**ENJOY a beer - you've earned it!**

As a side note, if you want to run a quick sample application you can add `CONFIG+=SAMPLEAPP` to the additional arguments section located in QtCreator under `Projects > Build & Run > Build Steps > qmake > Additional arguments`