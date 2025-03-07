#pragma once
#include <chrono>
#include <mutex>
#include <string>
#include <unordered_map>

class PerformanceMetrics
{
public:
    PerformanceMetrics() = default;

    void startTimer(const std::string &name);

    void stopTimer(const std::string &name);

    double getElapsedTime(const std::string &name) const;

    double calculateSpeedup(const std::string &baseline, const std::string &comparison) const;

    double calculateEfficiency(const std::string &baseline, const std::string &comparison, int numThreads) const;

    void printMetrics(int numThreads) const;

private:
    std::chrono::time_point<std::chrono::high_resolution_clock> mStartTime;
    std::unordered_map<std::string, double> mElapsedTimes;
    mutable std::mutex mMutex;
};
