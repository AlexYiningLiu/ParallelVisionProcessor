#include <Processors/MultiThreadProcessor.h>

MultiThreadProcessor::MultiThreadProcessor(int numThreads, ThreadingStrategy strategy)
    : mNumThreads(numThreads), mStrategy(strategy)
{
    if (strategy == ThreadingStrategy::ThreadPool)
    {
        mThreadPool = std::make_unique<ThreadPool>(numThreads);
    }
}

cv::Mat MultiThreadProcessor::process(const cv::Mat &inputImage)
{
    switch (mStrategy)
    {
    case ThreadingStrategy::ThreadPool:
        return processWithThreadPool(inputImage);

    case ThreadingStrategy::Async:
        return processWithAsync(inputImage);

    case ThreadingStrategy::JThread:
        return processWithJThreads(inputImage);

    default:
        return processWithAsync(inputImage);
    }
}

void MultiThreadProcessor::processRegion(cv::Mat &image, const cv::Rect &region)
{
    applyHeavyFilter(image, region);
}

std::vector<cv::Rect> MultiThreadProcessor::divideImageIntoRegions(const cv::Mat &image)
{
    std::vector<cv::Rect> regions;
    regions.reserve(mNumThreads);

    int numRows = 0, numCols = 0;

    for (numRows = static_cast<int>(std::sqrt(mNumThreads)); numRows >= 1; numRows--)
    {
        if (mNumThreads % numRows == 0)
        {
            numCols = mNumThreads / numRows;
            break;
        }
    }

    if (numRows == 0)
    {
        numRows = static_cast<int>(std::sqrt(mNumThreads));
        numCols = (mNumThreads + numRows - 1) / numRows;
    }

    std::cout << "Dividing into " << numRows << " rows and " << numCols << " columns" << std::endl;

    int regionWidth = image.cols / numCols;
    int regionHeight = image.rows / numRows;

    for (int row = 0; row < numRows; row++)
    {
        for (int col = 0; col < numCols; col++)
        {
            if (regions.size() >= static_cast<size_t>(mNumThreads))
                return regions;

            int x = col * regionWidth;
            int y = row * regionHeight;

            int width = (col == numCols - 1) ? (image.cols - x) : regionWidth;
            int height = (row == numRows - 1) ? (image.rows - y) : regionHeight;

            regions.emplace_back(x, y, width, height);
        }
    }

    return regions;
}

cv::Mat MultiThreadProcessor::processWithThreadPool(const cv::Mat &inputImage)
{
    cv::Mat outputImage = inputImage.clone();

    std::vector<cv::Rect> regions = divideImageIntoRegions(outputImage);

    std::vector<std::future<void>> results;
    results.reserve(regions.size());

    for (const auto &region : regions)
    {
        results.push_back(mThreadPool->enqueue([this, &outputImage, region]() { processRegion(outputImage, region); }));
    }

    for (auto &result : results)
    {
        result.wait();
    }

    return outputImage;
}

cv::Mat MultiThreadProcessor::processWithAsync(const cv::Mat &inputImage)
{
    cv::Mat outputImage = inputImage.clone();

    std::vector<cv::Rect> regions = divideImageIntoRegions(outputImage);

    std::vector<std::future<void>> futures;
    futures.reserve(regions.size());

    for (const auto &region : regions)
    {
        futures.push_back(
            std::async(std::launch::async, [this, &outputImage, region]() { processRegion(outputImage, region); }));
    }

    for (auto &future : futures)
    {
        future.wait();
    }

    return outputImage;
}

cv::Mat MultiThreadProcessor::processWithJThreads(const cv::Mat &inputImage)
{
    cv::Mat outputImage = inputImage.clone();

    std::vector<cv::Rect> regions = divideImageIntoRegions(outputImage);

    std::latch completionLatch(regions.size());

    std::vector<std::jthread> threads;
    threads.reserve(regions.size());

    for (const auto &region : regions)
    {
        threads.emplace_back(
            [this, &outputImage, region, &completionLatch]()
            {
                processRegion(outputImage, region);

                completionLatch.count_down();
            });
    }

    completionLatch.wait();

    return outputImage;
}
