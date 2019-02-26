#pragma once
#include <queue>
#include <mutex>
#include <condition_variable>

template <class DATATYPE>
class ThreadSafeQueue
{
private:
	std::deque<DATATYPE> dataStore;
	mutable std::mutex stdMutex;
	std::condition_variable readwait, writewait;
	size_t maxsize;
	int count;
public:
	ThreadSafeQueue() : maxsize(1000)
	{
	}

	ThreadSafeQueue(size_t size) : maxsize(size)
	{
	}

	bool TryPush(DATATYPE data)
	{
		std::lock_guard<std::mutex> lock(stdMutex);
		if (dataStore.size() < maxsize)
		{
			dataStore.push(data);
			readwait.notify_one();
			return true;
		}
		return false;
	}

	bool Push(DATATYPE data)
	{
		std::unique_lock<std::mutex> lock(stdMutex);
		while (dataStore.size() >= maxsize)
		{
			writewait.wait(lock);
		}
		++count;
		data.offset = count;
		dataStore.push_back(data);
		readwait.notify_one();
		return true;
	}

	bool TryPop(DATATYPE& data)
	{
		std::lock_guard<std::mutex> lock(stdMutex);
		if (!dataStore.empty())
		{
			data = std::move(dataStore.back());
			dataStore.pop();
			writewait.notify_one();
			return true;
		}
		return false;
	}

	DATATYPE Pop()
	{
		std::unique_lock<std::mutex> lock(stdMutex);
		while (dataStore.empty())
		{
			readwait.wait(lock);
		}
		DATATYPE data = dataStore.front();
		dataStore.pop_front();
		writewait.notify_one();
		return data;
	}
};