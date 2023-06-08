#include <opencv2/opencv.hpp>
#include <opencv2/dnn/dnn.hpp>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>

// function to preprocess the image and return the preprocessed image


// cv::Mat convertNHWCtoNCHW(const cv::Mat& image){
//     std::vector<cv::Mat> channels(3);
//     cv::split(image, channels);
//     cv::Mat imageNCHW;
//     cv::merge(channels, imageNCHW);
//     cv::transpose(imageNCHW, imageNCHW);
//     return imageNCHW;
// }

cv::Mat preprocess(cv::Mat image)
{
    cv::Mat ImageRGB, resizedImage, preprocessedImage, transposedimage;

    cv:imshow("image", image);

    cv::cvtColor(image, ImageRGB, cv::ColorConversionCodes::COLOR_BGR2RGB);
    cv::resize(ImageRGB, ImageRGB, cv::Size(224, 224), cv::InterpolationFlags::INTER_LINEAR);
    
    // transposedimage = convertNHWCtoNCHW(ImageRGB);

    ImageRGB.convertTo(resizedImage, CV_32FC3, 1.0 / 255.0);

    // cv::Mat channels[3];
    // cv::split(resizedImage, channels);

    // channels[0] = (channels[0] - 0.485) / 0.229;
    // channels[1] = (channels[1] - 0.456) / 0.224;
    // channels[2] = (channels[2] - 0.406) / 0.225;

    // cv::merge(channels, 3, resizedImage);

    cv::dnn::blobFromImage(resizedImage, preprocessedImage);

    return preprocessedImage;
}