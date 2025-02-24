#include "chunk.hpp"

#include <cassert>


////////////////////////////////////////////////////////////////////////////////
// Chunk::init
////////////////////////////////////////////////////////////////////////////////

bool Chunk::init(std::size_t blockSize, BlockNumber_t numBlocks)
{
    if (!(pData_ = new Byte_t[blockSize * numBlocks]))
        return false;
    firstAvailableBlock_ = 0u;
    nAvailableBlocks_ = numBlocks;
    Byte_t* pBlock = pData_;
    for (BlockNumber_t blockIdx = 0u; blockIdx != numBlocks; pBlock += blockSize)
    {
        *pBlock = ++blockIdx;
    }
    return true;
}

////////////////////////////////////////////////////////////////////////////////
// Chunk::allocate
////////////////////////////////////////////////////////////////////////////////

void* Chunk::allocate(std::size_t blockSize)
{
    if (!nAvailableBlocks_)
        return nullptr;

    Byte_t* pAllocatedBlock =
        pData_ + (firstAvailableBlock_ * blockSize);

    // change firstAvailableBlock_ to point at next available block
    firstAvailableBlock_ = *pAllocatedBlock;
    --nAvailableBlocks_;
    return pAllocatedBlock;
}

////////////////////////////////////////////////////////////////////////////////
// Chunk::deallocate
////////////////////////////////////////////////////////////////////////////////

void Chunk::deallocate(void* p, std::size_t blockSize)
{
    assert(pData_ <= p);
    Byte_t* pBlockToRelease = static_cast<Byte_t*>(p);
    // check alignment of given pointer (must point to first byte of block)
    assert((pBlockToRelease - pData_) % blockSize == 0);
    *pBlockToRelease = firstAvailableBlock_;  // last released block points to previous first available block
    // the currently being released block will become the first available block
    firstAvailableBlock_ =
        static_cast<BlockNumber_t>((pBlockToRelease - pData_) / blockSize);
    // check range of given pointer
    assert(firstAvailableBlock_ == (pBlockToRelease - pData_) / blockSize);
    ++nAvailableBlocks_;
}

////////////////////////////////////////////////////////////////////////////////
// Chunk::release
////////////////////////////////////////////////////////////////////////////////

void Chunk::release()
{
    delete[] pData_;
    pData_ = nullptr;
    firstAvailableBlock_ = 0u;
    nAvailableBlocks_ = 0u;
}
