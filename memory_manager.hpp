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
			void* mem_malloc(size_t, size_t = 1);
			void mem_free(void*);
			
		private:
			memory_pool::ST_MEM_CHUNK* createMemChunk(size_t, size_t = MAX_BLOCK_COUNT);
			memory_pool::ST_MEM_LIST_NODE* createMemListNode(size_t);
			size_t getBlockSize(size_t);
	};
};

#endif
