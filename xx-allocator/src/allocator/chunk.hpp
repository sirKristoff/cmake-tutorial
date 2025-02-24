#pragma once

#include <cstddef>
#include <stdint.h>

/**
 * @brief Structure controlling a chunk of continues memory.
 *
 * Manages fixed-size blocks.
 * Number of managed blocks is limited.
 * Chunk allocates and releases blocks of memory.
 */
struct Chunk
{
    typedef  uint8_t  Byte_t;
    typedef  unsigned char  BlockNumber_t;

    /**
     * @brief Initialize the Chunk.
     *
     * Set number and size of managed blocks.
     * Allocate memory for blocks and initialize them.
     * @param blockSize  Size of managed block (bytes).
     * @param numBlocks  Number of managed blocks.
     * @retval @c true - on success.
     * @warning blockSize is not stored in the Chunk,
     *     it has to be saved by structure supervising Chunk.
     */
    bool init(std::size_t blockSize, BlockNumber_t numBlocks);

    /**
     * @brief Allocate managed block.
     *
     * @param blockSize  Size of managed block (bytes).
     * @return Address of allocated block.
     * @retval @c nullptr - on no available block left.
     */
    void* allocate(std::size_t blockSize);

    /**
     * @brief Release managed block.
     *
     * @param p  Address of block being released.
     * @param blockSize  Size of managed block (bytes).
     */
    void deallocate(void* p, std::size_t blockSize);

    /**
     * @brief Release memory allocated for managed blocks.
     */
    void release();

    /**  Holder for managed memory.  */
    Byte_t* pData_;
    /**  Index of first available block. (start from 0)  */
    BlockNumber_t firstAvailableBlock_;
    /**  Counter of available blocks.  */
    BlockNumber_t nAvailableBlocks_;
};
