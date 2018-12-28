#pragma once
#include <deque>
#include <mutex>
#include <condition_variable>



template<typename T>
class SharedQueue
{
public:
	SharedQueue();
	~SharedQueue();

	T& front();
	void pop_front();

	void push_back(const T& item);

	int size();
	void clear();
	bool empty();

private:
	std::deque<T> m_queue;
	std::mutex m_mutex;
	std::condition_variable m_cond;
};


template<typename T>
inline SharedQueue<T>::SharedQueue()
{

}

template<typename T>
inline SharedQueue<T>::~SharedQueue()
{

}

template<typename T>
inline T & SharedQueue<T>::front()
{
	std::unique_lock<std::mutex> lock(m_mutex);
	while (m_queue.empty())
	{
		m_cond.wait(lock);
	}

	return m_queue.front();
}

template<typename T>
inline void SharedQueue<T>::pop_front()
{
	std::unique_lock<std::mutex> lock(m_mutex);
	while (m_queue.empty())
	{
		m_cond.wait(lock);
	}

	m_queue.pop_front();
}

template<typename T>
inline void SharedQueue<T>::push_back(const T & item)
{
	std::unique_lock<std::mutex> lock(m_mutex);
	m_queue.push_back(item);
	lock.unlock();
	m_cond.notify_one();
}

template<typename T>
inline int SharedQueue<T>::size()
{
	std::unique_lock<std::mutex> lock(m_mutex);
	int size = m_queue.size();
	lock.unlock();
	return size;
}

template<typename T>
inline void SharedQueue<T>::clear()
{
	std::unique_lock<std::mutex> lock(m_mutex);
	m_queue.clear();
	lock.unlock();
}

template<typename T>
inline bool SharedQueue<T>::empty()
{
	bool result = false;

	std::unique_lock<std::mutex> lock(m_mutex);
	auto size = m_queue.size();

	if (size == 0)
	{
		result = true;
	}

	lock.unlock();
	return result;

}
