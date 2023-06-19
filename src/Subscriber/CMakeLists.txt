cmake_minimum_required(VERSION 3.12)
project(KafkaConsumerExample)

set(CMAKE_CXX_STANDARD 17)

find_package(libmongocxx REQUIRED)
find_package(libbsoncxx REQUIRED)

# Set the path to librdkafka include directory and library path
set(librdkafka_INCLUDE_DIR "/usr/local/include/librdkafka")
set(librdkafka_LIBRARY "/usr/local/lib/librdkafka.so")

# set and include mongodb
set(MONGO_CXX_DIR "/usr/local")
include_directories("${MONGO_CXX_DIR}/include/bsoncxx/v_noabi/bsoncxx")
include_directories("${MONGO_CXX_DIR}/include/mongocxx/v_noabi/mongocxx")
include_directories("${MONGO_CXX_DIR}/include/libmongoc-1.0")
include_directories("${MONGO_CXX_DIR}/include/libbson-1.0")
link_directories("${MONGO_CXX_DIR}/lib")

# Add the executable
add_executable(KafkaConsumerExample tmp.cpp)

# Include the Kafka and librdkafka header files
target_include_directories(KafkaConsumerExample PRIVATE
    ${librdkafka_INCLUDE_DIR}     
    ${LIBMONGOCXX_INCLUDE_DIRS}
    ${LIBBSONCXX_INCLUDE_DIRS}
)

# Link against the librdkafka library
target_link_libraries(KafkaConsumerExample PRIVATE
    ${librdkafka_LIBRARY} 
    ${LIBMONGOCXX_LIBRARIES}
    ${LIBBSONCXX_LIBRARIES}
    pthread
)