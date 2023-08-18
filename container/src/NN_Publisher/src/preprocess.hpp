#include <opencv2/opencv.hpp>
#include <opencv2/dnn/dnn.hpp>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>

#include "./base64.h"

// function to preprocess the image and return the preprocessed image


// cv::Mat convertNHWCtoNCHW(const cv::Mat& image){
//     std::vector<cv::Mat> channels(3);
//     cv::split(image, channels);
//     cv::Mat imageNCHW;
//     cv::merge(channels, imageNCHW);
//     cv::transpose(imageNCHW, imageNCHW);
//     return imageNCHW;
// }

struct PreprocessedData {
    cv::Mat preprocessedImage;
    std::string encodedImage;
};

PreprocessedData preprocess(cv::Mat image, bool flag=false)
{
    PreprocessedData data;
    
    cv::Mat ImageRGB, resizedImage, preprocessedImage, transposedimage;

    if (flag==true){
    // convert mat to base64 string
    std::vector<uchar> buf;
    cv::imencode(".jpg", image, buf);
    auto *enc_msg = reinterpret_cast<unsigned char*>(buf.data());
    data.encodedImage = base64_encode(enc_msg, buf.size());
    }
    // cv:imshow("image", image);

    cv::cvtColor(image, ImageRGB, cv::ColorConversionCodes::COLOR_BGR2RGB);
    cv::resize(ImageRGB, ImageRGB, cv::Size(256, 256), cv::InterpolationFlags::INTER_LINEAR);
    
    // transposedimage = convertNHWCtoNCHW(ImageRGB);

    ImageRGB.convertTo(resizedImage, CV_32FC3, 1.0 / 255.0);

    cv::Mat channels[3];
    cv::split(resizedImage, channels);

    channels[0] = (channels[0] - 0.485) / 0.229;
    channels[1] = (channels[1] - 0.456) / 0.224;
    channels[2] = (channels[2] - 0.406) / 0.225;

    cv::merge(channels, 3, resizedImage);

    cv::dnn::blobFromImage(resizedImage, preprocessedImage);

    data.preprocessedImage = preprocessedImage;

    return data;
}
