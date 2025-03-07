#include <Utils/Visualizer.h>
#include <iostream>
#include <random>

void Visualizer::displayImages(const cv::Mat &original, const cv::Mat &singleThread, const cv::Mat &multiThread,
                               const std::vector<cv::Rect> &regions)
{
    // Create a copy of the original to draw regions on
    cv::Mat regionView = original.clone();
    displayProcessingRegions(regionView, regions);

    // Create a composite image to display all results
    int width = original.cols;
    int height = original.rows;
    cv::Mat display(height * 2, width * 2, original.type());

    // Place images in a 2x2 grid
    original.copyTo(display(cv::Rect(0, 0, width, height)));
    regionView.copyTo(display(cv::Rect(width, 0, width, height)));
    singleThread.copyTo(display(cv::Rect(0, height, width, height)));
    multiThread.copyTo(display(cv::Rect(width, height, width, height)));

    // Add labels
    cv::putText(display, "Original Image", cv::Point(10, 30), cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(0, 255, 0), 2);
    cv::putText(display, "Processing Regions", cv::Point(width + 10, 30), cv::FONT_HERSHEY_SIMPLEX, 1.0,
                cv::Scalar(0, 255, 0), 2);
    cv::putText(display, "Single-Thread Result", cv::Point(10, height + 30), cv::FONT_HERSHEY_SIMPLEX, 1.0,
                cv::Scalar(0, 255, 0), 2);
    cv::putText(display, "Multi-Thread Result", cv::Point(width + 10, height + 30), cv::FONT_HERSHEY_SIMPLEX, 1.0,
                cv::Scalar(0, 255, 0), 2);

    // Display the composite image
    cv::namedWindow("Image Processing Comparison", cv::WINDOW_NORMAL);
    cv::imshow("Image Processing Comparison", display);

    // Save the composite image
    cv::imwrite("processing_comparison.png", display);
    std::cout << "Saved visualization to processing_comparison.png\n";

    // Wait for key press
    cv::waitKey(0);
}

void Visualizer::displayProcessingRegions(cv::Mat &image, const std::vector<cv::Rect> &regions)
{
    // First create a semi-transparent overlay to make regions more visible
    cv::Mat overlay = image.clone();

    // Create consistent random colors with higher contrast
    std::mt19937 rng(12345); // Fixed seed for reproducibility
    std::uniform_int_distribution<int> dist(50, 255);

    // Draw regions with filled rectangles first for better visibility
    for (const auto region : regions)
    {
        // Generate random but visually distinct color for each region
        cv::Scalar fillColor(dist(rng), dist(rng), dist(rng));

        // Draw filled rectangle with semi-transparency
        cv::rectangle(overlay, region, fillColor, -1); // -1 means filled
    }

    // Blend the overlay with the original
    cv::addWeighted(overlay, 0.3, image, 0.7, 0, image);

    // Now draw the boundaries and numbers
    for (size_t i = 0; i < regions.size(); i++)
    {
        // Generate consistent color (using same seed)
        std::mt19937 regionRng(12345 + static_cast<unsigned int>(i));
        cv::Scalar borderColor(dist(regionRng), dist(regionRng), dist(regionRng));

        // Draw rectangle border with thicker line
        cv::rectangle(image, regions[i], borderColor, 3);

        // Add region number with larger font
        cv::Point center(regions[i].x + regions[i].width / 2, regions[i].y + regions[i].height / 2);
        cv::putText(image, std::to_string(i + 1), center, cv::FONT_HERSHEY_SIMPLEX, 1.2, cv::Scalar(255, 255, 255), 3);
        cv::putText(image, std::to_string(i + 1), center, cv::FONT_HERSHEY_SIMPLEX, 1.2, borderColor, 2);
    }
}

void Visualizer::saveTimingChart(const std::string &filename, double singleThreadTime, double multiThreadTime,
                                 int numThreads)
{
    // Set up a simple chart to visualize timing
    const int height = 400;
    const int width = 600;
    const int barWidth = 150;
    const int margin = 50;

    cv::Mat chart(height, width, CV_8UC3, cv::Scalar(255, 255, 255));

    // Determine the maximum time for scaling
    double maxTime = std::max(singleThreadTime, multiThreadTime);
    double scale = (height - 2 * margin) / maxTime;

    // Draw bars
    int singleThreadHeight = static_cast<int>(singleThreadTime * scale);
    int multiThreadHeight = static_cast<int>(multiThreadTime * scale);

    cv::rectangle(chart, cv::Point(width / 3 - barWidth / 2, height - margin - singleThreadHeight),
                  cv::Point(width / 3 + barWidth / 2, height - margin), cv::Scalar(0, 0, 255), -1);

    cv::rectangle(chart, cv::Point(2 * width / 3 - barWidth / 2, height - margin - multiThreadHeight),
                  cv::Point(2 * width / 3 + barWidth / 2, height - margin), cv::Scalar(0, 255, 0), -1);

    // Add labels
    cv::putText(chart, "Single Thread", cv::Point(width / 3 - 70, height - 20), cv::FONT_HERSHEY_SIMPLEX, 0.6,
                cv::Scalar(0, 0, 0), 1);

    cv::putText(chart, std::to_string(numThreads) + " Threads", cv::Point(2 * width / 3 - 60, height - 20),
                cv::FONT_HERSHEY_SIMPLEX, 0.6, cv::Scalar(0, 0, 0), 1);

    // Add time values
    cv::putText(chart, std::to_string(singleThreadTime).substr(0, 6) + "s",
                cv::Point(width / 3 - 40, height - margin - singleThreadHeight - 10), cv::FONT_HERSHEY_SIMPLEX, 0.6,
                cv::Scalar(0, 0, 0), 1);

    cv::putText(chart, std::to_string(multiThreadTime).substr(0, 6) + "s",
                cv::Point(2 * width / 3 - 40, height - margin - multiThreadHeight - 10), cv::FONT_HERSHEY_SIMPLEX, 0.6,
                cv::Scalar(0, 0, 0), 1);

    // Add speedup information
    double speedup = singleThreadTime / multiThreadTime;
    cv::putText(chart, "Speedup: " + std::to_string(speedup).substr(0, 4) + "x", cv::Point(width / 2 - 60, 30),
                cv::FONT_HERSHEY_SIMPLEX, 0.7, cv::Scalar(0, 0, 0), 2);

    // Add title
    cv::putText(chart, "Processing Time Comparison", cv::Point(width / 2 - 140, 70), cv::FONT_HERSHEY_SIMPLEX, 0.8,
                cv::Scalar(0, 0, 0), 2);

    // Save the chart
    cv::imwrite(filename, chart);
    std::cout << "Saved timing chart to " << filename << "\n";
}
