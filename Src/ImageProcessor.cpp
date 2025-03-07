#include <Core/ImageProcessor.h>

void ImageProcessor::applyHeavyFilter(cv::Mat &image, const cv::Rect &roi)
{
    // Extract the region of interest
    cv::Mat region = image(roi);
    cv::Mat temp;

    // Apply bilateral filter with moderate parameters
    // Still computationally intensive but not excessive
    cv::bilateralFilter(region, temp, 5, 75, 75);

    // Apply a detail enhance filter (computationally intensive but visually
    // noticeable)
    cv::detailEnhance(temp, region, 10, 0.3);

    // Add a slight color shift to make the effect more obvious
    cv::Mat channels[3];
    cv::split(region, channels);

    // Enhance blue channel slightly
    channels[0] += 10;

    // Recombine channels
    cv::merge(channels, 3, region);
}
