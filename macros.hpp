#ifndef MACROS_HPP
#define MACROS_HPP

namespace memory_pool
{
	//multiple of 8
	//better to initialize it to multiple of (MEM_ALIGNMENT_BOUNDRY)
	#define MAX_BLOCK_COUNT 104
	#define MEM_ALIGNMENT_BOUNDRY 8
	enum BUFFER_INFO
	{
		BUFFER_EMPTY = 0,
		BUFFER_FILLED
	};
};

#endif
