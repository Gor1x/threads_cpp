#include "sync_queue.hpp"
#include <iostream>

using namespace std;
using namespace lab_17;


class TalkingInteger
{
public:
    explicit TalkingInteger(int a)
    {
        data = a;
        cout << "Created" << " " << data << endl;
    }

    TalkingInteger(const TalkingInteger &other)
    {
        data = other.data;
        cout << "Скопировался " << data << endl;
    }

    TalkingInteger(TalkingInteger &&other) noexcept
    {
        data = other.data;

        other.data = 0;

        cout << "Смувился!!" << " " << data << endl;
    }

    virtual ~TalkingInteger()
    {
        cout << "Закончил существование " << data << endl;
    }

private:
    int data = 0;
};


int main()
{
    vector<int> a;
    sync_queue<TalkingInteger> myque;
    myque.push(TalkingInteger(5));
    auto b = TalkingInteger(8);
    myque.push(std::forward<TalkingInteger>(b));

    auto element = myque.pop();
    auto element2 = myque.pop();
}