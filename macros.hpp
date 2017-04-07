#ifndef MACROS_HPP
#define MACROS_HPP

namespace memory_pool
{
	//multiple of 8
	//better to initialize it to multiple of (MEM_ALIGNMENT_BOUNDRY)
	#define MAX_BLOCK_COUNT 8
	#define MEM_ALIGNMENT_BOUNDRY 8
	#define MAX_CHUNK_COUNT 8
	typedef unsigned int uint;
	enum BUFFER_INFO
	{
		BUFFER_EMPTY = 0,
		BUFFER_FILLED
	};
	
	#define NO_BUFFER_EMPTY -1
	
};

#endif
