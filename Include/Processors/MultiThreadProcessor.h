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
    enum class ThreadingStrategy
    {
        Async,
        ThreadPool,
        JThread
    };

    explicit MultiThreadProcessor(int numThreads, ThreadingStrategy strategy = ThreadingStrategy::ThreadPool);

    cv::Mat process(const cv::Mat &inputImage) override;

    std::vector<cv::Rect> divideImageIntoRegions(const cv::Mat &image);

private:
    int mNumThreads;
    ThreadingStrategy mStrategy;
    std::unique_ptr<ThreadPool> mThreadPool;

    void processRegion(cv::Mat &image, const cv::Rect &region);

    cv::Mat processWithThreadPool(const cv::Mat &inputImage);

    cv::Mat processWithAsync(const cv::Mat &inputImage);

    cv::Mat processWithJThreads(const cv::Mat &inputImage);
};
