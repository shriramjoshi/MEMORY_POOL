#include "memory_manager.hpp"

#include <iostream>
#include <cstdlib>

/////////////////////////////////// Private Functions //////////////////////////////////////////////////

memory_pool::ST_MEM_CHUNK* memory_pool::MemoryManager::createMemChunk(size_t size, size_t nmemb)
{
	memory_pool::ST_MEM_CHUNK* ptrNewChunk = (memory_pool::ST_MEM_CHUNK*)calloc(1, sizeof(memory_pool::ST_MEM_CHUNK));
	if(ptrNewChunk != nullptr)
	{
		ptrNewChunk->freeSlotsBitmap = (char*)calloc(nmemb, sizeof(char));
		ptrNewChunk->memBlock = (char*)calloc(nmemb, size);
		ptrNewChunk->ptrNextChunk = nullptr;
		ptrNewChunk->ptrPrevChunk = nullptr;
		return ptrNewChunk;
	}
	return nullptr;
}

memory_pool::ST_MEM_LIST_NODE* memory_pool::MemoryManager::createMemListNode(size_t size)
{
	memory_pool::ST_MEM_LIST_NODE* ptrListNode = (memory_pool::ST_MEM_LIST_NODE*)calloc(1, sizeof(memory_pool::ST_MEM_LIST_NODE));
	if(ptrListNode != nullptr)
	{
		ptrListNode->slot_size = size;
		ptrListNode->ptrChunk = nullptr;
		ptrListNode->ptrNextNode = nullptr;
		return ptrListNode;
	}
	return nullptr;
}

size_t memory_pool::MemoryManager::getBlockSize(size_t rBlockSize)
{
	if(rBlockSize < MEM_ALIGNMENT_BOUNDRY)
	{
		return MEM_ALIGNMENT_BOUNDRY;
	}
	if(rBlockSize % MEM_ALIGNMENT_BOUNDRY != 0)
	{
		size_t blockSize = MEM_ALIGNMENT_BOUNDRY;
		for(int i = 2; ; i++)
		{
			if(blockSize * i > rBlockSize)
			{
				return blockSize;
			}
			blockSize += MEM_ALIGNMENT_BOUNDRY;
		}	
	}
	return rBlockSize;
}

/////////////////////////////////// Public Functions //////////////////////////////////////////////////

memory_pool::MemoryManager::MemoryManager()
{
	ptrListHead = (memory_pool::ST_MEM_LIST*)calloc(1, sizeof(memory_pool::ST_MEM_LIST));
	ptrListHead->head = nullptr;
}

memory_pool::MemoryManager::~MemoryManager()
{
	
}

void* memory_pool::MemoryManager::mem_malloc(size_t rBlockSize, size_t count)
{
	int blockSize = memory_pool::MemoryManager::getBlockSize(rBlockSize);
	if(ptrListHead->head == nullptr)//first malloc request
	{
		memory_pool::ST_MEM_LIST_NODE* mem_list_node = memory_pool::MemoryManager::createMemListNode(blockSize);
		memory_pool::ST_MEM_CHUNK* mem_chunk = memory_pool::MemoryManager::createMemChunk(blockSize, count);
		ptrListHead->head = mem_list_node;
		mem_list_node->ptrChunk = mem_chunk;
		mem_list_node->ptrChunk->freeSlotsBitmap[0] = memory_pool::BUFFER_INFO::BUFFER_FILLED;
		return mem_chunk->memBlock;
	}
	return nullptr;
}

void memory_pool::MemoryManager::mem_free(void* ptr)
{
	
}
