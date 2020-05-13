#include <iostream>

#include "mainHelper.hpp"

using namespace lab_17;
using namespace mainHelper;

typedef std::vector<std::thread> ThreadContainer;

static Result calc(Task &task, bool &timeToEnd)
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
        {
            if (timeToEnd)
            {
                throw queue_is_shutdown();
            }

            if (begin % j == 0)
            {
                isPrime = false;
                break;
            }
        }

        if (isPrime)
            counter++;
    }
    return {task, counter};
}

static void mainThreadCycle(sync_queue<Task> &tasks, sync_queue<Result> &results, bool &timeToEnd)
{
    while (true)
    {
        try
        {
            auto task = tasks.pop();
            results.push(calc(task, timeToEnd));
        }
        catch (queue_is_shutdown &e)
        {
            return;
        }
    }
}

static void
startThreads(ThreadContainer &threads, sync_queue<Task> &tasks, sync_queue<Result> &results, bool &timeToEnd)
{
    for (auto &thread : threads)
    {
        thread = std::thread(mainThreadCycle,
                             std::ref(tasks),
                             std::ref(results),
                             std::ref(timeToEnd));
    }
}

static void joinThreads(ThreadContainer &threads)
{
    for (auto &thread : threads)
    {
        thread.join();
    }
}

int main(int, char** argv)
{
    size_t maxQueueSize = std::stoull(argv[2]);
    size_t workers = std::stoull(argv[1]);

    sync_queue<Task> tasks(maxQueueSize);
    sync_queue<Result> results;

    ThreadContainer threads(workers);

    bool timeToEnd = false;
    startThreads(threads, tasks, results, timeToEnd);

    while (true)
    {
        std::string command;
        std::cin >> command;

        if (command == "submit")
        {
            MyTimer timer;

            Task task;
            std::cin >> task;

            if (tasks.size() == maxQueueSize)
            {
                std::cout << "Queue is full, operation can't be done" << std::endl;
                continue;
            }

            tasks.push(task);

            timer.print();
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

            timeToEnd = true;
            tasks.shutdown();
            results.shutdown();
            joinThreads(threads);

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