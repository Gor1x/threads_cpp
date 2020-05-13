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

#endif //LAB_17_2_MAINHELPER_HPP
