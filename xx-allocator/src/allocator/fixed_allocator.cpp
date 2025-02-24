#include "fixed_allocator.hpp"

#include <cassert>
#include <limits>
#include <utility>


////////////////////////////////////////////////////////////////////////////////
// FixedAllocator::FixedAllocator
////////////////////////////////////////////////////////////////////////////////

FixedAllocator::FixedAllocator(std::size_t blockSize)
  : blockSize_(blockSize),
    allocChunk_(0),
    deallocChunk_(0)
{
    assert(blockSize_ > 0);

    prev_ = next_ = this;

    std::size_t numBlocks = DEFAULT_CHUNK_SIZE / blockSize;
    if (std::numeric_limits<BlockNumber_t>::max() < numBlocks)
        numBlocks = std::numeric_limits<BlockNumber_t>::max();
    else if (numBlocks == 0)
        numBlocks = 8 * blockSize;

    numBlocks_ = static_cast<BlockNumber_t>(numBlocks);
    assert(numBlocks_ == numBlocks);
}

////////////////////////////////////////////////////////////////////////////////
// FixedAllocator::FixedAllocator
////////////////////////////////////////////////////////////////////////////////

FixedAllocator::FixedAllocator(const FixedAllocator& rhs)
  : blockSize_(rhs.blockSize_),
    numBlocks_(rhs.numBlocks_),
    chunks_(rhs.chunks_)
{
    prev_ = &rhs;
    next_ = rhs.next_;
    rhs.next_->prev_ = this;
    rhs.next_ = this;

    // keep relative position of allocChunk_ from rhs
    allocChunk_ = rhs.allocChunk_
        ? &chunks_.front() + (rhs.allocChunk_ - &rhs.chunks_.front())
        : nullptr;

    // keep relative position of deallocChunk_ from rhs
    deallocChunk_ = rhs.deallocChunk_
        ? &chunks_.front() + (rhs.deallocChunk_ - &rhs.chunks_.front())
        : nullptr;
}

////////////////////////////////////////////////////////////////////////////////
// FixedAllocator::operator=
////////////////////////////////////////////////////////////////////////////////

FixedAllocator& FixedAllocator::operator=(const FixedAllocator& rhs)
{
    FixedAllocator copy(rhs);
    copy.swap(*this);
    return *this;
}

////////////////////////////////////////////////////////////////////////////////
// FixedAllocator::~FixedAllocator
////////////////////////////////////////////////////////////////////////////////

FixedAllocator::~FixedAllocator()
{
    if (prev_ != this)
    {
        prev_->next_ = next_;
        next_->prev_ = prev_;
        return;
    }

    assert(prev_ == next_);
    for (auto& chunk : chunks_)
    {
       assert(chunk.nAvailableBlocks_ == numBlocks_);
       chunk.release();
    }
}

////////////////////////////////////////////////////////////////////////////////
// FixedAllocator::swap
////////////////////////////////////////////////////////////////////////////////

void FixedAllocator::swap(FixedAllocator& rhs)
{
    std::swap(blockSize_, rhs.blockSize_);
    std::swap(numBlocks_, rhs.numBlocks_);
    chunks_.swap(rhs.chunks_);
    std::swap(allocChunk_, rhs.allocChunk_);
    std::swap(deallocChunk_, rhs.deallocChunk_);
    // TODO: do something with next_ and prev_
}

////////////////////////////////////////////////////////////////////////////////
// FixedAllocator::allocate
////////////////////////////////////////////////////////////////////////////////

void* FixedAllocator::allocate()
{
    if (nullptr == allocChunk_ || 0 == allocChunk_->nAvailableBlocks_)
    {
        // set allocChunk_ with available block
        for (Chunks::iterator chunkItr = chunks_.begin(); ; ++chunkItr)
        {
            if (0 < chunkItr->nAvailableBlocks_)
            {  // found Chunk with available block
                allocChunk_ = &*chunkItr;
                break;
            }

            if (chunkItr == chunks_.end())
            {  // have to create new Chunk
                // Initialize
                chunks_.reserve(chunks_.size() + 1);
                Chunk newChunk;
                newChunk.init(blockSize_, numBlocks_);
                chunks_.push_back(newChunk);
                allocChunk_ = &chunks_.back();
                deallocChunk_ = &chunks_.front();
                break;
            }
        }
    }
    assert(allocChunk_ != 0);
    assert(0 < allocChunk_->nAvailableBlocks_);

    return allocChunk_->allocate(blockSize_);  // block allocation
}

////////////////////////////////////////////////////////////////////////////////
// FixedAllocator::deallocate
////////////////////////////////////////////////////////////////////////////////

void FixedAllocator::deallocate(void* p)
{
    assert(!chunks_.empty());
    assert(&chunks_.front() <= deallocChunk_);
    assert(deallocChunk_ <= &chunks_.back());

    deallocChunk_  = vicinityFind(p);  // find Chunk owning memory 'p'
    assert(deallocChunk_);

    doDeallocate(p);
}

////////////////////////////////////////////////////////////////////////////////
// FixedAllocator::vicinityFind
// Finds the chunk corresponding to a pointer, using an efficient search
////////////////////////////////////////////////////////////////////////////////

Chunk* FixedAllocator::vicinityFind(void* p)
{
    assert(!chunks_.empty());
    assert(deallocChunk_);

    const std::size_t chunkLength = numBlocks_ * blockSize_;

    Chunk* lo = deallocChunk_;
    Chunk* hi = deallocChunk_ + 1;
    Chunk* loBound = &chunks_.front();
    Chunk* hiBound = &chunks_.back() + 1;

	// Special case: deallocChunk_ is the last in the array
	if (hi == hiBound) hi = nullptr;

    while (lo || hi)
    {
        if (lo)
        {
            if (lo->pData_ <= p  &&  p < lo->pData_ + chunkLength)
            {
                return lo;
            }
            if (lo == loBound)  lo = nullptr;
            else                --lo;
        }

        if (hi)
        {
            if (hi->pData_ <= p  &&  p < hi->pData_ + chunkLength)
            {
                return hi;
            }
            if (++hi == hiBound)  hi = nullptr;
        }
    }
    assert(false);
    return nullptr;
}

////////////////////////////////////////////////////////////////////////////////
// FixedAllocator::doDeallocate
// Performs deallocation. Assumes deallocChunk_ points to the correct chunk
////////////////////////////////////////////////////////////////////////////////

void FixedAllocator::doDeallocate(void* p)
{
    assert(deallocChunk_->pData_ <= p);
    assert(p < deallocChunk_->pData_ + numBlocks_ * blockSize_);

    // call into the chunk, will adjust the inner list but won't release memory
    deallocChunk_->deallocate(p, blockSize_);

    if (deallocChunk_->nAvailableBlocks_ == numBlocks_)
    {
        // deallocChunk_ is completely free, should we release it? 

        Chunk& lastChunk = chunks_.back();

        if (&lastChunk == deallocChunk_)
        {
            // check if we have two last chunks empty

            if (1 < chunks_.size()
                &&  deallocChunk_[-1].nAvailableBlocks_ == numBlocks_)
            {
                // Two free chunks, discard the last one
                lastChunk.release();
                chunks_.pop_back();
                allocChunk_ = deallocChunk_ = &chunks_.front();
                // TODO: consider leaving allocChunk_ unchanged with check ??
            }
            return;
        }

        if (lastChunk.nAvailableBlocks_ == numBlocks_)
        {
            // Two free blocks, discard one on back (lastChunk)
            lastChunk.release();
            chunks_.pop_back();
            allocChunk_ = deallocChunk_;
            // TODO: consider leaving allocChunk_ unchanged with check if it's not lastChunk
        }
        else  // TODO: validate removing 'else' to move deallocChunk_ to back
        {
            // move the empty chunk to the end
            std::swap(*deallocChunk_, lastChunk);
            allocChunk_ = &chunks_.back();
            // TODO: consider leaving allocChunk_ unchanged with check if it's not deallocChunk_ neither lastChunk
        }
    }
}
