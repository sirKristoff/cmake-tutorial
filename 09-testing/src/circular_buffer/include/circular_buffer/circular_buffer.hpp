#pragma once

#include <cstddef>  // for size_t
#include <memory>  // for make_unique unique_ptr
#include <stdexcept>  // for range_error
#include <type_traits>  // for is_base_of is_same remove_cv
#include <utility>  // for move forward

#include "circular_buffer/istorage.hpp"



#if __cplusplus < 201402L
namespace std
{
template< class T >
std::unique_ptr<T> make_unique()
{
    return std::unique_ptr<T>(new T);
}

template< class T, class... Args  >
std::unique_ptr<T> make_unique( Args&&... args )
{
    return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}
}  // namespace std
#endif // < C++14

class circular_buffer_base
{
    class Impl;
    std::unique_ptr<Impl> pImpl_;

protected:
    typedef  void*              pointer;
    typedef  void*              const_pointer;
    typedef  std::size_t        size_type;

public:
    circular_buffer_base(std::unique_ptr<IStorage>&& storage);
    ~circular_buffer_base();

protected:
    pointer front();

    bool empty() const;
    size_type size() const;

    void clear(typename IStorage::destroyer_f destroyer)  noexcept;
    void push_back(typename IStorage::creator_f creator);
    void pop_front(typename IStorage::destroyer_f destroyer);

    IStorage& get_storage()  const noexcept;
};


template < class T, class StoragePolicy >
class circular_buffer : private circular_buffer_base
{
    static_assert(
        std::is_same<T, typename std::remove_cv<T>::type>::value,
        "circular_buffer must have a non-const, non-volatile value_type!");

    static_assert(
        std::is_base_of<IStorage, StoragePolicy>::value,
        "StoragePolicy has to derive from IStorage!");

#if __cplusplus > 201703L
    static_assert(
        std::is_invocable_r_v<
            typename IStorage::creator_f,
            decltype(&StoragePolicy::make_creator),
            StoragePolicy&,
            const T&>,
	    "StoragePolicy must define member function returning creator functor!");

    static_assert(
        std::is_invocable_r_v<
            typename IStorage::destroyer_f,
            decltype(&StoragePolicy::make_destroyer),
            StoragePolicy&>,
	    "StoragePolicy must define member function returning destroyer functor!");
#endif // C++17


public:
    typedef          circular_buffer_base  Base;
    typedef          StoragePolicy         storage_type;
    typedef          T                     value_type;
    typedef          T&                    reference;
    typedef          const T&              const_reference;
    typedef          T*                    pointer;
    typedef          const T*              const_pointer;
    typedef typename Base::size_type       size_type;

    circular_buffer()
    : circular_buffer_base(std::make_unique<StoragePolicy>())
    {}

    circular_buffer(StoragePolicy&& storage)
    : circular_buffer_base(std::make_unique<StoragePolicy>(std::move(storage)))
    {}

    ~circular_buffer()
    {
        clear();
    }

    // -------------------------------------------------------------------------
    // Element access
    // -------------------------------------------------------------------------

    reference front()  throws(std::range_error)
    {
        auto pFront = static_cast<pointer>(Base::front());  // TODO: consider using std::launder
        if (nullptr == pFront)
            throw std::range_error{"Empty circular_buffer has no front element!"};
        return *pFront;
    }

    // -------------------------------------------------------------------------
    // Capacity
    // -------------------------------------------------------------------------

    using Base::empty;
    using Base::size;

    // -------------------------------------------------------------------------
    // Modifiers
    // -------------------------------------------------------------------------

    void clear()  noexcept
    {
        typename IStorage::destroyer_f destroyer_functor =
            static_cast<StoragePolicy&>(get_storage()).make_destroyer();
        Base::clear(destroyer_functor);
    }

    void push_back(const value_type& value)
    {
        typename IStorage::creator_f creator_functor =
            static_cast<StoragePolicy&>(get_storage()).make_creator(value);
        Base::push_back(creator_functor);
    }

    void pop_front()
    {
        typename IStorage::destroyer_f destroyer_functor =
            static_cast<StoragePolicy&>(get_storage()).make_destroyer();
        Base::pop_front(destroyer_functor);
    }
};
