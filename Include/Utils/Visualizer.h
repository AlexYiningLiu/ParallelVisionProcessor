#pragma once
#include <opencv2/opencv.hpp>
#include <string>
#include <vector>

class Visualizer
{
public:
    static void displayImages(const cv::Mat &original, const cv::Mat &singleThread, const cv::Mat &multiThread,
                              const std::vector<cv::Rect> &regions);

    static void displayProcessingRegions(cv::Mat &image, const std::vector<cv::Rect> &regions);

    static void saveTimingChart(const std::string &filename, double singleThreadTime, double multiThreadTime,
                                int numThreads);
};
