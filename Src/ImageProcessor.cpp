#include <Core/ImageProcessor.h>

void ImageProcessor::applyHeavyFilter(cv::Mat& image, const cv::Rect& roi)
{

    cv::Mat region = image(roi);
    cv::Mat temp;

    cv::bilateralFilter(region, temp, 5, 75, 75);

    cv::detailEnhance(temp, region, 10, 0.3);

    cv::Mat channels[3];
    cv::split(region, channels);

    channels[0] += 10;

    cv::merge(channels, 3, region);
}
