#pragma once
#include <condition_variable>
#include <functional>
#include <future>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

class ThreadPool
{
public:
    explicit ThreadPool(size_t numThreads);

    ~ThreadPool();

    // Updated reference formatting.
    ThreadPool(const ThreadPool&) = delete;
    ThreadPool& operator=(const ThreadPool&) = delete;

    template <class F, class... Args>
    auto enqueue(F&& f, Args&&... args) -> std::future<typename std::invoke_result<F, Args...>::type>
    {
        using return_type = typename std::invoke_result<F, Args...>::type;

        auto task = std::make_shared<std::packaged_task<return_type()>>(
            std::bind(std::forward<F>(f), std::forward<Args>(args)...));

        std::future<return_type> result = task->get_future();

        {
            std::unique_lock<std::mutex> lock(mQueueMutex);

            if (mStop)
            {
                throw std::runtime_error("Enqueue on stopped ThreadPool");
            }

            mTasks.emplace([task]() { (*task)(); });
        }

        mCondition.notify_one();
        return result;
    }

private:
    std::vector<std::thread> mWorkers;

    std::queue<std::function<void()>> mTasks;

    std::mutex mQueueMutex;
    std::condition_variable mCondition;
    bool mStop;
};
