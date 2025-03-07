#pragma once
#include <opencv2/opencv.hpp>
#include <string>
#include <vector>

class Visualizer
{
public:
    // Display a comparison of original, region view, and processed images
    static void displayImages(const cv::Mat &original, const cv::Mat &singleThread, const cv::Mat &multiThread,
                              const std::vector<cv::Rect> &regions);

    // Visualize the image regions used for multi-threading
    static void displayProcessingRegions(cv::Mat &image, const std::vector<cv::Rect> &regions);

    // Save visual comparison of timing results
    static void saveTimingChart(const std::string &filename, double singleThreadTime, double multiThreadTime,
                                int numThreads);
};
