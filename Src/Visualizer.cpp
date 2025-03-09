#include <Utils/Visualizer.h>
#include <iostream>
#include <random>

void Visualizer::displayImages(const cv::Mat& original, const cv::Mat& singleThread, const cv::Mat& multiThread,
                               const std::vector<cv::Rect>& regions)
{

    cv::Mat regionView = original.clone();
    displayProcessingRegions(regionView, regions);

    int width = original.cols;
    int height = original.rows;
    cv::Mat display(height * 2, width * 2, original.type());

    original.copyTo(display(cv::Rect(0, 0, width, height)));
    regionView.copyTo(display(cv::Rect(width, 0, width, height)));
    singleThread.copyTo(display(cv::Rect(0, height, width, height)));
    multiThread.copyTo(display(cv::Rect(width, height, width, height)));

    cv::putText(display, "Original Image", cv::Point(10, 30), cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(0, 255, 0), 2);
    cv::putText(display, "Processing Regions", cv::Point(width + 10, 30), cv::FONT_HERSHEY_SIMPLEX, 1.0,
                cv::Scalar(0, 255, 0), 2);
    cv::putText(display, "Single-Thread Result", cv::Point(10, height + 30), cv::FONT_HERSHEY_SIMPLEX, 1.0,
                cv::Scalar(0, 255, 0), 2);
    cv::putText(display, "Multi-Thread Result", cv::Point(width + 10, height + 30), cv::FONT_HERSHEY_SIMPLEX, 1.0,
                cv::Scalar(0, 255, 0), 2);

    cv::namedWindow("Image Processing Comparison", cv::WINDOW_NORMAL);
    cv::imshow("Image Processing Comparison", display);

    cv::imwrite("processing_comparison.png", display);
    std::cout << "Saved visualization to processing_comparison.png\n";

    cv::waitKey(0);
}

void Visualizer::displayProcessingRegions(cv::Mat& image, const std::vector<cv::Rect>& regions)
{

    cv::Mat overlay = image.clone();

    std::mt19937 rng(12345);
    std::uniform_int_distribution<int> dist(50, 255);

    for (const auto& region : regions)
    {

        cv::Scalar fillColor(dist(rng), dist(rng), dist(rng));

        cv::rectangle(overlay, region, fillColor, -1);
    }

    cv::addWeighted(overlay, 0.3, image, 0.7, 0, image);

    for (size_t i = 0; i < regions.size(); i++)
    {

        std::mt19937 regionRng(12345 + static_cast<unsigned int>(i));
        cv::Scalar borderColor(dist(regionRng), dist(regionRng), dist(regionRng));

        cv::rectangle(image, regions[i], borderColor, 3);

        cv::Point center(regions[i].x + regions[i].width / 2, regions[i].y + regions[i].height / 2);
        cv::putText(image, std::to_string(i + 1), center, cv::FONT_HERSHEY_SIMPLEX, 1.2, cv::Scalar(255, 255, 255), 3);
        cv::putText(image, std::to_string(i + 1), center, cv::FONT_HERSHEY_SIMPLEX, 1.2, borderColor, 2);
    }
}

void Visualizer::saveTimingChart(const std::string& filename, double singleThreadTime, double multiThreadTime,
                                 int numThreads)
{

    const int height = 400;
    const int width = 600;
    const int barWidth = 150;
    const int margin = 50;

    cv::Mat chart(height, width, CV_8UC3, cv::Scalar(255, 255, 255));

    double maxTime = std::max(singleThreadTime, multiThreadTime);
    double scale = (height - 2 * margin) / maxTime;

    int singleThreadHeight = static_cast<int>(singleThreadTime * scale);
    int multiThreadHeight = static_cast<int>(multiThreadTime * scale);

    cv::rectangle(chart, cv::Point(width / 3 - barWidth / 2, height - margin - singleThreadHeight),
                  cv::Point(width / 3 + barWidth / 2, height - margin), cv::Scalar(0, 0, 255), -1);

    cv::rectangle(chart, cv::Point(2 * width / 3 - barWidth / 2, height - margin - multiThreadHeight),
                  cv::Point(2 * width / 3 + barWidth / 2, height - margin), cv::Scalar(0, 255, 0), -1);

    cv::putText(chart, "Single Thread", cv::Point(width / 3 - 70, height - 20), cv::FONT_HERSHEY_SIMPLEX, 0.6,
                cv::Scalar(0, 0, 0), 1);

    cv::putText(chart, std::to_string(numThreads) + " Threads", cv::Point(2 * width / 3 - 60, height - 20),
                cv::FONT_HERSHEY_SIMPLEX, 0.6, cv::Scalar(0, 0, 0), 1);

    cv::putText(chart, std::to_string(singleThreadTime).substr(0, 6) + "s",
                cv::Point(width / 3 - 40, height - margin - singleThreadHeight - 10), cv::FONT_HERSHEY_SIMPLEX, 0.6,
                cv::Scalar(0, 0, 0), 1);

    cv::putText(chart, std::to_string(multiThreadTime).substr(0, 6) + "s",
                cv::Point(2 * width / 3 - 40, height - margin - multiThreadHeight - 10), cv::FONT_HERSHEY_SIMPLEX, 0.6,
                cv::Scalar(0, 0, 0), 1);

    double speedup = singleThreadTime / multiThreadTime;
    cv::putText(chart, "Speedup: " + std::to_string(speedup).substr(0, 4) + "x", cv::Point(width / 2 - 60, 30),
                cv::FONT_HERSHEY_SIMPLEX, 0.7, cv::Scalar(0, 0, 0), 2);

    cv::putText(chart, "Processing Time Comparison", cv::Point(width / 2 - 140, 70), cv::FONT_HERSHEY_SIMPLEX, 0.8,
                cv::Scalar(0, 0, 0), 2);

    cv::imwrite(filename, chart);
    std::cout << "Saved timing chart to " << filename << "\n";
}
