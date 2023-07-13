FROM ubuntu:20.04

ARG DEBIAN_FRONTEND=noninteractive

RUN apt-get update

RUN apt-get install -y wget nano  tar git cmake build-essential \
    unzip \
    pkg-config \
    libssl-dev

RUN mkdir /workspace

WORKDIR /workspace

COPY ./src /workspace/src

# install cmake
WORKDIR /workspace/src/Dependencies/cmake-3.27.0-rc2
RUN ./bootstrap
RUN make
RUN make install

#install mongodb c driver
WORKDIR /workspace/src/Dependencies/mongo-c-driver-1.23.5
RUN mkdir cmake-build
WORKDIR /workspace/src/Dependencies/mongo-c-driver-1.23.5/cmake-build
RUN cmake -DENABLE_AUTOMATIC_INIT_AND_CLEANUP=OFF ..
RUN cmake --build .
RUN cmake --build . --target install

#install mongodb c++ driver
WORKDIR /workspace/src/Dependencies/mongo-cxx-driver-r3.7.2/build
RUN cmake ..                                        \
    -DCMAKE_BUILD_TYPE=Release                      \
    -DCMAKE_INSTALL_PREFIX=/usr/local
RUN cmake --build . --target EP_mnmlstc_core
RUN cmake --build .
RUN cmake --build . --target install

RUN apt-get install -y zlib1g \
                    libssl-dev \
                    libsasl2-dev \
                    libzstd-dev \
                    libcurl4-openssl-dev

# install librdkafka
WORKDIR /workspace/src/Dependencies/librdkafka
RUN ./configure
RUN make
RUN make install

# install rapidjson
WORKDIR /workspace/src/Dependencies/rapidjson
RUN mkdir build
WORKDIR /workspace/src/Dependencies/rapidjson/build
RUN cmake ..
RUN make
RUN make install

# install nlohmann json
WORKDIR /workspace/src/Dependencies/json
RUN mkdir build 
WORKDIR /workspace/src/Dependencies/json/build
RUN cmake ..
RUN make
RUN make install

RUN apt-get install libboost-all-dev -y

# install googletest
WORKDIR /workspace/src/Dependencies/googletest
RUN mkdir build
WORKDIR /workspace/src/Dependencies/googletest/build
RUN cmake ..
RUN make
RUN make install

# install modern-cpp-kafka
WORKDIR /workspace/src/Dependencies/modern-cpp-kafka
RUN mkdir build
WORKDIR /workspace/src/Dependencies/modern-cpp-kafka/build
RUN cmake ..
RUN make
RUN make install

# remove dependencies folder
WORKDIR /workspace/src/
RUN rm -rf Dependencies

# # build DB_Subscriber
RUN mkdir /workspace/src/build
WORKDIR /workspace/src/build
RUN cmake .. 
RUN make

ENTRYPOINT [ "./Consumer" ]