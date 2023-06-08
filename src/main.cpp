#include <chrono>
#include <cmath>
#include <exception>
#include <fstream>
#include <iostream>
#include <limits>
#include <numeric>
#include <string>
#include <vector>
#include <cstdlib>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <opencv2/opencv.hpp>
#include <opencv2/dnn/dnn.hpp>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include </workspace/onnxruntime-linux-x64-gpu-1.15.0/include/onnxruntime_cxx_api.h>

#include "db.hpp"
#include "preprocess.hpp"

std::mutex printMutex;

MongoDBHelper helper;

template <typename T>
T vectorProduct(const std::vector<T>& v)
{
    return accumulate(v.begin(), v.end(), 1, std::multiplies<T>());
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
    std::string modelFilepath{"/workspace/omkar_projects/XAI/mobilenetv2-12-int8.onnx"};
    std::string inputName;
    std::string outputName;

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

    while (true) {
        cv::Mat frame;
        cap >> frame;
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

        //get the max value from the output tensor
        float max_value = *std::max_element(outputTensorValues.begin(), outputTensorValues.end());
        //get the index of the max element
        auto max_index = std::max_element(outputTensorValues.begin(), outputTensorValues.end()) - outputTensorValues.begin();
        std::cout << "Camera " << cameraIndex << ": " << max_index << " " << max_value << std::endl;

        std::string camera_index = std::to_string(cameraIndex);
        auto data = make_document(kvp("camera", cameraIndex), kvp("max_index", max_index), kvp("max_value", max_value));
        helper.insertDocument("onnx_output", camera_index, data.view());

        // std::cout << "Camera " << cameraIndex << ": ";
        // for (int i = 0; i < outputTensorSize; i++) {
        //     std::cout << outputTensorValues.at(i) << " ";
        // }
        // std::cout << std::endl;

        char c = (char)cv::waitKey(1);
        if (c == 'q')
            break;
    }
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

    for (pid_t pid : pids) {
        waitpid(pid, NULL, 0);
    }
    return 0;
}