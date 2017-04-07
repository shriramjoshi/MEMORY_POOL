#ifndef MEMEORY_MANAGER_HPP
#define MEMEORY_MANAGER_HPP

#include "ds.hpp"
#include "macros.hpp"

namespace memory_pool
{
	class MemoryManager
	{
		private:
			ST_MEM_LIST* ptrListHead;
			
		public:
			MemoryManager();
			~MemoryManager();
			void* mem_alloc(size_t, size_t = MAX_BLOCK_COUNT);
			void mem_free(void*);
			
		private:
			memory_pool::ST_MEM_CHUNK* createMemChunk(size_t, size_t);
			memory_pool::ST_MEM_LIST_NODE* createMemListNode(size_t, size_t);
			memory_pool::ST_MEM_LIST_NODE* getListNode(void*);
			memory_pool::ST_MEM_LIST_NODE* getListNode(size_t);
			memory_pool::ST_MEM_CHUNK* getChunkByIndex(memory_pool::ST_MEM_LIST_NODE*, int index);
			size_t getBlockSize(size_t);
			memory_pool::ST_MEM_LIST_NODE* indexFromAddress(long*, char*);
			char* addressFromIndex(void* blockBaseAddress, long index, size_t blockSize);
			void poolRelease(void);
			void listRelease(memory_pool::ST_MEM_LIST_NODE*);
			void chunkRelease(memory_pool::ST_MEM_CHUNK*);
			int getFreeSlotIndex(memory_pool::ST_MEM_LIST_NODE*, int*);
	};
};

#endif
