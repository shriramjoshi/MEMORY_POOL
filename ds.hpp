#ifndef DS_HPP
#define DS_HPP

#include <iostream>

namespace memory_pool
{
	typedef struct _st_mem_list_node ST_MEM_LIST_NODE;
	typedef struct _st_mem_list ST_MEM_LIST;
	typedef struct _st_mem_chunk ST_MEM_CHUNK;
	
	struct _st_mem_list_node
	{
		size_t slot_size;
		ST_MEM_CHUNK* ptrChunk;
		ST_MEM_LIST_NODE* ptrNextNode;
	};
	
	struct _st_mem_list
	{
		ST_MEM_LIST_NODE* head;
	};
	
	struct _st_mem_chunk
	{
		//free mem blocks index
		char* freeSlotsBitmap;
		char* memBlock;
		ST_MEM_CHUNK* ptrNextChunk;
		ST_MEM_CHUNK* ptrPrevChunk;
	};
};

#endif
