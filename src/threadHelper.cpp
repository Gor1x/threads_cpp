#include "mainHelper.hpp"

threadHelper::ThreadsContainer::ThreadsContainer(const size_t maxWorkers, std::condition_variable &distributorVar) :
        collectorThread(std::thread(__guts::threadCollector, std::ref(*this))),
        distributor(distributorVar),
        MAX_WORKERS(maxWorkers)
{
    threads.resize(MAX_WORKERS);
    for (size_t i = 0; i < MAX_WORKERS; i++)
        freeThreads.insert(i);
}

bool threadHelper::ThreadsContainer::hasFreeThreads() const {
    return busyWorkers < MAX_WORKERS;
}


void threadHelper::ThreadsContainer::threadCollecting() {
    std::mutex collectorMutex;
    std::unique_lock<std::mutex> locker(collectorMutex);
    while (true)
    {
        workIsDoneCollector.wait(locker);

        while (freeNumbers.size() > 0)
        {
            auto current = freeNumbers.pop();
            threads[current].join();
            freeThreads.insert(current);
            busyWorkers--;
        }
        distributor.notify_one();
    }
}

threadHelper::ThreadsContainer::~ThreadsContainer() noexcept {
    endTime = true;
    workIsDoneCollector.notify_one();
}

void threadHelper::__guts::threadCollector(threadHelper::ThreadsContainer &cont) {
    cont.threadCollecting();
}

void threadHelper::ThreadsContainer::operationEnded(size_t number)
{
    freeNumbers.push(number);
}
