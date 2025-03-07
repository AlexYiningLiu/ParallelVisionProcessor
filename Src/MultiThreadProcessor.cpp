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
        return processWithAsync(inputImage); // Fallback
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

    // Try to create a balanced grid
    int numRows = 0, numCols = 0;

    // Find factors of mNumThreads that are closest to each other
    // This creates the most square-like division possible
    for (numRows = static_cast<int>(std::sqrt(mNumThreads)); numRows >= 1; numRows--)
    {
        if (mNumThreads % numRows == 0)
        {
            numCols = mNumThreads / numRows;
            break;
        }
    }

    // If we couldn't find divisors, use a simple approach
    if (numRows == 0)
    {
        numRows = static_cast<int>(std::sqrt(mNumThreads));
        numCols = (mNumThreads + numRows - 1) / numRows; // Ceiling division
    }

    // Debug output to help diagnose issues
    std::cout << "Dividing into " << numRows << " rows and " << numCols << " columns" << std::endl;

    // Calculate region dimensions
    int regionWidth = image.cols / numCols;
    int regionHeight = image.rows / numRows;

    // Create regions
    for (int row = 0; row < numRows; row++)
    {
        for (int col = 0; col < numCols; col++)
        {
            if (regions.size() >= static_cast<size_t>(mNumThreads))
                return regions;

            int x = col * regionWidth;
            int y = row * regionHeight;

            // Handle the last column/row which might be smaller
            int width = (col == numCols - 1) ? (image.cols - x) : regionWidth;
            int height = (row == numRows - 1) ? (image.rows - y) : regionHeight;

            // Create region and add to list
            regions.emplace_back(x, y, width, height);
        }
    }

    return regions;
}

cv::Mat MultiThreadProcessor::processWithThreadPool(const cv::Mat &inputImage)
{
    cv::Mat outputImage = inputImage.clone();

    // Divide the image into regions
    std::vector<cv::Rect> regions = divideImageIntoRegions(outputImage);

    // Vector to store the futures
    std::vector<std::future<void>> results;
    results.reserve(regions.size());

    // Submit tasks to the thread pool
    for (const auto &region : regions)
    {
        results.push_back(mThreadPool->enqueue([this, &outputImage, region]() { processRegion(outputImage, region); }));
    }

    // Wait for all tasks to complete
    for (auto &result : results)
    {
        result.wait();
    }

    return outputImage;
}

cv::Mat MultiThreadProcessor::processWithAsync(const cv::Mat &inputImage)
{
    cv::Mat outputImage = inputImage.clone();

    // Divide the image into regions
    std::vector<cv::Rect> regions = divideImageIntoRegions(outputImage);

    // Create a vector to hold futures
    std::vector<std::future<void>> futures;
    futures.reserve(regions.size());

    // Process each region in a separate thread
    for (const auto &region : regions)
    {
        futures.push_back(
            std::async(std::launch::async, [this, &outputImage, region]() { processRegion(outputImage, region); }));
    }

    // Wait for all threads to complete
    for (auto &future : futures)
    {
        future.wait();
    }

    return outputImage;
}

cv::Mat MultiThreadProcessor::processWithJThreads(const cv::Mat &inputImage)
{
    cv::Mat outputImage = inputImage.clone();

    // Divide the image into regions
    std::vector<cv::Rect> regions = divideImageIntoRegions(outputImage);

    // Create a latch to wait for all threads to complete
    std::latch completionLatch(regions.size());

    // Vector to store the threads
    std::vector<std::jthread> threads;
    threads.reserve(regions.size());

    // Launch threads to process each region
    for (const auto &region : regions)
    {
        threads.emplace_back(
            [this, &outputImage, region, &completionLatch]()
            {
                // Process the region
                processRegion(outputImage, region);

                // Signal completion
                completionLatch.count_down();
            });
    }

    // Wait for all tasks to complete
    completionLatch.wait();

    // No need to manually join threads as std::jthread will join automatically on
    // destruction

    return outputImage;
}
