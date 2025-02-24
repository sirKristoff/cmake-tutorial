/**
 * @file threading_model.hpp
 * @ingroup GrpThreadingModelPolicy
 *
 * @remark This code is inspired by the materials included with the book:
 *   Alexandrescu, Andrei. "Modern C++ Design: Generic Programming and Design
 *   Patterns Applied". Copyright (c) 2001. Addison-Wesley.
 */

#pragma once

#include <mutex>
#include <type_traits>

/**
 * @defgroup GrpThreadingModelPolicy  Threading Model Policy
 *
 * Policy providing multithreading synchronisation mechanisms.
 *
 * Introduce policy class template with following constraints:
 *
 * @li  policy class template has only one type parameter
 * @li  template parameter should inherit from policy class
 * @li  TODO: typedef  Host  volatile_type;
 * @li  policy class has internal type @c Lock
 * @li  @c Lock is default constructable
 * @li  @c Lock can be constructed from policy class reference
 *
 * Sample code for policy class declaration:
 * @code{.cpp}
 * template <class Host>
 * struct PolicyClassName
 * {
 *   typedef  typename std::add_volatile_t<Host>  volatile_type;
 *   struct Lock
 *   {
 *     Lock();
 *     Lock(const PolicyClassName& host);
 *   };
 * };
 * @endcode
 *
 * @{
 */

/**  @ingroup GrpThreadingModelPolicy  */
namespace threading_policy
{

#ifndef DEFAULT_THREADING
  /**
   * @def DEFAULT_THREADING
   * @brief  Selects the default threading model for certain components.
   *
   * If you don't define it, it defaults to single-threaded.
   * Some classes have configurable threading model;
   * DEFAULT_THREADING affects only default template arguments.
   */
# define DEFAULT_THREADING  threading_policy::SingleThreaded
#endif


////////////////////////////////////////////////////////////////////////////////
// class template SingleThreaded
////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Threading Model policy implements a single-threaded model.
 * @note @b No @b synchronization provided.
 * @tparam Host  Class which needs synchronization.
 * @sa @link GrpThreadingModelPolicy Threading Model policy specification @endlink
 */
template <class Host>
class SingleThreaded
{
public:
    typedef  Host  volatile_type;

    struct Lock
    {
        Lock() = default;
        explicit Lock(const SingleThreaded&) {}
        ~Lock() = default;
    };
};


////////////////////////////////////////////////////////////////////////////////
// class template ObjectLevelLockable
////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Threading Model policy implements a object-level locking scheme.
 * @note Separate synchronization for each object - reference to object required.
 * @tparam Host  Class which objects need synchronization.
 * @sa @link GrpThreadingModelPolicy Threading Model policy specification @endlink
 */
template <class Host>
class ObjectLevelLockable
{
private:
    /**
     * @brief Mutex for guarding Host object if its class @b inherits from
     *        ObjectLevelLockable.
     */
    mutable std::mutex mtx_;

public:
    typedef  typename std::add_volatile_t<Host>  volatile_type;

    ObjectLevelLockable() = default;
    ~ObjectLevelLockable() = default;

    class Lock;
    friend class Lock;  // the Lock needs access to ObjectLevelLockable::mtx_

    class Lock
    {
    private:
        /**  Object which the Lock is going to guard.  */
        const ObjectLevelLockable& host_;

    public:
        /**  @brief Unable to lock object without its reference.  */
        Lock() = delete;

        explicit Lock(const ObjectLevelLockable& host)
        : host_(host)
        {  host_.mtx_.lock();  }

        Lock(const Lock&) = delete;

        Lock& operator=(const Lock&) = delete;

        ~Lock()
        {  host_.mtx_.unlock();  }
    };
};


////////////////////////////////////////////////////////////////////////////////
// class template ClassLevelLockable
////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Threading Model policy implements a class-level locking scheme.
 * @note Synchronization for all class resources at once -
 *       no object reference required.
 * @tparam Host  Class which resources need synchronization.
 * @sa @link GrpThreadingModelPolicy Threading Model policy specification @endlink
 */
template <class Host>
class ClassLevelLockable
{
private:
    /**
     * @brief Mutex for guarding Host class objects if the Host class
     *        @b inherits from ClassLevelLockable.
     */
    static std::mutex mtx_;

public:
    typedef  typename std::add_volatile_t<Host>  volatile_type;

    class Lock;
    friend class Lock;  // the Lock needs access to ClassLevelLockable::mtx_

    class Lock
    {
    public:
        Lock()
        {  mtx_.lock();  }

        explicit Lock(const ClassLevelLockable&)
        {  mtx_.lock();  }

        Lock(const Lock&) = delete;

        Lock& operator=(const Lock&) = delete;

        ~Lock()
        {  mtx_.unlock();  }
    };
};

template <class Host>
std::mutex ClassLevelLockable<Host>::mtx_;

}  // namespace threading_policy
/** @} */  // @defgroup GrpThreadingModelPolicy
