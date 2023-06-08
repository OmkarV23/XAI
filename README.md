# XAI

## Installing MongoDB C drivers
```
$ wget https://github.com/mongodb/mongo-c-driver/releases/download/1.23.5/mongo-c-driver-1.23.5.tar.gz
$ tar xzf mongo-c-driver-1.23.5.tar.gz
$ cd mongo-c-driver-1.23.5
$ mkdir cmake-build
$ cd cmake-build
$ cmake -DENABLE_AUTOMATIC_INIT_AND_CLEANUP=OFF ..
$ cmake --build .
$ sudo cmake --build . --target install
```
## Installing MongoDB CXX drivers
```
$ curl -OL https://github.com/mongodb/mongo-cxx-driver/releases/download/r3.7.2/mongo-cxx-driver-r3.7.2.tar.gz
$ tar -xzf mongo-cxx-driver-r3.7.2.tar.gz
$ cd mongo-cxx-driver-r3.7.2/build
$ cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr/local
% cmake --build .
% cmake --build . --target install
```

## build
``` 
cd src && mkdir build
cd build
cmake ..
make
./test <space seperated camera ids>
```
