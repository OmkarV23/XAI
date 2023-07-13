cmake_minimum_required(VERSION 3.12)
project(NN_Publisher)

# Find OpenCV package
find_package(OpenCV 4 REQUIRED)

# Set the include OpenCV directories
include_directories(${OpenCV_INCLUDE_DIRS})

# Set the source file
set(SOURCES main.cpp)

# Create the executable
add_executable(Publisher ${SOURCES} base64.cpp)

# Link the libraries
target_link_libraries(Publisher
    PRIVATE
    rdkafka
    pthread
    z
    stdc++
    ${OpenCV_LIBS}
)

target_include_directories(Publisher
    PRIVATE
    ${OpenCV_INCLUDE_DIRS}
)

# Add the library directory to the linker's search path
link_directories("/usr/local/lib")

# Link libonnxruntime
target_link_libraries(Publisher
    PRIVATE
    onnxruntime
)
