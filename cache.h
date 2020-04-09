#pragma once
#include <mutex>
#include <map>
#include <chrono>
#include <vector>
#include <algorithm>
#include <thread>
#include <list>

template <typename T, typename Key,
          typename = typename std::enable_if<std::is_default_constructible<T>::value>::type>
class TimeBasedCache {
    using time = std::chrono::time_point<std::chrono::high_resolution_clock>;
public:
    TimeBasedCache(size_t size = 10, double time = 3 /*in ms*/):
        m_size(size),
        m_expirationTime(time)
    {}

    TimeBasedCache(const TimeBasedCache& other) = delete;

    ~TimeBasedCache()
    {
        if(m_daemon.joinable())
            m_daemon.join();
    }

    void set(Key key, T item)
    {
        if(m_times.size() == m_size)
            removeOldItem();

        auto now = std::chrono::high_resolution_clock::now();

        std::unique_lock<std::mutex> guardTime(m_timesMutex);
        m_times.push_back(now);
        guardTime.unlock();

        std::unique_lock<std::mutex> guardData(m_dataStoreMutex);
        m_dataStore.emplace(key, item);        
        m_dataStoreTimed.emplace(now, std::pair<Key, T>(key, item));
        guardData.unlock();

        if(!m_daemonActivated)
        {
            m_daemon = std::thread(&TimeBasedCache::daemonWithClock, this);
            m_daemonActivated = true;
        }
    }

    std::pair<bool, T> get(Key key)
    {
        if(m_dataStore.count(key))
            return std::pair<bool, T>(true, m_dataStore[key]);
        return std::pair<bool, T>(false, T());
    }

private:
    void removeOldItem()
    {
        std::lock_guard<std::mutex> guardTime(m_timesMutex);
        time oldTime = time(m_times.front());
        m_times.erase(m_times.begin());

        std::unique_lock<std::mutex> guardData(m_dataStoreMutex);
        m_dataStore.erase(m_dataStoreTimed[oldTime].first);
        m_dataStoreTimed.erase(oldTime);
        guardData.unlock();
    }

    void daemonWithClock()
    {
        while (!m_times.empty())
        {
            std::lock_guard<std::mutex> guardTime(m_timesMutex);
            for(auto& time: m_times)
            {
                auto now = std::chrono::high_resolution_clock::now();
                if(std::chrono::duration_cast<std::chrono::milliseconds>(now - time).count() >= m_expirationTime)
                {
                    std::lock_guard<std::mutex> guardData(m_dataStoreMutex);
                    m_dataStore.erase(m_dataStoreTimed[time].first);
                    m_dataStoreTimed.erase(time);
                }
            }
        }
        m_daemonActivated = false;
    }
private:
    size_t m_size;
    double m_expirationTime;
    std::mutex m_dataStoreMutex;
    std::mutex m_timesMutex;

    std::list<time> m_times;
    std::map<time, std::pair<Key, T>> m_dataStoreTimed;
    std::map<Key, T> m_dataStore;

    std::thread m_daemon;

    bool m_daemonActivated {false};
};
