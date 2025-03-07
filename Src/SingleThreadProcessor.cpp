#include <Processors/SingleThreadProcessor.h>

cv::Mat SingleThreadProcessor::process(const cv::Mat &inputImage)
{
    cv::Mat outputImage = inputImage.clone();

    // Process the entire image at once with a single thread
    applyHeavyFilter(outputImage, cv::Rect(0, 0, outputImage.cols, outputImage.rows));

    return outputImage;
}
