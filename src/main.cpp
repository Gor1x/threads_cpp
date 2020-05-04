#include <iostream>
#include <set>
#include "mainHelper.hpp"

using namespace lab_17;
using namespace mainHelper;
using namespace threadHelper;

void calc(std::condition_variable& workIsDone, sync_queue<Result> &results, Task task, ThreadsContainer& cont, int number)
{
    auto begin = task.begin;
    auto end = task.end;
    size_t counter = 0;

    for (; begin < end; begin++)
    {
        if (begin <= 1)
            continue;
        bool isPrime = true;
        for (size_t j = 2; j * j <= begin; j++)
            if (begin % j == 0)
            {
                isPrime = false;
                break;
            }
        if (isPrime)
            counter++;
    }

    results.push({task, counter});
    cont.operationEnded(number);
    workIsDone.notify_one();
}

void distribute(sync_queue<Task> &tasks, sync_queue<Result> &results, size_t workers, std::condition_variable &startWork,
           bool &mustEnd)
{
    ThreadsContainer container(workers, startWork);

    std::mutex distributeMutex;
    while (true)
    {
        std::unique_lock<std::mutex> locker(distributeMutex);
        startWork.wait(locker);

        if (mustEnd)
        {
            container.shutdown();
            break;
        }

        while (tasks.size() > 0 && container.hasFreeThreads())
        {
            container.giveTask(tasks.pop(), results, calc);
        }
    }
}

int main(int, char** argv)
{
    size_t maxQueueSize = std::stoi(argv[2]);
    size_t workers = std::stoi(argv[1]);

    sync_queue<Task> tasks(maxQueueSize);
    sync_queue<Result> results;

    std::condition_variable distributorStarter;
    bool distributeEndFlag = false;

    std::thread distributorThread(std::thread(distribute, std::ref(tasks)
            , std::ref(results)
            , workers
            , std::ref(distributorStarter)
            , std::ref(distributeEndFlag)));

    while (true)
    {
        std::string command;
        std::cin >> command;

        if (command == "submit")
        {
            MyTimer timer;
            Task task;
            std::cin >> task;
            tasks.push(task);
            distributorStarter.notify_one();
        }
        else if (command == "show-done")
        {
            MyTimer timer;
            while (results.size() > 0)
            {
                results.pop().print();
            }
            timer.print();
        }
        else if (command == "quit")
        {
            MyTimer timer;
            distributeEndFlag = true;
            distributorStarter.notify_one();
            distributorThread.detach();
            timer.print();
            break;
        }
        else
        {
            std::cout << "Unknown command: " << command << std::endl;
        }
    }
    return 0;
}