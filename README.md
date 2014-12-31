# Bitkey v0.9 beta - work in progress

## Introduction

*Bitkey* is an open-source platform-independet cryptocurrency framework
for anonymous online transactions. 

##Dependencies

The BitKey library depends on openssl, cmake, curl, libjson-cpp, libjson-rpc-cpp, librootcoin and boost
to be built.
```sh
sudo apt-get install cmake libboost-all-dev libssl-dev libcurl4-openssl-dev libjsoncpp-dev
```
**Install libjson-rpc-cpp and its dependencies**

Dependencies:
```sh
sudo apt-get install libmicrohttpd-dev libargtable2-dev
```

Build and install:
```sh
git clone https://github.com/cinemast/libjson-rpc-cpp
mkdir -p libjson-rpc-cpp/build && cd libjson-rpc-cpp/build
cmake .. && make
sudo make install
sudo ldconfig   #linking only required on linux
```

**Install [librootcoin](https://github.com/bitsta/librootcoin) like described in the instructions.

##Configuration

##Usage

##More Info

###Development
