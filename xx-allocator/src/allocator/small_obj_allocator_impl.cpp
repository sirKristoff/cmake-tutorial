#include "small_obj_allocator_impl.hpp"

#include <algorithm>
#include <cassert>


////////////////////////////////////////////////////////////////////////////////
// SmallObjAllocator::Impl::Impl
////////////////////////////////////////////////////////////////////////////////

SmallObjAllocator::Impl::Impl(
    std::size_t chunkSize,
    std::size_t maxObjectSize)
: pLastAlloc_(0),
  pLastDealloc_(0),
  chunkSize_(chunkSize),
  maxObjectSize_(maxObjectSize)
{
}

////////////////////////////////////////////////////////////////////////////////
// SmallObjAllocator::Impl::allocate
////////////////////////////////////////////////////////////////////////////////

void* SmallObjAllocator::Impl::allocate(std::size_t size)
{
    if (maxObjectSize_ < size )
        return operator new(size);

    if (pLastAlloc_ && pLastAlloc_->blockSize() == size)
    {  // pLastAlloc_ block's size fits request ('size')
        return pLastAlloc_->allocate();
    }

    // Find allocator with fitting block's size
    FixedAllocatorPool::iterator fixedAllocatorItr = std::lower_bound(
        pool_.begin(), pool_.end(), size,
        [](const FixedAllocator& fixedAllocator, std::size_t size) -> bool
        {  return fixedAllocator.blockSize() < size;  }
    );

    if (fixedAllocatorItr == pool_.end() || fixedAllocatorItr->blockSize() != size)
    {  // No allocator fits 'size' - create one
        fixedAllocatorItr = pool_.insert(fixedAllocatorItr, FixedAllocator(size));
        pLastDealloc_ = &*pool_.begin();
        // TODO: consider leaving pLastDealloc_ unchanged (its position - pointer might be changed)
    }
    pLastAlloc_ = &*fixedAllocatorItr;

    return pLastAlloc_->allocate();
}

////////////////////////////////////////////////////////////////////////////////
// SmallObjAllocator::Impl::deallocate
////////////////////////////////////////////////////////////////////////////////

void SmallObjAllocator::Impl::deallocate(void* p, std::size_t size)
{
    if (maxObjectSize_ < size)
        return operator delete(p);

    if (pLastDealloc_ && pLastDealloc_->blockSize() == size)
    {  // pLastDealloc_ block's size fits request ('size')
        pLastDealloc_->deallocate(p);
        return;
    }

    // Find allocator with fitting block's size
    FixedAllocatorPool::iterator fixedAllocatorItr = std::lower_bound(
        pool_.begin(), pool_.end(), size,
        [](const FixedAllocator& fixedAllocator, std::size_t size) -> bool
        {  return fixedAllocator.blockSize() < size;  }
    );

    assert(fixedAllocatorItr != pool_.end());  // allocator exists in pool
    assert(fixedAllocatorItr->blockSize() == size);  // allocator block's size fits

    pLastDealloc_ = &*fixedAllocatorItr;
    pLastDealloc_->deallocate(p);

    // @note Never release allocator, even if all its memory chunks are free.
}
