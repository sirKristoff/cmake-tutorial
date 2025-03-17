#pragma once

#include <cstddef>  // for ptrdiff_t size_t
#include <functional>  // for function
#include <tuple>  // for tuple


#if defined(__has_cpp_attribute) && __has_cpp_attribute(nodiscard)  // 201603L
# define nodiscard_atr [[nodiscard]]
#else
# define nodiscard_atr
#endif

#ifndef __cpp_noexcept_function_type  // 201510L
# include <new>  // for bad_alloc
# define throws(exc)  //throw(exc) // deprecated in C++11
#else  // C++17
# define throws(_)  noexcept(false)
#endif


struct IStorage
{
    typedef  void*                                     pointer;
    typedef  const void*                               const_pointer;
    typedef  std::ptrdiff_t                            difference_type;
    typedef  std::size_t                               size_type;
    typedef  std::tuple<pointer, pointer>              element_range;
    typedef  std::tuple<const_pointer, const_pointer>  const_element_range;
    typedef  std::function<void(pointer)>              creator_f;
    typedef  std::function<void(pointer)>              destroyer_f;

    IStorage() = default;
    IStorage(const IStorage&) = default;
    IStorage(IStorage&&) = default;
    virtual ~IStorage() = default;


    /**
     * @brief Increase the capacity of the storage to a value that's greater or equal to new_cap.
     * @param new_cap  new capacity of the storage, in number of elements.
     * @param front_range  TODO:
     * @param wrapped_range  TODO:
     * @return  TODO:  element_range
     * @throw @c std::bad_alloc  if allocation fails.
     * @remark If an exception is thrown, this function has no effect.
     *
     * If new_cap is greater than the current capacity(), new storage space is allocated,
     * otherwise the function does nothing.
     * TODO: mention about copying data.
     */
    virtual element_range reserve(
        size_type new_cap,
        const_element_range front_range,
        const_element_range wrapped_range
    )  throws(std::bad_alloc) = 0;

    /**
     * @brief The number of elements that the storage has currently allocated space for.
     * @return the number of elements that the storage has currently allocated space for.
     */
    nodiscard_atr
    virtual size_type capacity()  const noexcept = 0;

    /**
     * @brief Requests the removal of unused capacity.
     * @param front_range  TODO:
     * @param wrapped_range  TODO:
     * @return  TODO:  element_range
     * @throw @c std::bad_alloc  if reallocation fails.
     * @remark If an exception is thrown, this function has no effect.
     */
    virtual element_range shrink_to_fit(
        const_element_range front_range,
        const_element_range wrapped_range
    )  throws(std::bad_alloc) = 0;

    nodiscard_atr
    virtual pointer begin()  noexcept = 0;
    nodiscard_atr
    virtual const_pointer begin()  const noexcept = 0;
    nodiscard_atr
    virtual pointer end()  noexcept = 0;
    nodiscard_atr
    virtual const_pointer end()  const noexcept = 0;

    /**
     * @brief Returns the number of hops from first to last.
     * @param first  Points to the first element.
     * @param last   Points to the end of the range.
     * @return The number of increments needed to go from first to last.
     * @note Return value may be negative if first is reachable from last.
     */
    virtual difference_type distance(const_pointer first, const_pointer last)  const noexcept = 0;

    /**
     * @brief Increments given iterator/pointer by one element.
     * @param p   pointer to be advanced.
     * @retval @c true  if advancing caused wrapped in cyclic buffer,
     *   aka @a p was equal @c end() and become next element behind @c begin().
     * @retval @c false  no wrapping - address of advanced pointer was raised.
     */
    virtual bool advance(pointer& p)  const noexcept = 0;
};


// #include <gmock/gmock.h>
#ifdef MOCK_METHOD
# include <algorithm>
# include <cstddef>  // for nullptr_t
# include <cstdint>  // for uintptr_t
# include <memory>  // for destroy_at
# include <new>  // for launder
# include <vector>

// TestCaseClass template parameter for make one instance of 'obj_tab_' per TestCase
// TEST(<test_suite_name>, <test_name>)  ->  class  <test_suite_name>_<test_name>_Test

template < class value_type, class TestCaseClass = std::nullptr_t >
class MockStorage : public IStorage
{
public:
    static std::vector<MockStorage*> obj_tab_;

    static std::vector<MockStorage*>& get_obj_tab_()
    {  return MockStorage::obj_tab_;  }

    virtual typename IStorage::pointer at_(typename IStorage::size_type idx) const
    {  return nullptr;  }

private:
    static bool obj_tab_remove(MockStorage* obj)
    {
        auto it = std::find(obj_tab_.begin(), obj_tab_.end(), obj);
        if (it == obj_tab_.end())
        {
            return false;
        }
        obj_tab_.erase(it);
        return true;
    }

public:
    MockStorage()
    {
        obj_tab_.push_back(this);
        EXPECT_CALL(*this, MockStorageDtor());
    }
    MockStorage(MockStorage&& src)
    {
        obj_tab_.push_back(this);
        EXPECT_CALL(*this, MockStorageDtor());
        if (!MockStorage::obj_tab_remove(&src))
        {
            ADD_FAILURE() << "Missing pointer to MockStorage in the 'obj_tab_'!";
        }
    }
    virtual ~MockStorage()
    {
        MockStorageDtor();
        if (!MockStorage::obj_tab_remove(this))
        {
            // TODO: mark test as failed or consider move scenario
            ADD_FAILURE() << "Missing pointer to MockStorage in the 'obj_tab_'!";
        }
    }

    virtual typename IStorage::creator_f make_creator(const value_type& value)
    {
        return [&](IStorage::pointer p) {
            this->value_typeCtor(p, value);
            EXPECT_CALL(*this, value_typeDtor(p));
        };
    }

    virtual typename IStorage::destroyer_f make_destroyer()
    {
        return [this](IStorage::pointer p) {
            this->value_typeDtor(p);
        };
    }

    MOCK_METHOD(
        void,
        MockStorageDtor, ()
    );
    MOCK_METHOD(
        void,
        value_typeCtor, (
            IStorage::pointer  p,
            const value_type&  value)
    );
    MOCK_METHOD(
        void,
        value_typeDtor, (
            IStorage::pointer  p)
    );


    MOCK_METHOD(
        IStorage::element_range,
        reserve, (
            IStorage::size_type  new_cap,
            IStorage::const_element_range  front_range,
            IStorage::const_element_range  wrapped_range
        ),(override)
    );
    MOCK_METHOD(
        IStorage::size_type,
        capacity, (
        ),(const, noexcept, override)
    );
    MOCK_METHOD(
        IStorage::element_range,
        shrink_to_fit, (
            IStorage::const_element_range  front_range,
            IStorage::const_element_range  wrapped_range
        ),(override)
    );
    MOCK_METHOD(
        IStorage::pointer,
        begin, (
        ),(noexcept, override)
    );
    MOCK_METHOD(
        IStorage::const_pointer,
        begin__const, (
        ),(const, noexcept)
    );
    IStorage::const_pointer
    begin() const noexcept override
    {  return begin__const();  }
    MOCK_METHOD(
        IStorage::pointer,
        end, (
        ),(noexcept, override)
    );
    MOCK_METHOD(
        IStorage::const_pointer,
        end__const, (
        ),(const, noexcept)
    );
    IStorage::const_pointer
    end() const noexcept override
    {  return end__const();  }
    MOCK_METHOD(
        IStorage::difference_type,
        distance, (
            IStorage::const_pointer first,
            IStorage::const_pointer last
        ),(const, noexcept, override)
    );
    MOCK_METHOD(
        bool,
        advance, (
            IStorage::pointer& p
        ),(const, noexcept, override)
    );
};

template < class T, class TCC >
std::vector<MockStorage<T, TCC>*> MockStorage<T, TCC>::obj_tab_{};

#endif  // MOCK_METHOD
