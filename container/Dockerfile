FROM nvidia/cuda:11.7.0-cudnn8-runtime-ubuntu20.04

ARG DEBIAN_FRONTEND=noninteractive 

RUN apt-get update

RUN apt-get install -y wget nano  tar git cmake build-essential \
    unzip \
    pkg-config \
    libjpeg-dev \
    libpng-dev \
    libtiff-dev \
    libavcodec-dev \
    libavformat-dev \
    libswscale-dev \
    libv4l-dev \
    libxvidcore-dev \
    libx264-dev \
    libgtk-3-dev \
    libatlas-base-dev \
    gfortran \
    libgstreamer1.0-dev \
    libgstreamer-plugins-base1.0-dev \
    libdc1394-22-dev \
    libavresample-dev \
    libssl-dev

RUN mkdir /workspace

WORKDIR /workspace

# RUN python3 -m pip install onnxruntime_gpu

RUN wget -O onnxruntime.tgz https://github.com/microsoft/onnxruntime/releases/download/v1.15.0/onnxruntime-linux-x64-gpu-1.15.0.tgz
RUN tar -xvf onnxruntime.tgz && mv onnxruntime-linux-x64-gpu-1.15.0 onnxruntime

RUN mkdir /usr/local/include/onnxruntime
WORKDIR /usr/local/include/onnxruntime
RUN mv /workspace/onnxruntime/include/* .

RUN mv /workspace/onnxruntime/lib/* /usr/local/lib/

WORKDIR /workspace

RUN rm -r onnxruntime*

RUN wget https://github.com/Kitware/CMake/releases/download/v3.27.0-rc2/cmake-3.27.0-rc2.tar.gz
RUN tar -xvf cmake-3.27.0-rc2.tar.gz
WORKDIR /workspace/cmake-3.27.0-rc2
RUN ./bootstrap
RUN make
RUN make install

WORKDIR /workspace

RUN git clone --depth 1 --branch 4.7.0 https://github.com/opencv/opencv.git
RUN git clone --depth 1 --branch 4.7.0 https://github.com/opencv/opencv_contrib.git

WORKDIR /workspace/opencv

RUN mkdir build
WORKDIR /workspace/opencv/build

RUN cmake -D CMAKE_BUILD_TYPE=RELEASE \
    -D CMAKE_INSTALL_PREFIX=/usr/local \
    # -D INSTALL_PYTHON_EXAMPLES=ON \
    # -D BUILD_NEW_PYTHON_SUPPORT=ON \
    # -D BUILD_PYTHON_SUPPORT=ON \
    -D INSTALL_C_EXAMPLES=OFF \
    # -D OPENCV_PYTHON3_INSTALL_PATH=/opt/conda/lib/python3.8/site-packages \
    -D OPENCV_EXTRA_MODULES_PATH=/workspace/opencv_contrib/modules/ \
    # -D PYTHON3_EXECUTABLE=/opt/conda/bin/python3 \
    -D BUILD_EXAMPLES=ON \
    # -D WITH_CUDA=ON \
    # -D CUDA_ARCH_BIN=7.5 \
    # -D CUDA_ARCH_PTX="" \
    -D ENABLE_FAST_MATH=ON \
    # -D CUDA_FAST_MATH=ON \
    # -D WITH_CUBLAS=ON \
    -D WITH_V4L=ON \
    -D WITH_LIBV4L=ON \
    -D WITH_FFMPEG=1 \
    -D WITH_QT=ON \
    -D WITH_GTK=ON \
    -D WITH_OPENGL=ON \
    -D WITH_GSTREAMER=ON \
    -D WITH_GSTREAMER_0_10=ON \
    -D WITH_TBB=ON \
    ../

RUN make
RUN make install
RUN ldconfig

WORKDIR /workspace

RUN wget https://github.com/mongodb/mongo-c-driver/releases/download/1.23.5/mongo-c-driver-1.23.5.tar.gz
RUN tar xzf mongo-c-driver-1.23.5.tar.gz
WORKDIR /workspace/mongo-c-driver-1.23.5
RUN mkdir cmake-build
WORKDIR /workspace/mongo-c-driver-1.23.5/cmake-build
RUN cmake -DENABLE_AUTOMATIC_INIT_AND_CLEANUP=OFF ..
RUN cmake --build .
RUN cmake --build . --target install

WORKDIR /workspace

RUN wget https://github.com/mongodb/mongo-cxx-driver/releases/download/r3.7.2/mongo-cxx-driver-r3.7.2.tar.gz
RUN tar -xzf mongo-cxx-driver-r3.7.2.tar.gz
WORKDIR /workspace/mongo-cxx-driver-r3.7.2/build
RUN cmake ..                                        \
    -DCMAKE_BUILD_TYPE=Release                      \
    -DCMAKE_INSTALL_PREFIX=/usr/local
RUN cmake --build . --target EP_mnmlstc_core
RUN cmake --build .
RUN cmake --build . --target install

WORKDIR /workspace

RUN apt-get install -y zlib1g \
                    libssl-dev \
                    libsasl2-dev \
                    libzstd-dev \
                    libcurl4-openssl-dev
RUN git clone https://github.com/confluentinc/librdkafka.git
WORKDIR /workspace/librdkafka
RUN ./configure
RUN make
RUN make install

WORKDIR /workspace

RUN git clone https://github.com/Tencent/rapidjson.git
WORKDIR /workspace/rapidjson
RUN mkdir build && cd build && cmake .. && make && make install

WORKDIR /workspace

RUN git clone https://github.com/nlohmann/json.git
WORKDIR /workspace/json
RUN mkdir build && cd build && cmake .. && make && make install

RUN apt-get install libboost-all-dev -y

WORKDIR /workspace

RUN git clone https://github.com/google/googletest.git
WORKDIR /workspace/googletest
RUN mkdir build && cd build && cmake .. && make && make install

WORKDIR /workspace

RUN git clone https://github.com/morganstanley/modern-cpp-kafka.git
WORKDIR /workspace/modern-cpp-kafka
RUN mkdir build && cd build && cmake .. && make && make install

WORKDIR /workspace
RUN rm -r mongo* cmake* opencv* rapidjson* librdkafka* json* modern-cpp-kafka* googletest*

RUN usermod -a -G video root
RUN apt-get install -y libcanberra-gtk-module

COPY ./src .
# RUN mkdir build
# WORKDIR /workspace/build
# RUN cmake ..
# RUN make

# ENTRYPOINT ["./test"]