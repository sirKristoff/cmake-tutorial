#pragma once

#include "circular_buffer/istorage.hpp"

#include <array>
#include <cassert>
#include <cstdint>  // for uintptr_t


#if __cplusplus >= 201703L
# include <memory>  // for destroy_at
# include <new>  // for launder
#else
namespace std
{
template< class T >
void destroy_at( T* p )
{
    p->~T();
}

template< class T >
T* launder( T* p ) noexcept
{
    return p;
}
}  // namespace std
#endif // < C++17


namespace storage_policy
{

template < class T, std::size_t N >
class FixedSizeStorage : public IStorage
{
public:
    typedef          T                                                  value_type;
    typedef typename std::aligned_storage<sizeof(T), alignof(T)>::type  element_type;

    element_type data_[N];


    FixedSizeStorage() = default;
    FixedSizeStorage(const FixedSizeStorage&) = default;
    FixedSizeStorage(FixedSizeStorage&&) = default;
    virtual ~FixedSizeStorage() = default;

    virtual element_range reserve(
        size_type /*new_cap*/,
        const_element_range /*front_range*/,
        const_element_range /*wrapped_range*/
    )  throws(std::bad_alloc) override
    {
        throw std::bad_alloc{};
    }

    nodiscard_atr
    virtual size_type capacity()  const noexcept override
    {
        return N;
    };

    virtual element_range shrink_to_fit(
        const_element_range front_range,
        const_element_range wrapped_range
    )  throws(std::bad_alloc) override
    {
        if (const_element_range{} == wrapped_range)
            return element_range(
                const_cast<pointer>(std::get<0>(front_range)),
                const_cast<pointer>(std::get<1>(front_range))
            );
        else
            return element_range(
                const_cast<pointer>(std::get<0>(front_range)),
                const_cast<pointer>(std::get<1>(wrapped_range))
            );
    }

    nodiscard_atr
    virtual pointer begin()  noexcept override
    {  return &data_[0];  }
    nodiscard_atr
    virtual const_pointer begin()  const noexcept override
    {  return &data_[0];  }
    nodiscard_atr
    virtual pointer end()  noexcept override
    {  return &data_[N];  }
    nodiscard_atr
    virtual const_pointer end()  const noexcept override
    {  return &data_[N];  }

    virtual difference_type distance(const_pointer first, const_pointer last)  const noexcept override
    {
        auto pFirst = reinterpret_cast<const element_type*>(first);
        auto pLast = reinterpret_cast<const element_type*>(last);
        return pLast - pFirst;
    }

    virtual bool advance(pointer& p)  const noexcept override
    {
        assert(
            (uintptr_t)&data_[0] <= (uintptr_t)p &&
            (uintptr_t)p <= (uintptr_t)&data_[N] &&
            "Address does not belong to underlying storage!"
        );

        auto ptr = static_cast<element_type*>(p);
        if (&data_[N-1] == ptr)
        {  // advanced pointer always points to existing element
            p = reinterpret_cast<pointer>(const_cast<element_type*>(&data_[0]));
            return true;
        }

        p = reinterpret_cast<pointer>(ptr + 1);
        return false;
    }

    typename IStorage::creator_f make_creator(const value_type& value)
    {
        return [&](IStorage::pointer p) -> void {
            (void) ::new(p) value_type(value);
        };
    }

    typename IStorage::destroyer_f make_destroyer()
    {
        return [](IStorage::pointer p) -> void {
            std::destroy_at(std::launder(reinterpret_cast<T*>(p)));
        };
    }
};



// #include <gmock/gmock.h>
#ifdef MOCK_METHOD

#define MockStorage_ON_CALL  \
    ON_CALL(*this, MockStorageDtor).WillByDefault([this] () {});  \
    ON_CALL(*this, value_typeCtor).WillByDefault(  \
        [this] (IStorage::pointer  p,  \
                const value_type&  value) -> void {  \
            return (real_.make_creator(value))(p);  \
    });  \
    ON_CALL(*this, value_typeDtor).WillByDefault(  \
        [this] (IStorage::pointer  p) -> void {  \
            (real_.make_destroyer())(p);  \
    });  \
  \
    ON_CALL(*this, reserve).WillByDefault(  \
        [this] (IStorage::size_type  new_cap,  \
                IStorage::const_element_range  front_range,  \
                IStorage::const_element_range  wrapped_range) {  \
            return real_.reserve(new_cap, front_range, wrapped_range);  \
    });  \
    ON_CALL(*this, capacity).WillByDefault(  \
        [this] () {  \
            return real_.capacity();  \
    });  \
    ON_CALL(*this, shrink_to_fit).WillByDefault(  \
        [this] (IStorage::const_element_range  front_range,  \
                IStorage::const_element_range  wrapped_range) {  \
            return real_.shrink_to_fit(front_range, wrapped_range);  \
    });  \
    ON_CALL(*this, begin).WillByDefault(  \
        [this] () {  \
            return real_.begin();  \
    });  \
    ON_CALL(::testing::Const(*this), begin__const).WillByDefault(  \
        [this] () {  \
            return const_cast<const RealStorage&>(real_).begin();  \
    });  \
    ON_CALL(*this, end).WillByDefault(  \
        [this] () {  \
            return real_.end();  \
    });  \
    ON_CALL(::testing::Const(*this), end__const).WillByDefault(  \
        [this] () {  \
            return const_cast<const RealStorage&>(real_).end();  \
    });  \
    ON_CALL(*this, distance).WillByDefault(  \
        [this] (IStorage::const_pointer first,  \
                IStorage::const_pointer last) {  \
            return real_.distance(first, last);  \
    });  \
    ON_CALL(*this, advance).WillByDefault(  \
        [this] (IStorage::pointer& p) {  \
            return real_.advance(p);  \
    });


template
<
    class T,
    class RealStorage,  // FixedSizeStorage<T, N>
    class TestCaseClass = std::nullptr_t
>
struct MockRealStorage : public MockStorage<T, TestCaseClass>
{
    typedef  MockStorage<T, TestCaseClass>  Base;
    typedef  T                              value_type;

    RealStorage real_;

    virtual typename IStorage::pointer at_(typename IStorage::size_type idx) const
    {  return const_cast<typename RealStorage::element_type*>(&real_.data_[idx]);  }

    MockRealStorage()
    : Base(), real_()
    {
        EXPECT_CALL(*this, begin());
        MockStorage_ON_CALL
    }

    MockRealStorage(MockRealStorage&& src)
    : Base(std::move(src)), real_(std::move(src.real_))
    {
        MockStorage_ON_CALL
    }

    virtual ~MockRealStorage() = default;
};

#endif  // MOCK_METHOD

}  // namespace storage_policy
