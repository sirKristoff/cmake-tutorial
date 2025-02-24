/**
 * @file singleton.hpp
 *
 * @remark This code is inspired by the materials included with the book:
 *   Alexandrescu, Andrei. "Modern C++ Design: Generic Programming and Design
 *   Patterns Applied". Copyright (c) 2001. Addison-Wesley.
 */

#pragma once

#include <cassert>
#include <cstdlib>
#include <stdexcept>
#include <type_traits>

#include "threads/threading_model.hpp"


/**
 * @def ATEXIT_FIXED
 * @ingroup GrpLifetimePolicy
 * @brief Indicator of fix for std::atexit() in context of its recurrence calls.
 *
 * Some compilers did not fix problem with not defined by standard behaviour,
 * when function registered by std::atexit() calls std::atexit() internally.
 *
 * @sa lifetime_policy::Phoenix
 *
 * @todo
 * If used compiler fixing mentioned problem, then ATEXIT_FIXED macro should
 * be defined before including "singleton.hpp" header file.
 */
#define ATEXIT_FIXED
// TODO: should be defined externally


/**
 * @defgroup GrpCreationPolicy  Creation policy
 *
 * Policy providing object creation and destruction mechanisms.
 *
 * Introduce policy class template with following constraints:
 *
 * @li  policy class template has only one type parameter
 * @li  implements @a create method providing memory address of already allocated object
 * @li  implements @a destroy method able to release memory for previously allocated
 *       object by @a create method
 *
 * Sample code for policy class declaration:
 * @code{.cpp}
 * template <class T>
 * struct PolicyClassName
 * {
 *   static T* create();
 *   static void destroy(T* p);
 * };
 * @endcode
 */

/**  @ingroup GrpCreationPolicy  */
namespace creation_policy
{

////////////////////////////////////////////////////////////////////////////////
// class template NewAllocator
////////////////////////////////////////////////////////////////////////////////

/**
 * @ingroup GrpCreationPolicy
 * @brief Creates objects using a straight call to the new operator.
 *
 * @tparam T  Class of objects being created.
 * @sa @link GrpCreationPolicy  Creation policy specification @endlink
 */
template <class T>
struct NewAllocator
{
    static T* create()
    {  return new T;  }

    static void destroy(T* p)
    {  delete p;  }
};


////////////////////////////////////////////////////////////////////////////////
// class template MallocAllocator
////////////////////////////////////////////////////////////////////////////////

/**
 * @ingroup GrpCreationPolicy
 * @brief Creates objects using a call to std::malloc, followed by a call to the
 *        placement new operator.
 *
 * @tparam T  Class of objects being created.
 * @sa @link GrpCreationPolicy  Creation policy specification @endlink
 */
template <class T>
struct MallocAllocator
{
    static T* create()
    {
        void* p = std::malloc(sizeof(T));
        if (!p) return nullptr;
        return new(p) T;
    }

    static void destroy(T* p)
    {
        if (nullptr == p)  return;
        p->~T();
        std::free(p);
    }
};


////////////////////////////////////////////////////////////////////////////////
// class template StaticAllocator
////////////////////////////////////////////////////////////////////////////////

/**
 * @ingroup GrpCreationPolicy
 * @brief Creates an object in static memory.
 *
 * @tparam T  Class of objects being created.
 * @sa @link GrpCreationPolicy  Creation policy specification @endlink
 */
template <class T>
struct StaticAllocator
{
    static T* create()
    {
        static std::aligned_storage_t<sizeof(T), alignof(T)> staticMemory_;
#ifndef NDEBUG
        staticMemoryAddress_ = &staticMemory_;
#endif
        return new(&staticMemory_) T;
    }

    static void destroy(T* p)
    {
        assert(p == staticMemoryAddress_);
        if(p)
            p->~T();
    }

private:
#ifndef NDEBUG
    static T* staticMemoryAddress_;
#endif
};

#ifndef NDEBUG
template <class T>
T* StaticAllocator<T>::staticMemoryAddress_ = nullptr;
#endif

}  // namespace creation_policy


/**
 * @defgroup GrpLifetimePolicy  Lifetime policy
 *
 * Policy providing mechanism for scheduling moment of object destruction.
 *
 * Introduce policy class template with following constraints:
 *
 * @li  policy class template has only one type parameter
 * @li  implements @a scheduleDestruction method registering function responsible
 *        for the object destruction
 * @li  implements @a onDeadReference method, called in case of the object dead
 *        reference detection
 *
 * Sample code for policy class declaration:
 * @code{.cpp}
 * template <class T>
 * struct PolicyClassName
 * {
 *   static void scheduleDestruction(T* pObj, void (*atexit_pFun)());
 *   static void onDeadReference();
 * };
 * @endcode
 */

/**  @ingroup GrpLifetimePolicy  */
namespace lifetime_policy
{

/**  Function prototype accepted by std::atexit().  */
typedef void (*atexit_pfn_t)();


////////////////////////////////////////////////////////////////////////////////
// class template Default
////////////////////////////////////////////////////////////////////////////////

/**
 * @ingroup GrpLifetimePolicy
 * @brief Meyers Singleton behaviour.
 *
 * Schedules an object's destruction as per C++ rules; forwards to std::atexit().
 *
 * @note After object destruction (done by std::atexit()) any call for its
 *   instance will cause rising std::logic_error exception.
 *
 * @tparam T  Class of objects which destruction is being scheduled for.
 * @sa @link GrpLifetimePolicy  Lifetime policy specification @endlink
 */
template <class T>
struct Default
{
    static void scheduleDestruction(T*, atexit_pfn_t pFun)
    {  std::atexit(pFun);  }

    static void onDeadReference()
    {  throw std::logic_error("Dead Reference Detected");  }
};


////////////////////////////////////////////////////////////////////////////////
// class template Phoenix
////////////////////////////////////////////////////////////////////////////////

/**
 * @ingroup GrpLifetimePolicy
 * @brief Recreate object on dead reference detected.
 *
 * Schedules an object's destruction as per C++ rules, and it allows object
 * recreation by not throwing an exception from onDeadReference.
 *
 * @tparam T  Class of objects which destruction is being scheduled for.
 * @sa @link GrpLifetimePolicy  Lifetime policy specification @endlink
 */
template <class T>
class Phoenix
{
public:
    static void scheduleDestruction(T*, atexit_pfn_t pFun)
    {
#ifndef ATEXIT_FIXED
        if (!destroyedOnce_)
#endif
            std::atexit(pFun);
    }

    static void onDeadReference()
    {
#ifndef ATEXIT_FIXED
        destroyedOnce_ = true;
#endif
    }

private:
#ifndef ATEXIT_FIXED
    static bool destroyedOnce_;
#endif
};

#ifndef ATEXIT_FIXED
template <class T>
bool Phoenix<T>::destroyedOnce_ = false;
#endif


////////////////////////////////////////////////////////////////////////////////
// class template NoDestroy
////////////////////////////////////////////////////////////////////////////////

/**
 * @ingroup GrpLifetimePolicy
 * @brief Infinite lifetime objects never destroyed.
 *
 * Once created object lives till applications process finish without memory leak.
 * @note The OS is responsible for releasing memory but singleton may acquire
 * some @b resources (network connection, database transaction, semaphore, shared
 * memory, etc.) which are not going to be released automatic.
 *
 * @tparam T  Class of objects which destruction is being scheduled for.
 * @sa @link GrpLifetimePolicy  Lifetime policy specification @endlink
 */
template <class T>
struct NoDestroy
{
    static void scheduleDestruction(T*, atexit_pfn_t /*pFun*/)
    {}

    static void onDeadReference()
    {}
};


/**  For making implementation private.  */
namespace internal
{

////////////////////////////////////////////////////////////////////////////////
// class LifetimeTracker
////////////////////////////////////////////////////////////////////////////////

/**
 * @brief For tracking objects lifetime.
 *
 * Helper class for setLongevity.
 */
class LifetimeTracker
{
public:
    LifetimeTracker(unsigned int longevity);

    virtual ~LifetimeTracker() = 0;  // For make class abstract

    static bool compare(
        const LifetimeTracker* lhs,
        const LifetimeTracker* rhs);

    /**
     * @brief Register tracker in some global collection.
     *
     * @param newTracker  Pointer to tracker.
     * @throws  @c std::bad_alloc  in case of not enough memory for storing
     *   tracker in global collection.
     */
    static void registerTracker(LifetimeTracker* newTracker);

    /**
     * @brief Ensures destruction of objects with longevity in proper order.
     * @note Its pointer will be passed to std::atexit().
     */
    static void atexit_fun();

private:
    unsigned int longevity_;
};


////////////////////////////////////////////////////////////////////////////////
// class template ObjDestroyer
////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Concrete lifetime tracker for objects of type T
 * @tparam T  Objects for tracking lifetime.
 * @tparam Destroyer  Responsible for destruction of T objects.
 *   Destroyer must implement:
 *   @code{.cpp}
 *     void operator()(T*)
 *   @endcode
 */
template <typename T, typename Destroyer>
class ObjDestroyer : public LifetimeTracker
{
public:
    ObjDestroyer(
        T* p,
        unsigned int longevity,
        Destroyer d)
    : LifetimeTracker(longevity),
      pTracked_(p),
      destroyer_(d)
    {}

    ~ObjDestroyer()
    {  destroyer_(pTracked_);  }

private:
    T* pTracked_;
    Destroyer destroyer_;
};


////////////////////////////////////////////////////////////////////////////////
// function template setLongevity
////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Assigns an object a longevity.
 *
 * Ensures ordered destructions of objects registered thusly during the exit
 * sequence of the application.
 *
 * @note Memory for objects with assigned longevity will be released internally.
 * @note An pDynObject may be destroyed immediately in case of some failure.
 * @warning No staticly allocated object may have longevity assigned.
 *
 * @tparam T  Objects for tracking lifetime.
 * @tparam Destroyer  Responsible for destruction of T objects.
 *   Destroyer must implement:
 *   @code{.cpp}
 *     void operator()(T*)
 *   @endcode
 *
 * @param pDynObject  Object for tracking lifetime.
 * @param longevity  Priority in destruction order
 *   (higher longevity, later destruction).
 * @param destroyer  Object responsible for destruction of pDynObject.
 *
 * @sa WithLongevity::scheduleDestruction()
 */
template <typename T, typename Destroyer>
void setLongevity(
    T* pDynObject,
    unsigned int longevity,
    Destroyer destroyer = creation_policy::NewAllocator<T>::destroy)
{
    if (nullptr == pDynObject)  return;

    auto pNewTracker = new ObjDestroyer<T, Destroyer>(
        pDynObject, longevity, destroyer);
    if (nullptr == pNewTracker)
    {  // failure tracker creation - immediately destroy pDynObject
        destroyer(pDynObject);
        return;
    }

    try
    {
        LifetimeTracker::registerTracker(pNewTracker);
    }
    catch(const std::bad_alloc&)
    {
        delete pNewTracker;
        assert(("Failure placing LifetimeTracker in global collection", false));
        return;
    }
    catch(...)
    {
        delete pNewTracker;
        assert(("Unspecified LifetimeTracker::registerTracker() failure", false));
        throw;
    }

    // Register a call for destruction
    std::atexit(LifetimeTracker::atexit_fun);
}

} // namespace internal


////////////////////////////////////////////////////////////////////////////////
// function template getLongevity
////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Get object's longevity.
 *
 * This is generic function prototype.
 * Specialization for concrete type should be provided.
 *
 * Example specialization of getLongevity() function:
 * @code
 * template <>
 * unsigned int lifetime_policy::getLongevity(ConcreteObjectType*)
 * {  return 666u;  }
 * @endcode
 *
 * @tparam T  Class of object with longevity property.
 * @param pDynObject  Object with longevity property.
 * @return  Value of objects longevity (higher longevity, later destruction).
 * @sa WithLongevity::scheduleDestruction()
 */
template <typename T>
unsigned int getLongevity(T* pDynObject);


////////////////////////////////////////////////////////////////////////////////
// class template WithLongevity
////////////////////////////////////////////////////////////////////////////////

/**
 * @ingroup GrpLifetimePolicy
 * @brief Schedules an object's destruction in order of their longevities.
 *
 * Assumes a visible function getLongevity(T*) that returns the longevity of the
 * object.
 *
 * @tparam T  Class of objects which destruction is being scheduled for.
 * @sa @link GrpLifetimePolicy  Lifetime policy specification @endlink
 */
template <class T>
class WithLongevity
{
public:
    static void scheduleDestruction(T* pObj, atexit_pfn_t pFun)
    {
        internal::setLongevity(pObj, getLongevity(pObj), [](T*){ pFun(); });
    }

    static void onDeadReference()
    {  throw std::logic_error("Dead Reference Detected");  }
};

}  // namespace lifetime_policy


////////////////////////////////////////////////////////////////////////////////
// class template SingletonHolder
////////////////////////////////////////////////////////////////////////////////

/**
 * @ingroup GrpCreationPolicy GrpLifetimePolicy GrpThreadingModelPolicy
 * @brief Provides singleton amenities for a type Singleton_t.
 *
 * To protect Singleton_t type from spurious instantiations, you have to protect
 * it yourself.@n
 * Sample code for singleton class declaration:
 * @code{.cpp}
 * struct Singleton
 * {
 *   friend PolicyClassName<Singleton>;
 * private:
 *   Singleton() = default;
 *   Singleton(const Singleton&) = delete;
 *   Singleton& operator=(const Singleton&) = delete;
 *   ~Singleton() = default;
 *   Singleton* operator&() = delete;  // address-of operator
 *   const Singleton* operator&() const = delete;
 * };
 * @endcode
 *
 * @tparam Singleton_t  Singleton class.
 * @tparam CreationPolicy
 *   The @link GrpCreationPolicy  Creation policy @endlink class
 * @tparam LifetimePolicy
 *   The @link GrpLifetimePolicy  Lifetime policy @endlink class
 * @tparam ThreadingModel
 *   The @link GrpThreadingModelPolicy  Threading Model policy @endlink
 * @sa @link GrpCreationPolicy  Creation policy specification @endlink
 * @sa @link GrpLifetimePolicy  Lifetime policy specification @endlink
 * @sa @link GrpThreadingModelPolicy  Threading Model policy specification @endlink
 */
template
<
    typename Singleton_t,
    template <class> class CreationPolicy = creation_policy::NewAllocator,
    template <class> class LifetimePolicy = lifetime_policy::Default,
    template <class> class ThreadingModel = threading_policy::SingleThreaded
>
class SingletonHolder
{
public:
    /**
     * @brief Get singleton instance.
     * @return  reference to singleton instance.
     * @throw @c std::bad_alloc  in case of the instance creation failure.
     * @throw @c std::runtime_error  in case of the instance destruction while
     *   scheduling destruction.
     */
    static Singleton_t& instance();

private:
    // Helpers

    /**
     * @brief Create singleton instance.
     *
     * The instance may be recreated when dead reference detected.
     */
    static void makeInstance();

    /**
     * @brief Responsible for destroying singleton instance.
     *
     * Method is indirectly called by LifetimePolicy (typically at the program
     * closing procedure);
     * It should be scheduled by LifetimePolicy::scheduleDestruction().
     */
    static void destroySingleton();

    SingletonHolder() = delete;

    /** Pointer type of singleton instance which may be modified interthread */
    typedef  typename ThreadingModel<Singleton_t*>::volatile_type
             instance_pointer;

    /**  Holder for the singleton object  */
    static instance_pointer  pInstance_;
    /** Indicator for call SingletonHolder::destroySingleton()  */
    static bool  destroyed_;
};


////////////////////////////////////////////////////////////////////////////////
// SingletonHolder's static members
////////////////////////////////////////////////////////////////////////////////

template
<
    class S,
    template <class> class C,
    template <class> class L,
    template <class> class M
>
typename SingletonHolder<S, C, L, M>::instance_pointer
    SingletonHolder<S, C, L, M>::pInstance_ = nullptr;

template
<
    class S,
    template <class> class C,
    template <class> class L,
    template <class> class M
>
bool  SingletonHolder<S, C, L, M>::destroyed_ = false;


////////////////////////////////////////////////////////////////////////////////
// SingletonHolder::instance
////////////////////////////////////////////////////////////////////////////////

template
<
    class Singleton_t,
    template <class> class CreationPolicy,
    template <class> class LifetimePolicy,
    template <class> class ThreadingModel
>
inline Singleton_t&
SingletonHolder<Singleton_t, CreationPolicy, LifetimePolicy, ThreadingModel>::
instance()
{
    if (nullptr == pInstance_)
    {
        makeInstance();
    }
    return *pInstance_;
}

////////////////////////////////////////////////////////////////////////////////
// SingletonHolder::makeInstance (helper for instance)
////////////////////////////////////////////////////////////////////////////////

template
<
    class Singleton_t,
    template <class> class CreationPolicy,
    template <class> class LifetimePolicy,
    template <class> class ThreadingModel
>
void
SingletonHolder<Singleton_t, CreationPolicy, LifetimePolicy, ThreadingModel>::
makeInstance()
{
    typename ThreadingModel<Singleton_t>::Lock guard;
    (void)guard;

    if (nullptr == pInstance_)
    {
        if (destroyed_)
        {  // exception may be thrown by LifetimePolicy
            LifetimePolicy<Singleton_t>::onDeadReference();
            destroyed_ = false;
        }
        if (nullptr == (pInstance_ = CreationPolicy<Singleton_t>::create()))
            throw std::bad_alloc();
        LifetimePolicy<Singleton_t>::scheduleDestruction(
            pInstance_, &destroySingleton);
        if (destroyed_)  // pInstance_ destroyed by scheduleDestruction()
            throw std::runtime_error(
                "Instance destroyed while scheduling destruction");
    }
}


////////////////////////////////////////////////////////////////////////////////
// SingletonHolder::destroySingleton (helper for instance)
////////////////////////////////////////////////////////////////////////////////

template
<
    class Singleton_t,
    template <class> class CreationPolicy,
    template <class> class L,
    template <class> class M
>
void  SingletonHolder<Singleton_t, CreationPolicy, L, M>::
destroySingleton()
{
    assert(!destroyed_);
    CreationPolicy<Singleton_t>::destroy(pInstance_);
    pInstance_ = nullptr;
    destroyed_ = true;
}
