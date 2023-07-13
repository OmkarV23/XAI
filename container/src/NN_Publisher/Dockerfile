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

COPY ./src /workspace/src

RUN mkdir /usr/local/include/onnxruntime
WORKDIR /usr/local/include/onnxruntime
RUN mv /workspace/src/Dependencies/onnxruntime/include/* .

RUN mv /workspace/src/Dependencies/onnxruntime/lib/* /usr/local/lib/

WORKDIR /workspace/src/Dependencies/cmake-3.27.0-rc2
RUN ./bootstrap
RUN make
RUN make install

WORKDIR /workspace/src/Dependencies/opencv
RUN mkdir build
WORKDIR /workspace/src/Dependencies/opencv/build

RUN cmake -D CMAKE_BUILD_TYPE=RELEASE \
    -D CMAKE_INSTALL_PREFIX=/usr/local \
    # -D INSTALL_PYTHON_EXAMPLES=ON \
    # -D BUILD_NEW_PYTHON_SUPPORT=ON \
    # -D BUILD_PYTHON_SUPPORT=ON \
    -D INSTALL_C_EXAMPLES=OFF \
    # -D OPENCV_PYTHON3_INSTALL_PATH=/opt/conda/lib/python3.8/site-packages \
    -D OPENCV_EXTRA_MODULES_PATH=/workspace/src/Dependencies/opencv_contrib/modules/ \
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

RUN apt-get install -y zlib1g \
                    libssl-dev \
                    libsasl2-dev \
                    libzstd-dev \
                    libcurl4-openssl-dev

WORKDIR /workspace/src/Dependencies/librdkafka
RUN ./configure
RUN make
RUN make install

WORKDIR /workspace/src/Dependencies/rapidjson
RUN mkdir build
WORKDIR /workspace/src/Dependencies/rapidjson/build
RUN cmake ..
RUN make
RUN make install

WORKDIR /workspace/src/Dependencies/json
RUN mkdir build
WORKDIR /workspace/src/Dependencies/json/build
RUN cmake ..
RUN make
RUN make install

RUN apt-get install libboost-all-dev -y

WORKDIR /workspace/src/Dependencies/googletest
RUN mkdir build
WORKDIR /workspace/src/Dependencies/googletest/build
RUN cmake ..
RUN make
RUN make install

WORKDIR /workspace/src/Dependencies/modern-cpp-kafka
RUN mkdir build
WORKDIR /workspace/src/Dependencies/modern-cpp-kafka/build
RUN cmake ..
RUN make
RUN make install

WORKDIR /workspace/src/
RUN rm -rf Dependencies

RUN mkdir /workspace/src/build
WORKDIR /workspace/src/build 
RUN cmake .. 
RUN make

ENTRYPOINT [ "./Publisher" ]