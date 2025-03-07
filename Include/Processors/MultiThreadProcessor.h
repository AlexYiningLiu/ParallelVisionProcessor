#pragma once
#include <Core/ImageProcessor.h>
#include <Utils/ThreadPool.h>

#include <barrier>
#include <cmath>
#include <future>
#include <latch>
#include <mutex>
#include <semaphore>
#include <thread>
#include <vector>

class MultiThreadProcessor : public ImageProcessor
{
public:
    // Supported threading strategies
    enum class ThreadingStrategy
    {
        Async,      // Use std::async
        ThreadPool, // Use thread pool
        JThread     // Use std::jthread (C++20)
    };

    // Constructor takes number of threads and threading strategy
    explicit MultiThreadProcessor(int numThreads, ThreadingStrategy strategy = ThreadingStrategy::ThreadPool);

    // Process the image using multiple threads
    cv::Mat process(const cv::Mat &inputImage) override;

    // Split the image into regions for multi-threaded processing
    std::vector<cv::Rect> divideImageIntoRegions(const cv::Mat &image);

private:
    int mNumThreads;
    ThreadingStrategy mStrategy;
    std::unique_ptr<ThreadPool> mThreadPool;

    // Process a specific region of the image
    void processRegion(cv::Mat &image, const cv::Rect &region);

    // Implementation using thread pool
    cv::Mat processWithThreadPool(const cv::Mat &inputImage);

    // Implementation using std::async
    cv::Mat processWithAsync(const cv::Mat &inputImage);

    // Implementation using std::jthread (C++20)
    cv::Mat processWithJThreads(const cv::Mat &inputImage);
};
