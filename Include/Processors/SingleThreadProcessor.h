#pragma once
#include <Core/ImageProcessor.h>

class SingleThreadProcessor : public ImageProcessor
{
public:
    cv::Mat process(const cv::Mat& inputImage) override;
};
