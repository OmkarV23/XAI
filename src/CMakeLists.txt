cmake_minimum_required(VERSION 3.12)
project(onnx_test)

# Find OpenCV package
find_package(OpenCV 4 REQUIRED)
find_package(libmongocxx REQUIRED)
find_package(libbsoncxx REQUIRED)

# Set the include OpenCV directories
include_directories(
    ${OpenCV_INCLUDE_DIRS}
)

# Find ONNX Runtime
set(ONNXRUNTIME_DIR "/workspace/onnxruntime-linux-x64-gpu-1.15.0")
include_directories(${ONNXRUNTIME_DIR}/include)
link_directories(${ONNXRUNTIME_DIR}/lib)

# Find libmongocxx and libbsoncxx
set(MONGO_CXX_DIR "/usr/local")
include_directories("${MONGO_CXX_DIR}/include/bsoncxx/v_noabi/bsoncxx")
include_directories("${MONGO_CXX_DIR}/include/mongocxx/v_noabi/mongocxx")
include_directories("${MONGO_CXX_DIR}/include/libmongoc-1.0")
include_directories("${MONGO_CXX_DIR}/include/libbson-1.0")
link_directories("${MONGO_CXX_DIR}/lib")

# Set the source file
set(SOURCES main.cpp)

# Create the executable
add_executable(test ${SOURCES})

# Link the libraries
target_link_libraries(test
    PRIVATE
    ${OpenCV_LIBS}
    ${ONNXRUNTIME_DIR}/lib/libonnxruntime.so
    ${LIBMONGOCXX_LIBRARIES}
    ${LIBBSONCXX_LIBRARIES}
)

target_include_directories(test
    PRIVATE
    ${OpenCV_INCLUDE_DIRS}
    ${ONNXRUNTIME_DIR}/include
    ${LIBMONGOCXX_INCLUDE_DIRS}
    ${LIBBSONCXX_INCLUDE_DIRS}
)