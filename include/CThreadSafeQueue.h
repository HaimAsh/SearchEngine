//
// Created by haimash on 29/01/2026.
//

#ifndef SEARCHENGINE_CTHREADSAFEQUEUE_H
#define SEARCHENGINE_CTHREADSAFEQUEUE_H

#include <queue>
#include <mutex>
#include <condition_variable>

template <typename T>
class CThreadSafeQueue
{
public:
    void Push(T&& element)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_queue.push(std::move(element));
        m_cond.notify_one();
    }

    void SignalFinished()
    {
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            m_finished = true;
        }
        m_cond.notify_all(); // Wake up ALL waiting threads so they can exit
    }

    bool Pop(T& element)
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_cond.wait(lock, [this]() { return !m_queue.empty() || m_finished; });

        if (m_queue.empty() || m_finished)
        {
            return false;
        }

        element = std::move(m_queue.front());
        m_queue.pop();

        return true;
    }

private:
    std::queue<T> m_queue;
    std::mutex m_mutex;
    std::condition_variable m_cond;
    bool m_finished = false;
};

#endif //SEARCHENGINE_CTHREADSAFEQUEUE_H