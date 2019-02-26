#include <ELABuffer.h>
#include <stdlib.h>
ELABuffer::BufferNode ELABuffer::GetBuffer(size_t size)
{
	BufferNode block;
	block.data = reinterpret_cast<char*>(calloc(size, 1));
	block.len = size;
	return block;
}

void ELABuffer::FreeBuffer(BufferNode* blocks)
{
	free(blocks->data);
	blocks->data = nullptr;
}

ELABuffer::UPBufferNode ELABuffer::GetUniqueBlock(BufferNode* blocks)
{
	return UPBufferNode(blocks, FreeBuffer);
}
