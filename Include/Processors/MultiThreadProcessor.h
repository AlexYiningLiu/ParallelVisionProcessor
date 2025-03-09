#pragma once
#include <Core/ImageProcessor.h>
#include <Utils/ThreadPool.h>

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

    cv::Mat process(const cv::Mat& inputImage) override;

    std::vector<cv::Rect> divideImageIntoRegions(const cv::Mat& image) const;

private:
    int mNumThreads;
    ThreadingStrategy mStrategy;
    std::unique_ptr<ThreadPool> mThreadPool;

    cv::Mat processWithThreadPool(const cv::Mat& inputImage);

    cv::Mat processWithAsync(const cv::Mat& inputImage);

    cv::Mat processWithJThreads(const cv::Mat& inputImage);
};
