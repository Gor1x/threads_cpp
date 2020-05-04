#include <mainHelper.hpp>
#include <iostream>

mainHelper::MyTimer::MyTimer()
{
    start = std::chrono::system_clock::now();
}

size_t mainHelper::MyTimer::getTime() {
    if (taken)
        throw std::logic_error("Out time is already taken");

    taken = true;
    end = std::chrono::system_clock::now();
    int elapsed_seconds = std::chrono::duration_cast<std::chrono::milliseconds>
            (end-start).count();
    return elapsed_seconds;
}

void mainHelper::MyTimer::print()
{
    std::cout << "Done in " << getTime() << " ms" << std::endl;
}

void mainHelper::Result::print() {
    std::cout << "Task " << task.id << " done in " << timeRes << " ms, found " << counter << " primes" << std::endl;
}

mainHelper::Result::Result(Task task, size_t counter) : task(task)
        , counter(counter) {
    timeRes = task.timer.getTime();
}

namespace mainHelper {
    std::istream& operator>>(std::istream &is, mainHelper::Task &task)
    {
        is >> task.id >> task.begin >> task.end;
        return is;
    }
}
