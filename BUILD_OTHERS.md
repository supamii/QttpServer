# Optional components

## Build Redis client

Pretty straight forward stuff here - just be mindful of the extra environment variable set to DESTDIR:

```bash
cd lib/qredisclient
git submodule update --init
qmake CONFIG+=debug DESTDIR=. qredisclient.pro
make
```

## Build MongoDb driver

1. Install [scons](http://www.scons.org/) - e.g. `brew install scons` or `sudo apt-get install scons`
2. Install [boost](https://github.com/mongodb/mongo-cxx-driver/wiki/Download-and-Compile-the-Legacy-Driver)

   Boost may be challenging to build so it's highly recommended that you use something like homebrew, apt-get, or the pre-built [binary installer for windows](http://sourceforge.net/projects/boost/files/boost-binaries/)

    ```bash
    # Install what you can with Ubuntu
    sudo apt-get install mongodb-dev
    sudo apt-get install libboost1.54-all-dev
    ```

    ```bash
    # Brew on macx
    brew search boost
    brew info boost
    brew install boost --c++11
    ```

   *NOTE:* If you're installing mongo locally, make sure you link to the same set of BOOST libraries!

    ```bash
    brew install mongodb --with-boost --with-openssl
    ```
4. Build the driver!

   Ubuntu

    ```bash
    cd QttpServer/lib/mongo-cxx-driver
    sudo scons --libpath=/usr/lib/x86_64-linux-gnu --cpppath=/usr/include --dbg=on --opt=on --64 install
    ```

   Windows

    ```batch
    set PATH=C:\Python27;C:Pyathon27\Scripts;%PATH%
    scons --32 --dbg=on --opt=off --sharedclient --dynamic-windows --prefix="C:\local\mongo-client" --cpppath="C:\local\boost_1_59_0" --libpath="C:\local\boost_1_59_0\lib32-msvc-14.0" install
    ```

   Mac

    ```bash
    scons --libpath=/usr/local/lib --cpppath=/usr/local/include --dbg=on --opt=on --64 install
    ```

For more information visit [mongodb.org](https://docs.mongodb.org/getting-started/cpp/client/)
