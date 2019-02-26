#pragma once
#include <memory>
#include <functional>

namespace ELABuffer
{
	typedef struct BufferNode
	{
		char* data;
		size_t len;
		size_t offset;

		BufferNode() :
			data(nullptr),
			len(0),
			offset(0)
		{
		}
	} BufferNode;

	typedef std::function<BufferNode(size_t)> BufferNodeCreator;
	typedef std::function<void(BufferNode*)> BufferNodeDestroyer;
	typedef std::unique_ptr<BufferNode, BufferNodeDestroyer> UPBufferNode;

	UPBufferNode GetUniqueBlock(BufferNode* blocks);
	void FreeBuffer(BufferNode* blocks);
	BufferNode GetBuffer(size_t size);
};


namespace MemoryPool
{
	template <typename T>
	T* malloc()
	{
		return new T;
	}

	template <typename T>
	void free(T* data)
	{
		delete data;
	}
};
