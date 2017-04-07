#include "memory_manager.hpp"

#include <iostream>
#include <cstdlib>

/////////////////////////////////// Private Functions //////////////////////////////////////////////////

memory_pool::ST_MEM_CHUNK* memory_pool::MemoryManager::createMemChunk(size_t size, size_t nmemb)
{
	memory_pool::ST_MEM_CHUNK* ptrNewChunk = (memory_pool::ST_MEM_CHUNK*)calloc(1, sizeof(memory_pool::ST_MEM_CHUNK));
	if(ptrNewChunk != nullptr)
	{
		ptrNewChunk->memBlock = (char*)calloc(nmemb, sizeof(char) * size);
		ptrNewChunk->ptrNextChunk = nullptr;
		return ptrNewChunk;
	}
	return nullptr;
}

memory_pool::ST_MEM_LIST_NODE* memory_pool::MemoryManager::createMemListNode(size_t size, size_t nmemb)
{
	memory_pool::ST_MEM_LIST_NODE* ptrListNode = (memory_pool::ST_MEM_LIST_NODE*)calloc(1, sizeof(memory_pool::ST_MEM_LIST_NODE));
	if(ptrListNode != nullptr)
	{
		ptrListNode->slot_size = size;
		ptrListNode->freeSlotsBitmap = (char*)calloc(MAX_CHUNK_COUNT, nmemb);
		ptrListNode->chunkStartAddress = (long int*)calloc(MAX_CHUNK_COUNT, sizeof(long int));
		ptrListNode->ptrChunk = nullptr;
		ptrListNode->ptrNextNode = nullptr;
		return ptrListNode;
	}
	return nullptr;
}

memory_pool::ST_MEM_LIST_NODE* memory_pool::MemoryManager::getListNode(void* addr)
{
	memory_pool::ST_MEM_LIST_NODE* node = ptrListHead->head;
	while(node != nullptr)
	{
		long int* chunkAddress = node->chunkStartAddress;
		
		for(int i = 0; i < MAX_CHUNK_COUNT; ++i)
		{
			if(chunkAddress[i] <= ((long int)addr) && ((long int)addr) <= (chunkAddress[i] + (long int)(node->slot_size * MAX_BLOCK_COUNT)))
				return node;
		}
		node = node->ptrNextNode;
	}
	return nullptr;
}

memory_pool::ST_MEM_LIST_NODE* memory_pool::MemoryManager::getListNode(size_t size)
{
	memory_pool::ST_MEM_LIST_NODE* node = ptrListHead->head;
	while(node != nullptr)
	{
		if(node->slot_size == size)
			return node;
		node = node->ptrNextNode;
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
		while(blockSize < rBlockSize)
		{
			blockSize += MEM_ALIGNMENT_BOUNDRY;
		}
		return blockSize;
	}
	return rBlockSize;
}

memory_pool::ST_MEM_LIST_NODE* memory_pool::MemoryManager::indexFromAddress(long* physicalIndex, char* addr)
{
	memory_pool::ST_MEM_LIST_NODE* node = memory_pool::MemoryManager::getListNode(addr);
	long int* chunkAddress = node->chunkStartAddress;
	long baseAddress = chunkAddress[0];
	size_t i = 0;
	for(; i < node->batchCount - 1; ++i)
	{
		if(chunkAddress[i] != 0)
		{
			if(chunkAddress[i] <= ((long)addr) && ((long)addr) < (long)chunkAddress[i + 1])
			{
				baseAddress = chunkAddress[i];
				break;
			}
		}
	}
	long logicalIndex = (((long)(addr - baseAddress)) / MEM_ALIGNMENT_BOUNDRY);
	*physicalIndex = (MAX_BLOCK_COUNT * i) + logicalIndex;
	return node;
}

char* memory_pool::MemoryManager::addressFromIndex(void* blockBaseAddress, long index, size_t blockSize)
{
	return (char*)blockBaseAddress + (blockSize * index);
}

void memory_pool::MemoryManager::poolRelease(void)
{
	memory_pool::ST_MEM_LIST_NODE* lNode = ptrListHead->head;
	if(lNode != nullptr)
	{
		memory_pool::MemoryManager::listRelease(lNode);
		free(ptrListHead);
	}
}

void memory_pool::MemoryManager::listRelease(memory_pool::ST_MEM_LIST_NODE* node)
{
	if(node->ptrNextNode != nullptr)
	{
		memory_pool::MemoryManager::listRelease(node->ptrNextNode);
	}
	memory_pool::MemoryManager::chunkRelease(node->ptrChunk);
	free(node->freeSlotsBitmap);
	free(node->chunkStartAddress);
	free(node);
}
void memory_pool::MemoryManager::chunkRelease(memory_pool::ST_MEM_CHUNK* chunk)
{
	if(chunk->ptrNextChunk != nullptr)
	{
		memory_pool::MemoryManager::chunkRelease(chunk->ptrNextChunk);
	}
	free(chunk->memBlock);
	free(chunk);
}

int memory_pool::MemoryManager::getFreeSlotIndex(memory_pool::ST_MEM_LIST_NODE* listNode, int* chunkIndex)
{
	char* freeSlotBitmap = listNode->freeSlotsBitmap;
	int chunks = listNode->batchCount;
	for(int i = 0; i < chunks; ++i)
	{
		for(int j = 0; j < MAX_BLOCK_COUNT; ++j)
		{
			if(freeSlotBitmap[j] == memory_pool::BUFFER_INFO::BUFFER_EMPTY)
			{
				*chunkIndex = i;
				return j;
			}
		}
	}
	return NO_BUFFER_EMPTY;
}

memory_pool::ST_MEM_CHUNK* memory_pool::MemoryManager::getChunkByIndex(memory_pool::ST_MEM_LIST_NODE* listNode, int index)
{
	memory_pool::ST_MEM_CHUNK* chunk = listNode->ptrChunk;
	int i = 0;
	while(i < index)
	{
		chunk = chunk->ptrNextChunk;
		i++;
	}
	return chunk;
}

/////////////////////////////////// Public Functions //////////////////////////////////////////////////

memory_pool::MemoryManager::MemoryManager()
{
	ptrListHead = (memory_pool::ST_MEM_LIST*)calloc(1, sizeof(memory_pool::ST_MEM_LIST));
	ptrListHead->head = nullptr;
}

memory_pool::MemoryManager::~MemoryManager()
{
	memory_pool::MemoryManager::poolRelease();
}

void* memory_pool::MemoryManager::mem_alloc(size_t rBlockSize, size_t count)
{
	int blockSize = memory_pool::MemoryManager::getBlockSize(rBlockSize);
	if(ptrListHead->head == nullptr)//first malloc request
	{
		memory_pool::ST_MEM_LIST_NODE* memListNode = memory_pool::MemoryManager::createMemListNode(blockSize, count);
		memory_pool::ST_MEM_CHUNK* memChunk = memory_pool::MemoryManager::createMemChunk(blockSize, count);
		ptrListHead->head = memListNode;
		memListNode->ptrChunk = memChunk;
		memListNode->freeSlotsBitmap[0] = memory_pool::BUFFER_INFO::BUFFER_FILLED;
		memListNode->chunkStartAddress[memListNode->batchCount++] = (long int)memChunk->memBlock;
		return memChunk->memBlock;
	}
	else {
		memory_pool::ST_MEM_LIST_NODE* listNode = memory_pool::MemoryManager::getListNode(blockSize);
		if(listNode != nullptr)
		{
			int chunkIndex = 0;
			int blockIndex = memory_pool::MemoryManager::getFreeSlotIndex(listNode, &chunkIndex);
			if(blockIndex != NO_BUFFER_EMPTY)
			{
				memory_pool::ST_MEM_CHUNK* memChunk = memory_pool::MemoryManager::getChunkByIndex(listNode, chunkIndex);
				void* addr = (void*)memory_pool::MemoryManager::addressFromIndex(memChunk->memBlock, blockIndex, listNode->slot_size);
				long index = 0l;
				memory_pool::MemoryManager::indexFromAddress(&index, (char*)addr);
				listNode->freeSlotsBitmap[index] = memory_pool::BUFFER_INFO::BUFFER_FILLED;
				listNode->batchCount++;
				return addr;
			}
			else
			{
				//NO EMPTY SLOT IN ANY CHUNK
				//CREATE NEW CHUNK
				//TODO
			}
		}
		else
		{
			memory_pool::ST_MEM_LIST_NODE* memListNode = memory_pool::MemoryManager::createMemListNode(blockSize, count);
			memory_pool::ST_MEM_CHUNK* memChunk = memory_pool::MemoryManager::createMemChunk(blockSize, count);
			memListNode->ptrNextNode = ptrListHead->head;
			ptrListHead->head = memListNode;
			memListNode->ptrChunk = memChunk;
			memListNode->freeSlotsBitmap[0] = memory_pool::BUFFER_INFO::BUFFER_FILLED;
			memListNode->chunkStartAddress[memListNode->batchCount++] = (long int)memChunk->memBlock;
			return memChunk->memBlock;
		}
	}
	
	return nullptr;
}

void memory_pool::MemoryManager::mem_free(void* ptr)
{
	memory_pool::ST_MEM_LIST_NODE* node;
	long index = 0l;
	node = memory_pool::MemoryManager::indexFromAddress(&index, (char*)ptr);
	node->freeSlotsBitmap[index] = memory_pool::BUFFER_INFO::BUFFER_EMPTY;
}
