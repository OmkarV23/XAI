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
#include <kafka/KafkaProducer.h>
#include <opencv2/opencv.hpp>
#include <opencv2/dnn/dnn.hpp>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <onnxruntime/onnxruntime_cxx_api.h>
# include <nlohmann/json.hpp>

#include "preprocess.hpp"

using namespace kafka;
using namespace kafka::clients::producer;
using json = nlohmann::json;

const std::string brokers = "75.204.78.27:9092";

// Prepare the configuration
const Properties props({{"bootstrap.servers", brokers}});

std::mutex printMutex;

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

void processCamera(std::string cameraIndex, std::string rtspUrl)
{
    std::string modelFilepath{"../../model/mobilenetv2-12-int8.onnx"};
    std::string inputName;
    std::string outputName;

    // const Topic topic = std::to_string(cameraIndex);
    const Topic topic = cameraIndex;

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

    cv::VideoCapture cap(rtspUrl);
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


    // std::vector<cv::Mat> frames;
    bool flag = false;
    std::string message;

    while (true) {
        cv::Mat frame;
        cap >> frame;
        frame_count++;

        if (frame.empty())
            break;
        
        // frames.push_back(frame);
        // cv::Mat preprocessedImage = preprocess(frame);
        PreprocessedData result = preprocess(frame,flag);
        cv::Mat preprocessedImage = result.preprocessedImage;
        if (flag == true){
            std::cout << "Encoding Image" << std::endl;
            std::string encodedImage = result.encodedImage;
            message = encodedImage;
            flag = false;
        }

        // std::string encodedImage = result.encodedImage;

        // if (cameraIndex == "1"){
        // std::cout << "Camera Index" << cameraIndex << std::endl;
	    // std::cout << "Encoded Image: " << encodedImage.substr(encodedImage.size()-10, encodedImage.size()) << std::endl;
        // }
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

        stop = std::chrono::steady_clock::now();
        //check if stop-begin== 20 milliseconds
        std::chrono::duration<double> elapsedSeconds = (stop - begin);

        std::cout << "Elapsed Seconds: " << elapsedSeconds.count() << std::endl;

        if (std::abs(elapsedSeconds.count()-0.2) < 0.01 || std::abs(elapsedSeconds.count()-0.8) < 0.01){
            std::cout <<"Making flag true" << std::endl;
            flag = true;
        }

        if (std::abs(elapsedSeconds.count() - 1.0) <= 0.1){
            std::cout << "Sending Message" << std::endl;
            // std::cout << "Camera " << cameraIndex << " fps : " << frame_count / elapsedSeconds.count() << std::endl;
            // frame_count = 0;
            begin = std::chrono::steady_clock::now();
        }

        // if(frame_count==100){
        //     stop = std::chrono::steady_clock::now();
        //     std::chrono::duration<double> elapsedSeconds = (stop - begin);
        //     std::cout << "Camera " << cameraIndex << " fps : " << frame_count / elapsedSeconds.count() << std::endl;
        //     frame_count = 0;
        //     begin = std::chrono::steady_clock::now();
        //     } 

        //get the max value from the output tensor
        float max_value = *std::max_element(outputTensorValues.begin(), outputTensorValues.end());
        //get the index of the max element
        auto max_index = std::max_element(outputTensorValues.begin(), outputTensorValues.end()) - outputTensorValues.begin();
        // std::cout << "Camera " << cameraIndex << ": " << max_index << " " << max_value << std::endl;

        if (message.size() == 0){
            message = std::to_string(max_index) + "\t" + std::to_string(max_value);
        }
        else{
            message = std::to_string(max_index) + "\t" + std::to_string(max_value) + "\t" + message;
        }

        ProducerRecord record(topic, Key(topic.c_str(), topic.size()) , Value(message.c_str(), message.size()));

        auto deliveryCb = [](const RecordMetadata& metadata, const Error& error) {
            if (!error) {
                std::cout << "Message delivered: " << metadata.toString() << std::endl;
            } else {
                std::cerr << "Message failed to be delivered: " << error.message() << std::endl;
            }
        };

        producer.send(record, deliveryCb);
        message.clear();

        char c = (char)cv::waitKey(1);
        if (c == 'q')
            break;
    }
    producer.close();
    cap.release();
}

int main(int argc, char* argv[])
{
    if (argc!=2) {
        std::cout << "Please provide camera mapping file" << std::endl;
        return 1;
    }

    std::ifstream f(argv[1]);
    if (!f.is_open()) {
        std::cout << "Error opening camera mapping file" << std::endl;
        return 1;
    }
    json json_data = json::parse(f);
    json& cameras = json_data["Camera_map"];

    std::vector<pid_t> pids;
    for (json::iterator it = cameras.begin(); it != cameras.end(); ++it) {

        std::string cameraIndex = it.key();
        json rtsp_url = it.value();
        pid_t pid = fork();
        if (pid == 0) {
            processCamera(cameraIndex, rtsp_url["camera_ip"]);
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
