#pragma once
#include <deque>
#include <vector>
#include <set>
#include <mutex>
#include <condition_variable>

template <class DATATYPE, template <class DATATYPE, class = std::allocator<DATATYPE>> class CONTAINER = std::vector>
class ThreadSafeBuffer
{
private:
	CONTAINER<DATATYPE> dataStore;
	mutable std::mutex stdMutex;
	std::condition_variable stdCondVar;
	size_t maxsize;

public:
	ThreadSafeBuffer() : maxsize(1000)
	{
		//dataStore.reserve(maxsize);
	}

	ThreadSafeBuffer(size_t size) : maxsize(size)
	{
		//dataStore.reserve(maxsize);
	}

	bool InsertData(DATATYPE data)
	{
		// don't remove this scope
		{
			std::lock_guard<std::mutex> lock(stdMutex);
			if (dataStore.size() < maxsize)
			{
				dataStore.push_back(data);
			}
			else
			{
				return false;
			}
		}
		stdCondVar.notify_one();
		return true;
	}

	DATATYPE FetchData()
	{
		std::unique_lock<std::mutex> lock(stdMutex);
		while (dataStore.empty())
		{
			stdCondVar.wait(lock);
		}
		DATATYPE popped_value;
		popped_value = dataStore.back();
		dataStore.pop_back();
		return popped_value;
	}

	bool NonBlockingFetch(DATATYPE& data)
	{
		std::lock_guard<std::mutex> lock(stdMutex);
		if (!dataStore.empty())
		{
			data = std::move(dataStore.back());
			dataStore.pop_back();
			return true;
		}
		return false;
	}

	int GetSize()
	{
		std::lock_guard<std::mutex> lock(stdMutex);
		return dataStore.size();
	}

	bool IsEmpty()
	{
		std::lock_guard<std::mutex> lock(stdMutex);
		return dataStore.empty();
	}

	void Clear()
	{
		dataStore.clear();
	}

	~ThreadSafeBuffer()
	{
		dataStore.clear();
		printf("Threadsafebuffer cleared\n");
	}
};

#ifdef WINDOWS
template <typename DATATYPE, class COMP = std::less<>>
class ThreadSafeSet
{
private:
	std::set<DATATYPE, COMP> dataStore;
	mutable std::mutex stdMutex;
	std::condition_variable stdCondVar;
	long long maxsize;

public:
	ThreadSafeSet()
	{
	}

	ThreadSafeSet(long long size) : maxsize(size)
	{
	}

	void InsertData(DATATYPE data)
	{
		// don't remove this scope
		{
			std::lock_guard<std::mutex> lock(stdMutex);
			dataStore.insert(data);
		}
		stdCondVar.notify_one();
	}

	DATATYPE FetchData()
	{
		std::unique_lock<std::mutex> lock(stdMutex);
		while (dataStore.empty())
		{
			stdCondVar.wait(lock);
		}
		DATATYPE popped_value;
		popped_value = *dataStore.begin();
		dataStore.erase(dataStore.begin());
		return popped_value;
	}

	int GetSize()
	{
		std::lock_guard<std::mutex> lock(stdMutex);
		return dataStore.size();
	}

	bool IsEmpty()
	{
		std::lock_guard<std::mutex> lock(stdMutex);
		return dataStore.empty();
	}

	~ThreadSafeSet()
	{
		dataStore.clear();
	}
};
#endif
