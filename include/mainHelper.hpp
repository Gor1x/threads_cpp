#ifndef LAB_17_2_MAINHELPER_HPP
#define LAB_17_2_MAINHELPER_HPP

#include <vector>
#include <set>
#include "sync_queue.hpp"

namespace mainHelper
{
    class MyTimer
    {
        std::chrono::time_point<std::chrono::system_clock> start, end;
        bool taken = false;
    public:
        MyTimer();
        size_t getTime();
        void print();

    };

    struct Task
    {
        MyTimer timer;
        std::string id;
        unsigned long long begin = 0, end = 0;

        friend std::istream& operator >>(std::istream& is, Task& task);
    };

    struct Result
    {
    public:
        Result(Task task, size_t counter);

        void print();
    private:
        Task task;
        size_t counter = 0;
        size_t timeRes;
    };
}

namespace threadHelper
{
    class ThreadsContainer;
    namespace __guts
    {
        void threadCollector(ThreadsContainer &cont);
    }

    class ThreadsContainer
    {
    public:
        ThreadsContainer(size_t maxWorkers, std::condition_variable &distributorVar);

        bool hasFreeThreads() const;

        ~ThreadsContainer() noexcept;

        void operationEnded(size_t number);

        void shutdown();

        template<typename F>
        void giveTask(mainHelper::Task task, lab_17::sync_queue<mainHelper::Result> &results, F function) {
            auto number = *freeThreads.begin();
            busyWorkers++;
            freeThreads.erase(number);
            threads[number] = std::thread(function
                    , std::ref(workIsDoneCollector)
                    , std::ref(results)
                    , task
                    , std::ref(*this)
                    , number);
        }

    private:
        friend void __guts::threadCollector(ThreadsContainer &cont);

        void threadCollecting();

        std::thread collectorThread;
        std::condition_variable workIsDoneCollector;
        bool endTime = false;

        std::condition_variable &distributor;

        lab_17::sync_queue<int> freeNumbers;
        const size_t MAX_WORKERS;
        std::vector<std::thread> threads;
        size_t busyWorkers = 0;
        std::set<int> freeThreads;
    };


}

#endif //LAB_17_2_MAINHELPER_HPP
