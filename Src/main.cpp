#include <iostream>
#include <string>
#include <thread>

#include <Processors/MultiThreadProcessor.h>
#include <Processors/SingleThreadProcessor.h>
#include <Utils/PerformanceMetrics.h>
#include <Utils/Visualizer.h>

void printUsage(const char *programName)
{
    std::cout << "Usage: " << programName << " <image_path> [num_threads] [threading_strategy]\n";
    std::cout << "  <image_path>       : Path to the input image\n";
    std::cout << "  [num_threads]      : Number of threads to use (default: "
                 "number of CPU cores)\n";
    std::cout << "  [threading_strategy]: Threading strategy to use (default: "
                 "threadpool)\n";
    std::cout << "                        - async     : Use std::async\n";
    std::cout << "                        - threadpool: Use thread pool\n";
    std::cout << "                        - jthread   : Use std::jthread\n";
}

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        printUsage(argv[0]);
        return 1;
    }

    std::string imagePath = argv[1];
    auto numThreads = static_cast<int>(std::thread::hardware_concurrency());
    MultiThreadProcessor::ThreadingStrategy strategy = MultiThreadProcessor::ThreadingStrategy::ThreadPool;

    if (argc >= 3)
    {
        try
        {
            numThreads = std::stoi(argv[2]);
            if (numThreads <= 0)
            {
                std::cerr << "Error: Number of threads must be positive\n";
                return 1;
            }
        }
        catch (const std::exception &e)
        {
            std::cerr << "Error parsing number of threads: " << e.what() << "\n";
            return 1;
        }
    }

    if (argc >= 4)
    {
        std::string strategyArg = argv[3];
        if (strategyArg == "async")
        {
            strategy = MultiThreadProcessor::ThreadingStrategy::Async;
        }
        else if (strategyArg == "threadpool")
        {
            strategy = MultiThreadProcessor::ThreadingStrategy::ThreadPool;
        }
        else if (strategyArg == "jthread")
        {
            strategy = MultiThreadProcessor::ThreadingStrategy::JThread;
        }
        else
        {
            std::cerr << "Error: Unknown threading strategy: " << strategyArg << "\n";
            printUsage(argv[0]);
            return 1;
        }
    }

    std::cout << "Using " << numThreads << " threads with ";
    switch (strategy)
    {
    case MultiThreadProcessor::ThreadingStrategy::Async:
        std::cout << "std::async strategy\n";
        break;
    case MultiThreadProcessor::ThreadingStrategy::ThreadPool:
        std::cout << "thread pool strategy\n";
        break;
    case MultiThreadProcessor::ThreadingStrategy::JThread:
        std::cout << "std::jthread strategy\n";
        break;
    }

    cv::utils::logging::setLogLevel(cv::utils::logging::LOG_LEVEL_ERROR);

    cv::Mat inputImage = cv::imread(imagePath);
    if (inputImage.empty())
    {
        std::cerr << "Error: Could not open or find the image: " << imagePath << "\n";
        return 1;
    }

    std::cout << "Image loaded: " << imagePath << " (" << inputImage.cols << "x" << inputImage.rows << ")\n";

    SingleThreadProcessor singleProcessor;
    MultiThreadProcessor multiProcessor(numThreads, strategy);

    PerformanceMetrics metrics;

    std::cout << "Processing with single thread...\n";
    metrics.startTimer("SingleThread");
    cv::Mat singleThreadResult = singleProcessor.process(inputImage);
    metrics.stopTimer("SingleThread");

    std::cout << "Processing with " << numThreads << " threads...\n";
    metrics.startTimer("MultiThread");
    cv::Mat multiThreadResult = multiProcessor.process(inputImage);
    metrics.stopTimer("MultiThread");

    metrics.printMetrics(numThreads);

    auto regions = multiProcessor.divideImageIntoRegions(inputImage);

    Visualizer::saveTimingChart("timing_chart.png", metrics.getElapsedTime("SingleThread"),
                                metrics.getElapsedTime("MultiThread"), numThreads);

    Visualizer::displayImages(inputImage, singleThreadResult, multiThreadResult, regions);

    return 0;
}
