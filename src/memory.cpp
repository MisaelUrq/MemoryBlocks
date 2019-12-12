#include <stdlib.h>
#include <string.h>

struct MemoryBlock {
    size_t size;
    size_t count;
    size_t allocations;
    size_t block_count;
    void*  ptr;
    void*  start;
    const char*  label;
    MemoryBlock* next;
};

struct Memory {
    size_t        count;
    size_t        block_size;
    MemoryBlock*  blocks;
    MemoryBlock** allocators;
    int           allocator_count;
};

#define MEMORY_HEADERS 1

#if MEMORY_HEADERS
/*
  Defining headers makes each allocation have a small header, with
  information such as the size of the blocks, and it's position in the
  memory block, all of them are pack in the same block, so memory
  blocks are a little smaller.

  You can use GetNextHeader to get the header next to the one you
  pass, if nullptr is pass to this then you get the first header.
*/
struct MemoryHeader {
    size_t size;
    void*  ptr;
};
#endif

bool InitMemoryBlock(MemoryBlock* block, size_t size)
{
    block->size  = size;

#if MEMORY_HEADERS
    block->count = sizeof(MemoryHeader);
#else
    block->count = 0;
#endif

    block->allocations = 0;
    block->next  = nullptr;
    block->start = calloc(1, size);
    block->ptr   = block->start;
    if (block->start) {
        return true;
    } else {
        return false;
    }
}

bool InitMemory(Memory* memory, size_t size, size_t count)
{
    memory->count  = count;
    memory->allocator_count = -1;
    memory->blocks = (MemoryBlock*)calloc(count, sizeof(MemoryBlock));
    memory->allocators = (MemoryBlock**)calloc(count, sizeof(MemoryBlock*));
    memory->block_size = size;
    if (memory->blocks) {
        return true;
    } else {
        return false;
    }
}

void FreeMemoryBlock(MemoryBlock* block)
{
    free(block->start);
    *block = {};
}

void FreeMemory(Memory* memory)
{
    for (size_t i = 0; i < memory->count; i++) {
        if (memory->blocks[i].start) {
            FreeMemoryBlock(&memory->blocks[i]);
        }
    }
    free(memory->blocks);
    *memory = {};
}

size_t GetKey(const char* msg, size_t size) {
    size_t result = 0;
    for (size_t i = 0; i < size; i++) {
        result += msg[i];
    }
    return result;
}

inline MemoryBlock* GetMemoryBlock(Memory* memory) {
    return memory->allocators[memory->allocator_count];
}

inline MemoryBlock* GetMemoryBlockByIndex(Memory* memory, size_t index) {
    return &memory->blocks[index];
}

void PushAllocator(Memory* memory, const char* label) {
    size_t key = GetKey(label, strlen(label)) % memory->count;
    MemoryBlock *block = &memory->blocks[key];
    if(block->start == 0) {
        InitMemoryBlock(block, memory->block_size);
    }
    // Assert(block->label == 0);
    block->label = label;
    memory->allocators[++memory->allocator_count] = block;
}

void PopAllocator(Memory* memory) {
    memory->allocators[memory->allocator_count--] = 0;
}

void* Allocate(Memory* memory, size_t size) {
    MemoryBlock* block = GetMemoryBlock(memory);
    if (block->count + size < block->size) {
#if MEMORY_HEADERS
        MemoryHeader* header = (MemoryHeader*)block->ptr;
        size_t total_size = size + sizeof(MemoryHeader);
        void* result = (void*)((size_t)block->ptr + sizeof(MemoryHeader));
        block->count += total_size;
        block->allocations += 1;
        block->ptr = (void*)((size_t)block->ptr + total_size);
        header->size = size;
        header->ptr  = result;
#else
        void* result = block->ptr;
        block->count += size;
        block->allocations += 1;
        block->ptr = (void*)((size_t)block->ptr + size);
#endif
        return result;
    } else {
        // TODO(Misael): This needs to grow the list of memory blocks
        return nullptr;
    }
}

void Free(Memory* memory, void* ptr, bool total_wipe) {
    MemoryBlock* block = GetMemoryBlock(memory);
#if MEMORY_HEADERS
    size_t size = ((size_t)block->ptr - (size_t)ptr) + sizeof(MemoryHeader);
    block->count -= size;
    block->allocations -= 1;
    block->ptr = (void*)((size_t)block->ptr - size);
#else
    size_t size = (size_t)block->ptr - (size_t)ptr;
    block->count -= size;
    block->allocations -= 1;
    block->ptr = ptr;
#endif
    if (total_wipe) {
        memset(block->ptr, 0, size);
    }
}

void Clear(Memory* memory) {
    MemoryBlock* block = GetMemoryBlock(memory);
    block->count = 0;
    block->allocations = 0;
    block->ptr = block->start;
}

void ClearToZero(Memory* memory) {
    MemoryBlock* block = GetMemoryBlock(memory);
    block->count = 0;
    block->allocations = 0;
    block->ptr = block->start;
    memset(block->ptr, 0, block->size);
}

#if MEMORY_HEADERS

MemoryHeader* GetNextHeader(MemoryBlock* block, MemoryHeader* current) {
    if (current) {
        return (MemoryHeader*)((size_t)current->ptr + current->size);
    } else {
        return (MemoryHeader*)block->start;
    }
}

MemoryHeader* GetNextHeader(Memory* memory, MemoryHeader* current) {
    if (current) {
        return (MemoryHeader*)((size_t)current->ptr + current->size);
    } else {
        MemoryBlock* block = GetMemoryBlock(memory);
        return (MemoryHeader*)block->start;
    }
}

#endif
