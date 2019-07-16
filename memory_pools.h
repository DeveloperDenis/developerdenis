#if !defined(MEMORY_POOLS_H_)
#define MEMORY_POOLS_H_

// TODO(denis): think of a better system to allocated and deallocate arbitrarily sized 
// memory blocks, that also reduces fragmentation

struct MemoryBlock
{
	// NOTE(denis): the size includes the size of the BlockHeader
	u32 size;
	MemoryBlock* next;
};

// a stack structure for allocating memory
struct MemoryPool
{
	u32 maxSize;
	u32 size;
	u8* base;
	
	// a flag that determines whether to use the block list structure
	// true = a simple stack wihout the fancy memory deallocation
	bool stackMode;
	MemoryBlock* blocks;
	MemoryBlock* freeBlocks;
};

static inline MemoryPool initMemoryPool(void* base, u32 maxSize, bool stackMode=false)
{
	MemoryPool pool = {};
	pool.maxSize = maxSize;
	pool.base = (u8*)base;
	pool.stackMode = stackMode;
	
	return pool;
}

static inline void clearMemoryPool(MemoryPool* pool)
{
	pool->size = 0;
	pool->blocks = 0;
	pool->freeBlocks = 0;
}

static void* pushBlock(MemoryPool* pool, u32 dataSize)
{
	void* resultBlock = 0;
	
	if (pool->stackMode)
	{
		ASSERT(pool->size + dataSize < pool->maxSize);
		
		resultBlock = pool->base + pool->size;
		pool->size += dataSize;
	}
	else
	{
		void* blockBase = 0;
		
		u32 allocSize = dataSize + sizeof(MemoryBlock);
		ASSERT(pool->size + allocSize < pool->maxSize);
		
		MemoryBlock* prevBlock = 0;
		MemoryBlock* currBlock = pool->freeBlocks;
		while(currBlock)
		{
			// TODO(denis): find the smallest possible block that is >= allocSize to help
			// reduce fragmentation? would that even help?
			if (currBlock->size <= allocSize)
			{
				blockBase = currBlock;
				
				// create a new block using the remaining memory that isn't needed
				// for this allocation, and stick it in the free list
				MemoryBlock* remainderBlock = currBlock->next;
				
				if (allocSize != currBlock->size)
				{
					remainderBlock = (MemoryBlock*)((u8*)currBlock + allocSize);
					remainderBlock->size = currBlock->size - allocSize;
					remainderBlock->next = currBlock->next;
				}
				
				if (prevBlock)
					prevBlock->next = remainderBlock;
				else
					pool->freeBlocks = remainderBlock;
				
				break;
			}
			
			currBlock = currBlock->next;
		}
		
		if (!blockBase)
		{
			blockBase = pool->base + pool->size;
			pool->size += allocSize;
		}
		
		MemoryBlock* block = (MemoryBlock*)blockBase;
		block->size = allocSize;
		
		block->next = pool->blocks;
		pool->blocks = block;
		
		resultBlock = (u8*)blockBase + sizeof(MemoryBlock);
	}
	
	return resultBlock;
}

static void freeBlock(MemoryPool* pool, void* block)
{
	ASSERT(!pool->stackMode);
	
	MemoryBlock* prevBlock = 0;
	MemoryBlock* currBlock = pool->blocks;
	while (currBlock)
	{
		void* dataPtr = (u8*)currBlock + sizeof(MemoryBlock);
		if (dataPtr == block)
		{
			if (prevBlock)
				prevBlock->next = currBlock->next;
			else
				pool->blocks = currBlock->next;
			
			currBlock->next = pool->freeBlocks;
			pool->freeBlocks = currBlock;
			
			return;
		}
		
		currBlock = currBlock->next;
	}
	
	ASSERT(!"Could not find block to free");
}

// NOTE(denis): not to be used lightly, to work properly this needs to be called on the same
// data size before allocating more memory
static inline void popBlock(MemoryPool* pool, u32 dataSize)
{
	ASSERT(pool->size > dataSize);
	pool->size -= dataSize;
}

#define pushStruct(poolPtr, type) (type*)pushBlock(poolPtr, sizeof(type))

#endif