#include <kafka/KafkaConsumer.h>
#include <rdkafka.h>
#include <cstdlib>
#include <iostream>
#include <signal.h>
#include <string>
#include <fstream>
#include <sstream>
#include <unistd.h>
#include <sys/wait.h>
# include <nlohmann/json.hpp>
#include "db.hpp"

MongoDBHelper helper;

using namespace kafka;
using namespace kafka::clients::consumer;
using json = nlohmann::json;

std::atomic_bool running = {true};
// std::unordered_map<std::string, KafkaConsumer> consumerMap;

void stopRunning(int sig) {
    if (sig != SIGINT) return;

    if (running) {
        running = false;
    } else {
        // Restore the signal handler, -- to avoid stuck with this handler
        signal(SIGINT, SIG_IGN); // NOLINT
    }
}

void consumeMessages(const std::string& topic) {
    
    const std::string brokers = "75.204.78.27:9092";

    // Prepare the configuration
    const Properties props({{"bootstrap.servers", {brokers}}});

    // Create a consumer instance
    KafkaConsumer consumer(props);

    // Subscribe to topics
    consumer.subscribe({topic});
    
    while (running) {
        // Poll messages from Kafka brokers
        auto records = consumer.poll(std::chrono::milliseconds(100));

        for (const auto& record: records) {
            if (!record.error()) {
                std::cout << "Got a new message..." << std::endl;
                if (record.topic() == record.key().toString()) {
                    int cameraIndex = std::stoi(record.topic());
                    std::string timestamp = record.timestamp().toString();
                    std::vector<std::string> tokens;
                    std::string value = record.value().toString();

                    // Split value using ',' delimiter
                    std::istringstream iss(value);
                    std::string token;
                    while (std::getline(iss, token, ',')) {
                        tokens.push_back(token);
                    }
                    std::cout << "VECTOR SIZE: " << tokens.size() << std::endl;
                    try{
                        if (tokens.size() >= 3) {
                            int cls = std::stoi(tokens[0]);
                            double confidence = std::stod(tokens[1]);
                            std::string encodedImage = tokens[2];

                            // std::cout << "  Camera Index:   " << cameraIndex << std::endl;
                            // std::cout << "  Timestamp:  " << timestamp << std::endl;
                            // std::cout << "  Class:  " << cls << std::endl;
                            // std::cout << "  Confidence: " << confidence << std::endl;
                            // std::cout << "  Encoded Image: " << encodedImage << std::endl;
                            // std::cout << "  Offset: " << record.offset() << std::endl;

                            // std::string camera_index = std::to_string(cameraIndex);
                            // auto data = make_document(kvp("camera", cameraIndex), kvp("max_index", cls),
                            //                         kvp("confidence", confidence), kvp("timestamp", timestamp));
                            // helper.insertDocument("onnx_output", camera_index, data.view());
                        }
                    }
                    catch (std::exception& e) {
                        //pass  
                    }
                }
            } else {
                std::cerr << record.toString() << std::endl;
            }
        }
    }

    // No explicit close is needed, RAII will take care of it
    consumer.close();
}

int main(int argc, char* argv[])
{
    if (argc!=2) {
        std::cout << "Please provide camera mapping file" << std::endl;
        return 1;
    }

    // Use Ctrl-C to terminate the program
    signal(SIGINT, stopRunning); // NOLINT

    std::vector<Topic> topics;

    std::ifstream f(argv[1]);
    if (!f.is_open()) {
        std::cout << "Error opening camera mapping file" << std::endl;
        return 1;
    }
    json json_data = json::parse(f);
    json& cameras = json_data["Camera_map"];

    for (json::iterator it = cameras.begin(); it != cameras.end(); ++it) {
        std::string cameraIndex = it.key();
        topics.push_back(Topic(cameraIndex));
    }

    std::vector<pid_t> pids;
    for (const auto& topic : topics) {
        pid_t pid = fork();
        if (pid == 0) {
            consumeMessages(topic);
            exit(0);
        } else {
            pids.push_back(pid);
        }
    }

    for (pid_t pid : pids) {
            waitpid(pid, NULL, 0);
        }

    return EXIT_SUCCESS;
}