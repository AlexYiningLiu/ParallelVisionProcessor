#pragma once
#include <Core/ImageProcessor.h>

class SingleThreadProcessor : public ImageProcessor
{
public:
    // Process the entire image in a single thread
    cv::Mat process(const cv::Mat &inputImage) override;
};
