#pragma once
#include <opencv2/opencv.hpp>

class ImageProcessor
{
public:
    virtual ~ImageProcessor() = default;

    virtual cv::Mat process(const cv::Mat& inputImage) = 0;

protected:
    static void applyHeavyFilter(cv::Mat& image, const cv::Rect& roi);
};
