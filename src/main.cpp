#include <chrono>
#include <cmath>
#include <exception>
#include <fstream>
#include <iostream>
#include <limits>
#include <numeric>
#include <string>
#include <vector>
#include <variant>
#include <cstdlib>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
// #include <curl.h>
#include <kafka/KafkaProducer.h>
#include <opencv2/opencv.hpp>
#include <opencv2/dnn/dnn.hpp>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <onnxruntime/onnxruntime_cxx_api.h>

// #include "db.hpp"
#include "preprocess.hpp"

using namespace kafka;
using namespace kafka::clients::producer;

const std::string brokers = "localhost:9092";

// Prepare the configuration
const Properties props({{"bootstrap.servers", brokers}});

std::mutex printMutex;

// MongoDBHelper helper;

// CURL* curl;
// CURLcode res;

// curl_global_init(CURL_GLOBAL_DEFAULT);
// curl = curl_easy_init();

template <typename T>
T vectorProduct(const std::vector<T>& v)
{
    return accumulate(v.begin(), v.end(), 1, std::multiplies<T>());
}

size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* response)
{
    size_t totalSize = size * nmemb;
    response->append(static_cast<char*>(contents), totalSize);
    return totalSize;
}

Ort::MemoryInfo memoryInfo = Ort::MemoryInfo::CreateCpu(OrtDeviceAllocator, OrtMemTypeCPU);

std::vector<int64_t> inputShape{1, 3, 224, 224};
std::vector<int64_t> outputShape{1, 1000};

size_t inputTensorSize = vectorProduct(inputShape);

// implement a softmax function
std::vector<float> softmax(std::vector<float> inputTensorValues){
    std::vector<float> outputTensorValues(inputTensorSize);
    float sum = 0.0;
    for (int i = 0; i < inputTensorValues.size(); i++){
        sum += exp(inputTensorValues[i]);
    }
    for (int i = 0; i < inputTensorValues.size(); i++){
        outputTensorValues[i] = exp(inputTensorValues[i]) / sum;
    }
    return outputTensorValues;
}

void processCamera(int cameraIndex)
{
    std::string modelFilepath{"../../mobilenetv2-12-int8.onnx"};
    std::string inputName;
    std::string outputName;

    const Topic topic = std::to_string(cameraIndex);
    // const Topic topic = cameraIndex;

    Ort::Env env{ORT_LOGGING_LEVEL_WARNING, "test"};
    Ort::SessionOptions sessionOptions;
    OrtCUDAProviderOptions cuda_options{};
    cuda_options.device_id = 0;
    sessionOptions.AppendExecutionProvider_CUDA(cuda_options);
    sessionOptions.SetGraphOptimizationLevel(GraphOptimizationLevel::ORT_ENABLE_EXTENDED);
    Ort::Session session_{env, modelFilepath.c_str(), sessionOptions};

    std::vector<const char*> inputNames;
    std::vector<const char*> outputNames;

    inputNames.push_back("input");
    outputNames.push_back("output");

    cv::VideoCapture cap(cameraIndex);
    if (!cap.isOpened()) {
        std::lock_guard<std::mutex> lock(printMutex);
        std::cout << "Error opening video stream or file for camera " << cameraIndex << std::endl;
        return;
    }

    KafkaProducer producer(props);

    int frame_count = 0;
    double fps;
    std::chrono::steady_clock::time_point begin, stop;
    begin = std::chrono::steady_clock::now();


    while (true) {
        cv::Mat frame;
        cap >> frame;
        frame_count++;

        if (frame.empty())
            break;

        cv::Mat preprocessedImage = preprocess(frame);

        std::vector<float> inputTensorValues(inputTensorSize);

        for (int64_t i = 0; i < inputShape.at(0); ++i)
        {
            std::copy(preprocessedImage.begin<float>(),
                    preprocessedImage.end<float>(),
                    inputTensorValues.begin() + i * inputTensorSize / inputShape.at(0));
        }

        size_t outputTensorSize = vectorProduct(outputShape);
        std::vector<float> outputTensorValues(outputTensorSize);

        std::vector<Ort::Value> inputTensors;
        std::vector<Ort::Value> outputTensors;

        inputTensors.push_back(Ort::Value::CreateTensor<float>(
            memoryInfo, inputTensorValues.data(), inputTensorSize, inputShape.data(),
            inputShape.size()));
        outputTensors.push_back(Ort::Value::CreateTensor<float>(
            memoryInfo, outputTensorValues.data(), outputTensorSize,
            outputShape.data(), outputShape.size()));

        session_.Run(Ort::RunOptions{nullptr}, inputNames.data(),
                    inputTensors.data(), 1, outputNames.data(),
                    outputTensors.data(), 1);

        std::lock_guard<std::mutex> lock(printMutex);


        outputTensorValues = softmax(outputTensorValues);


        if(frame_count==100){
            stop = std::chrono::steady_clock::now();
            std::chrono::duration<double> elapsedSeconds = (stop - begin);
            std::cout << "Camera " << cameraIndex << " fps : " << frame_count / elapsedSeconds.count() << std::endl;
            frame_count = 0;
            begin = std::chrono::steady_clock::now();
            } 

        //get the max value from the output tensor
        float max_value = *std::max_element(outputTensorValues.begin(), outputTensorValues.end());
        //get the index of the max element
        auto max_index = std::max_element(outputTensorValues.begin(), outputTensorValues.end()) - outputTensorValues.begin();
        // std::cout << "Camera " << cameraIndex << ": " << max_index << " " << max_value << std::endl;

        std::string message;
        message = std::to_string(max_index) + "," + std::to_string(max_value);

        ProducerRecord record(topic, Key(topic.c_str(), topic.size()) , Value(message.c_str(), message.size()));

        auto deliveryCb = [](const RecordMetadata& metadata, const Error& error) {
            if (!error) {
                std::cout << "Message delivered: " << metadata.toString() << std::endl;
            } else {
                std::cerr << "Message failed to be delivered: " << error.message() << std::endl;
            }
        };

        producer.send(record, deliveryCb);

        // std::string camera_index = std::to_string(cameraIndex);
        // auto data = make_document(kvp("camera", cameraIndex), kvp("max_index", max_index), kvp("max_value", max_value));
        // helper.insertDocument("onnx_output", camera_index, data.view());

        char c = (char)cv::waitKey(1);
        if (c == 'q')
            break;
    }
    producer.close();
    cap.release();
}

int main(int argc, char* argv[])
{
    if (argc < 2) {
        std::cout << "Please provide camera index as a command-line argument." << std::endl;
        return 1;
    }

    std::vector<int> cameraIndexes;
    for (int i = 1; i < argc; ++i) {
        int cameraIndex = std::stoi(argv[i]);
        cameraIndexes.push_back(cameraIndex);
    }
    
    std::vector<pid_t> pids;
    for (int cameraIndex : cameraIndexes) {
        pid_t pid = fork();
        if (pid == 0) {
            processCamera(cameraIndex);
            return 0;
        } else {
            pids.push_back(pid);
        }
    }


    // std::vector<std::string> rtsp;
    // for (int i = 1; i < argc; ++i) {
    //     std::string rtsp_url = argv[i];
    //     rtsp.push_back(rtsp_url);
    // }

    // std::vector<pid_t> pids;
    // for (int i = 0; i < rtsp.size(); ++i) {
    //     pid_t pid = fork();
    //     if (pid == 0) {
    //         processCamera(rtsp[i]);
    //         return 0;
    //     } else {
    //         pids.push_back(pid);
    //     }
    // }

    for (pid_t pid : pids) {
        waitpid(pid, NULL, 0);
    }
    return 0;
}