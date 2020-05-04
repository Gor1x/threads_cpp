#ifndef LAB_17_SYNC_QUEUE
#define LAB_17_SYNC_QUEUE

//#include <optional>
#include <cstddef>
#include <stdexcept>
#include <experimental/optional>
#include <queue>
#include <mutex>
#include <thread>
#include <condition_variable>

//Не надо пожалуйста меня банить за experimental, мой CLION отказывался видеть optional, что бы я не делал.

using std::experimental::optional;
using std::experimental::nullopt;

namespace lab_17 {
    /**
     * Exception thrown when queue is shut down.
     * Any operation that was waiting inside the queue or would modify it after shutdown is aborted with this exception.
     */
    class queue_is_shutdown {

    };

    /**
     * Synchronised queue for inter.
     * @tparam T stored type.
     */
    template<typename T>
    class sync_queue {
    public:
        /**
         * Construct queue with given maximum queue size (unlimited by default).
         * @param max_size maximum queue size.
         */
        explicit sync_queue(std::size_t max_size = -1);
        /**
         * Move construction.
         */
        sync_queue(sync_queue &&) noexcept;
        /**
         * Move assignment.
         */
        sync_queue &operator =(sync_queue &&) noexcept = default;

        /**
         * Destruction.
         */
        ~sync_queue() noexcept = default;

        /**
         * Copy construction is forbidden.
         */
        sync_queue(sync_queue const &) = delete;
        /**
         * Copy assignment is forbidden.
         */
        sync_queue &operator =(sync_queue const &) = delete;

        /**
         * Push value into the queue, wait if needed.
         * If queue was already shutdown or is shutdown during waiting, exception is thrown.
         * @param value value to be pushed (todo: references?)
         */
        void push(const T &value);
        /**
         * Push value into the queue only if there is space for it.
         * If queue was shutdown, operation is not performed.
         * @param x value to be pushed (todo: references?)
         * @return was operation successful or not.
         */
        bool try_push(const T &x);

        /**
         * Pop value from the queue, wait if needed.
         * If queue was already shutdown or is shutdown during waiting, exception is thrown.
         * @return popped value.
         */
        T pop();
        /**
         * Pop value from the queue only if there is one.
         * If queue was shutdown, operation is not performed
         * @return optional popped value.
         */
        optional<T> try_pop();

        /**
         * Current size of queue.
         * @return size.
         */
        std::size_t size() const noexcept;

        /**
         * Shutdown queue.
         * After queue is shutdown, nothing can be pushed or popped,
         * and all waiting operations are aborted.
         */
        void shutdown();

        /**
         * Check if queue is shutdown.
         * @return is queue shutdown.
         */
        bool is_shutdown() const noexcept;

    private:

        std::condition_variable mNotFull;
        std::condition_variable mNotEmpty;

        std::mutex queueMutex;
        std::queue<T> mQueue;
        size_t maxSize;
        bool mShutdowned = false;
    };

    template<typename T>
    sync_queue<T>::sync_queue(std::size_t max_size) : maxSize(max_size) {}

    template<typename T>
    bool sync_queue<T>::is_shutdown() const noexcept
    {
        return mShutdowned;
    }

    template<typename T>
    void sync_queue<T>::shutdown()
    {
        std::lock_guard<std::mutex> guard(queueMutex);

        if (is_shutdown())
            throw queue_is_shutdown();
        mShutdowned = true;
    }

    template<typename T>
    sync_queue<T>::sync_queue(sync_queue &&other) noexcept : mQueue(std::move(other.mQueue))
                , maxSize(other.maxSize), mShutdowned(other.mShutdowned) {}

    template<typename T>
    std::size_t sync_queue<T>::size() const noexcept
    {
        return mQueue.size();
    }

    template<typename T>
    optional<T> sync_queue<T>::try_pop()
    {
        std::lock_guard<std::mutex> guard(queueMutex);

        if (is_shutdown() || mQueue.empty())
            return nullopt;

        auto element = std::move(mQueue.front());
        mQueue.pop();

        mNotFull.notify_one();

        return element;
    }

    template<typename T>
    T sync_queue<T>::pop()
    {
        std::unique_lock<std::mutex> queueLock(queueMutex);

        mNotEmpty.wait(queueLock, [this]() {
            return mQueue.size() > 0;
        });

        if (is_shutdown())
            throw queue_is_shutdown();

        T element = std::move(mQueue.front());
        mQueue.pop();
        mNotFull.notify_one();

        return element;
    }

    template<typename T>
    bool sync_queue<T>::try_push(const T &x)
    {
        std::lock_guard<std::mutex> guard(queueMutex);

        if (is_shutdown() || mQueue.size() < maxSize)
            return false;

        mQueue.push(x);
        return true;
    }

    template<typename T>
    void sync_queue<T>::push(const T& value)
    {
        std::unique_lock<std::mutex> queueLock(queueMutex);

        mNotFull.wait(queueLock, [this]() {
            return mQueue.size() < maxSize;
        });

        if (is_shutdown())
            throw queue_is_shutdown();

        mQueue.push(value);

        mNotEmpty.notify_one();
    }

}

#endif