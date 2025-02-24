#pragma once

#include <vector>

#include "fixed_allocator.hpp"
#include "allocator/small_obj_allocator.hpp"


/**
 * @brief Offers services for allocating @a small-sized objects.
 *
 * Allocates memory by using best fitting FixedAllocator
 * or standard ::operator new.
 */
class SmallObjAllocator::Impl
{
private:
    /** Fixed-sized allocators collection.  */
    typedef std::vector<FixedAllocator> FixedAllocatorPool;

public:
    /**
     * @brief Creates an allocator for small objects given chunk size and
     *        maximum 'small' object size.
     *
     * @param chunkSize  Default size of Chunk object's managed memory (bytes).
     * @todo TODO: check how to use chunkSize (currently unused).
     * @param maxObjectSize  Maximum size of @a small-sized object (bytes).
     */
    Impl(std::size_t chunkSize, std::size_t maxObjectSize);
    Impl(const Impl&) = delete;
    Impl& operator=(const Impl&) = delete;
    ~Impl() = default;

    /**
     * @brief Allocates 'size' bytes memory.
     *
     * Uses an internal pool of FixedAllocator objects for small objects.
     *
     * @param size  Size of object to allocate (bytes).
     * @return Address of allocated memory.
     * @retval @c nullptr - on failure.
     */
    void* allocate(std::size_t size);

    /**
     * @brief Deallocates memory previously allocated with allocate().
     *
     * @param p  Address of memory for deallocation.
     * @param size  Size of object being deallocate (bytes).
     * @warning @b Undefined @b behavior if you pass any other pointer
     *        for memory not allocated by allocate().
     */
    void deallocate(void* p, std::size_t size);

private:
    /**
     * @brief Memory pool for small objects of various sizes.
     *
     * The pool's elements are in order allowing perform binary search.
     */
    FixedAllocatorPool pool_;
    /**  First check for allocation request.  */
    FixedAllocator* pLastAlloc_;
    /**  First check for deallocation request.  */
    FixedAllocator* pLastDealloc_;
    /**
     * @brief  Default size of Chunk object's managed memory (bytes).
     * @todo TODO: chunkSize_ currently unused.
     */
    std::size_t chunkSize_;
    /**  Maximum size of @a small-sized object (bytes).  */
    std::size_t maxObjectSize_;
};
