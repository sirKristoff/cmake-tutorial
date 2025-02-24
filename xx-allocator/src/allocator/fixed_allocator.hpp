#pragma once

#include <cstddef>
#include <vector>

#include "chunk.hpp"


#ifndef DEFAULT_CHUNK_SIZE
  /**
   * @def DEFAULT_CHUNK_SIZE
   * @brief Default size of contiguous memory @a chunk internally allocated at once.
   * @note In fact Chunk size may be smaller due to total number of blocks,
   *       and equal size of each allocable block.
   */
# define DEFAULT_CHUNK_SIZE 4096
#endif


/**
 * @brief Allocator for fixed-size Chunks.
 *
 * Offers services for allocating fixed-sized objects.
 * Manages and control life-cycle of Chunks.
 */
class FixedAllocator
{
private:
    typedef  Chunk::BlockNumber_t  BlockNumber_t;
    /**  Chunk collection  */
    typedef std::vector<Chunk> Chunks;

public:
    /**
     * @brief Creates a FixedAllocator object of a fixed block size.
     *
     * @param blockSize  Size of block managed by Chunk (bytes).
     * @warning  TODO: blockSize equal zero cause division by zero.
     */
    explicit FixedAllocator(std::size_t blockSize = 0);
    FixedAllocator(const FixedAllocator& src);
    FixedAllocator& operator=(const FixedAllocator& rhs);
    ~FixedAllocator();
    void swap(FixedAllocator& rhs);

    /**
     * @brief Allocate a memory block of fixed size.
     *
     * @return Address of allocated memory block.
     * @retval @c nullptr - on failure.
     */
    void* allocate();

    /**
     * @brief Deallocate a memory block previously allocated with allocate().
     *
     * @param p  Address of block being released.
     * @warning @b Undefined @b behavior if called with the wrong pointer.
     */
    void deallocate(void* p);

    /**
     * @return The block size with which the FixedAllocator was initialized.
     */
    std::size_t blockSize() const
    {  return blockSize_;  }

private:
    /**
     * @brief Performs deallocation. Assumes deallocChunk_ points to the correct chunk
     *
     * @param p  Address of block being released.
     */
    void doDeallocate(void* p);

    /**
     * @brief Finds the chunk corresponding to a pointer, using an efficient search
     *
     * @param p  Address of block.
     * @return Chunk which p belongs to.
     * @retval @c nullptr - on failure.
     */
    Chunk* vicinityFind(void* p);

    /**  Size of block managed by Chunk (bytes).  */
    std::size_t blockSize_;
    /**  Number of managed blocks in one Chunk.  */
    BlockNumber_t numBlocks_;
    /**  Container for Chunks */
    Chunks chunks_;
    /**  Points to Chunk with block available to allocate  */
    Chunk* allocChunk_;
    /**  Chunk with the last released memory  */
    Chunk* deallocChunk_;

    // For ensuring proper copy semantics
    /**  Link to next FixedAllocator owning this same Chunks.  */
    mutable const FixedAllocator* next_;
    /**  Link to previous FixedAllocator owning this same Chunks.  */
    mutable const FixedAllocator* prev_;
};
