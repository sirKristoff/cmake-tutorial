#pragma once

#include <cstddef>
#include <memory>


/**
 * @brief TODO:
 * 
 */
class SmallObjAllocator
{
    class Impl;
    std::unique_ptr<Impl> pImpl_;
public:
    /**
     * @brief Creates an allocator for small objects given chunk size and
     *        maximum 'small' object size.
     *
     * @param chunkSize  Default size of Chunk object's managed memory (bytes).
     * @param maxObjectSize  Maximum size of @a small-sized object (bytes).
     */
    SmallObjAllocator(std::size_t chunkSize, std::size_t maxObjectSize);
    SmallObjAllocator(const SmallObjAllocator&) = delete;
    SmallObjAllocator& operator=(const SmallObjAllocator&) = delete;
    ~SmallObjAllocator() = default;

    /**
     * @brief Allocates 'size' bytes memory.
     * @param size  Size of object to allocate (bytes).
     * @return Address of allocated memory.
     * @retval @c nullptr - on failure.
     */
    void* allocate(std::size_t size);

    /**
     * @brief Deallocates memory previously allocated with allocate().
     * @param p  Address of memory for deallocation.
     * @param size  Size of object being deallocate (bytes).
     * @warning @b Undefined @b behavior if you pass any other pointer
     *        for memory not allocated by allocate().
     */
    void deallocate(void* p, std::size_t size);
};
