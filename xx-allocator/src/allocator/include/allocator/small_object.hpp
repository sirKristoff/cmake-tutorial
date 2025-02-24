#pragma once

#include <cstddef>

#include "allocator/small_obj_allocator.hpp"
#include "singleton/singleton.hpp"


#ifndef MAX_SMALL_OBJECT_SIZE
  /**
   * @def MAX_SMALL_OBJECT_SIZE
   * @brief Default size of object considered as @a small.
   */
# define MAX_SMALL_OBJECT_SIZE 64
#endif

#ifndef DEFAULT_CHUNK_SIZE
# define DEFAULT_CHUNK_SIZE 4096
#endif


/**
 * @ingroup ThreadingModelPolicy
 * @brief TODO:
 * @tparam ThreadingModel  TODO:
 * @tparam chunkSize  TODO:
 * @tparam maxSmallObjectSize  TODO:
 */
template
<
    template <class> class ThreadingModel = DEFAULT_THREADING,
    std::size_t chunkSize = DEFAULT_CHUNK_SIZE, // TODO: consider removing
    std::size_t maxSmallObjectSize = MAX_SMALL_OBJECT_SIZE
>
class SmallObject
  : public ThreadingModel<
        SmallObject<ThreadingModel, chunkSize, maxSmallObjectSize> >
{
    typedef ThreadingModel<
                SmallObject<ThreadingModel, chunkSize, maxSmallObjectSize> >
            ThisThreadingModel;

    struct SmallObjAllocatorAdapter : public SmallObjAllocator
    {
        SmallObjAllocatorAdapter()
        : SmallObjAllocator(chunkSize, maxSmallObjectSize)
        {}
    };

    typedef SingletonHolder<
                SmallObjAllocatorAdapter,
                creation_policy::StaticAllocator,
                lifetime_policy::Phoenix,
                ThreadingModel>
            SmallObjAllocatorSingleton;

public:
    SmallObject() = default;
    virtual ~SmallObject() = default;

    static void* operator new(std::size_t size)
    {
#if (MAX_SMALL_OBJECT_SIZE != 0)  && (DEFAULT_CHUNK_SIZE != 0)
        typename ThisThreadingModel::Lock lock;
        (void)lock; // get rid of warning

        return SmallObjAllocatorSingleton::instance().allocate(size);
#else
        return ::operator new(size);
#endif
    }

    static void operator delete(void* p, std::size_t size)
    {
#if (MAX_SMALL_OBJECT_SIZE != 0) && (DEFAULT_CHUNK_SIZE != 0)
        typename ThisThreadingModel::Lock lock;
        (void)lock; // get rid of warning

        SmallObjAllocatorSingleton::instance().deallocate(p, size);
#else
        ::operator delete(p);
#endif
    }
};
