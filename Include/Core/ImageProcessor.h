#pragma once
#include <opencv2/opencv.hpp>

class ImageProcessor
{
public:
    virtual ~ImageProcessor() = default;

    // Pure virtual function to be implemented by derived classes
    virtual cv::Mat process(const cv::Mat &inputImage) = 0;

protected:
    // Apply a computationally intensive filter to a region of the image
    static void applyHeavyFilter(cv::Mat &image, const cv::Rect &roi);
};
