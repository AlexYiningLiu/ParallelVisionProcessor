#include "Visualizer.h"
// ...existing code...

void Visualizer::displayImage(const cv::Mat& image) // changed: "const cv::Mat &image" -> "const cv::Mat& image"
{
    // ...existing code...
}

void Visualizer::updateWindow(
    const std::string& title) // changed: "const std::string &title" -> "const std::string& title"
{
    // ...existing code...
}

void Visualizer::processRegions(const std::vector<cv::Rect>& regions) // changed: "const std::vector<cv::Rect> &regions"
                                                                      // -> "const std::vector<cv::Rect>& regions"
{
    for (const auto& region : regions) // changed: "const auto &region" -> "const auto& region"
    {
        // ...existing processing code...
    }
}

// ...existing code...
