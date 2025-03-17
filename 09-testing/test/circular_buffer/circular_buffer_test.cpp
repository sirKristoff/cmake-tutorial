#include <gmock/gmock.h>

#include "circular_buffer/circular_buffer.hpp"
#include "circular_buffer/storage.hpp"



////////////////////////////////////////////////////////////////////////////////
// Helper definitions
////////////////////////////////////////////////////////////////////////////////

using ::testing::_;
using ::testing::Eq;

template < class TCClass >
using mock_storage_t = storage_policy::MockRealStorage<
        unsigned char, storage_policy::FixedSizeStorage<unsigned char, 3u>, TCClass>;


////////////////////////////////////////////////////////////////////////////////
// Test Cases
////////////////////////////////////////////////////////////////////////////////


TEST(circular_buffer_FixedSizeStorage_TC, DefaultCtor)
{
    typedef  circular_buffer<int, storage_policy::FixedSizeStorage<int, 5u>>  buffer_t;
    buffer_t  buf{};

    EXPECT_TRUE(buf.empty());
    EXPECT_EQ(0u, buf.size());
    EXPECT_THROW(buf.front(), std::range_error);
    EXPECT_NO_THROW(buf.pop_front());
}


////////////////////////////////////////////////////////////////////////////////
// Helper definitions
////////////////////////////////////////////////////////////////////////////////

#define storage_capacity  3u
#define is_wrapped(HEAD, TAIL) ((TAIL)<(HEAD))
#define calc_advancing(ITER)  ((ITER==storage_capacity-1)?(0):(ITER+1))
#define calc_size(HEAD, TAIL)  ((is_wrapped(HEAD, TAIL)?(storage_capacity):(0u)) + TAIL-(HEAD))

#define expect_call_size(HEAD, TAIL)  \
    EXPECT_CALL(storage, distance(Eq(storage.at_(HEAD)), Eq(storage.at_(TAIL))));  \
    if constexpr(is_wrapped(HEAD, TAIL)) {  \
        EXPECT_CALL(storage, capacity);  \
    }

#define expect_call_clear(HEAD, TAIL)  \
    if constexpr(HEAD != TAIL) {  \
        if constexpr(is_wrapped(HEAD, TAIL)) {  \
            EXPECT_CALL(storage, begin);  \
        }  \
        EXPECT_CALL(storage, advance(_)).Times(calc_size(HEAD, TAIL));  \
        EXPECT_CALL(storage, shrink_to_fit(  \
            Eq(IStorage::const_element_range{storage.at_(TAIL), storage.at_(TAIL)}),  \
            Eq(IStorage::const_element_range{})));  \
    }

#define expect_call_push_back(HEAD, TAIL, VALUE) {  \
    EXPECT_CALL(storage, distance(Eq(storage.at_(HEAD)), Eq(storage.at_(TAIL))));  \
    if constexpr(is_wrapped(HEAD, TAIL)) {  \
        EXPECT_CALL(storage, capacity).Times(2);  \
    } else { \
        EXPECT_CALL(storage, capacity);  \
    }  \
    if constexpr(calc_size(HEAD, TAIL) == (storage_capacity -1)) { /* need reserve more space*/ \
        if constexpr(is_wrapped(HEAD, TAIL)) {  \
            EXPECT_CALL(storage, end);  \
            EXPECT_CALL(storage, begin);  \
            EXPECT_CALL(storage, reserve(_,  \
                Eq(IStorage::const_element_range{storage.at_(HEAD), storage.at_(storage_capacity)}),  \
                Eq(IStorage::const_element_range{storage.at_(0), storage.at_(TAIL)})));  \
        } else {  \
            EXPECT_CALL(storage, reserve(_,  \
                Eq(IStorage::const_element_range{storage.at_(HEAD), storage.at_(TAIL)}),  \
                _));  \
        }  \
    } else { \
        EXPECT_CALL(storage, value_typeCtor(Eq(storage.at_(TAIL)), VALUE));  \
        EXPECT_CALL(storage, advance(Eq(storage.at_(TAIL))));  \
    }  \
}

#define expect_call_pop_front(HEAD, TAIL)  \
    if constexpr(HEAD != TAIL) {  \
        constexpr signed long long advanced_head = calc_advancing(HEAD);  \
        EXPECT_CALL(storage, advance(Eq(storage.at_(HEAD))));  \
        if constexpr(is_wrapped(advanced_head, TAIL)) {  \
            EXPECT_CALL(storage, end);  \
            EXPECT_CALL(storage, begin);  \
            EXPECT_CALL(storage, shrink_to_fit(  \
                Eq(IStorage::const_element_range{storage.at_(advanced_head), storage.at_(storage_capacity)}),  \
                Eq(IStorage::const_element_range{storage.at_(0), storage.at_(TAIL)})));  \
        } else {  \
            EXPECT_CALL(storage, shrink_to_fit(  \
                Eq(IStorage::const_element_range{storage.at_(advanced_head), storage.at_(TAIL)}),  \
                Eq(IStorage::const_element_range{})));  \
        }  \
    }


////////////////////////////////////////////////////////////////////////////////
// Test Cases
////////////////////////////////////////////////////////////////////////////////


TEST(circular_buffer_MockRealStorage_TC, DefaultCtor)
{
    typedef  mock_storage_t<decltype(*this)>  storage_t;
    typedef  circular_buffer<typename storage_t::value_type, storage_t>  buffer_t;
    {
    buffer_t  buf{};

    EXPECT_EQ(1u, storage_t::obj_tab_.size());
    auto& storage = *storage_t::obj_tab_.front();

    EXPECT_TRUE(buf.empty());
    expect_call_size(0, 0);
    EXPECT_EQ(0u, buf.size());
    EXPECT_THROW(buf.front(), std::range_error);
    expect_call_pop_front(0, 0);
    EXPECT_NO_THROW(buf.pop_front());
    }
    EXPECT_EQ(0u, storage_t::obj_tab_.size());
}



TEST(circular_buffer_MockRealStorage_TC, PushOneCheckDtor)
{
    typedef  mock_storage_t<decltype(*this)>  storage_t;
    typedef  circular_buffer<typename storage_t::value_type, storage_t>  buffer_t;
    {
    buffer_t  buf{};
    ASSERT_EQ(1u, storage_t::obj_tab_.size());
    auto& storage = *storage_t::obj_tab_.front();

    expect_call_push_back(0, 0, 'a');
    buf.push_back('a');

    EXPECT_FALSE(buf.empty());
    expect_call_size(0, 1);
    EXPECT_EQ(1u, buf.size());
    EXPECT_EQ('a', buf.front());

    // Removing all elements while destructing buffer
    expect_call_clear(0, 1);
    }
    EXPECT_EQ(0u, storage_t::obj_tab_.size());
}



TEST(circular_buffer_MockRealStorage_TC, DoWrap)
{
    typedef  mock_storage_t<decltype(*this)>  storage_t;
    typedef  circular_buffer<typename storage_t::value_type, storage_t>  buffer_t;
    {
    const std::array<typename buffer_t::value_type, 5u> vals = {'b', 'c', 'e', 'g', 'k'};
    buffer_t  buf{};
    ASSERT_EQ(1u, storage_t::obj_tab_.size());
    auto& storage = *storage_t::obj_tab_.front();

    // -------------------------------------------------------------------------
    // push 1st value
    //                    H  T
    expect_call_push_back(0, 0, vals[0]);
    buf.push_back(vals[0]);

    //               H  T
    expect_call_size(0, 1);
    EXPECT_EQ(1u, buf.size());
    EXPECT_EQ(vals[0], buf.front());
    EXPECT_FALSE(buf.empty());

    // -------------------------------------------------------------------------
    // push 2nd value
    //                    H  T
    expect_call_push_back(0, 1, vals[1]);
    buf.push_back(vals[1]);

    //               H  T
    expect_call_size(0, 2);
    EXPECT_EQ(2u, buf.size());
    EXPECT_EQ(vals[0], buf.front());  // front element not changed
    EXPECT_FALSE(buf.empty());

    // -------------------------------------------------------------------------
    // pop 1st element
    //                    H  T
    expect_call_pop_front(0, 2);
    buf.pop_front();

    //               H  T
    expect_call_size(1, 2);
    EXPECT_EQ(1u, buf.size());
    EXPECT_EQ(vals[1], buf.front());
    EXPECT_FALSE(buf.empty());

    // -------------------------------------------------------------------------
    // push 3rd value  - wrapping storage, in theory
    //                    H  T
    expect_call_push_back(1, 2, vals[2]);

    buf.push_back(vals[2]);

    //               H  T
    expect_call_size(1, 0);
    EXPECT_EQ(2u, buf.size());
    EXPECT_EQ(vals[1], buf.front());  // front element not changed
    EXPECT_FALSE(buf.empty());

    // -------------------------------------------------------------------------
    // pop 2nd element
    //                    H  T
    expect_call_pop_front(1, 0);
    buf.pop_front();

    //               H  T
    expect_call_size(2, 0);
    EXPECT_EQ(1u, buf.size());
    EXPECT_EQ(vals[2], buf.front());
    EXPECT_FALSE(buf.empty());

    // -------------------------------------------------------------------------
    // push 4th value  - wrapping storage, elements are not in continues memory space
    //                    H  T
    expect_call_push_back(2, 0, vals[3]);
    buf.push_back(vals[3]);

    //               H  T
    expect_call_size(2, 1);
    EXPECT_EQ(2u, buf.size());
    EXPECT_EQ(vals[2], buf.front());  // front element not changed
    EXPECT_FALSE(buf.empty());

    // -------------------------------------------------------------------------
    // Removing all elements while destructing buffer
    expect_call_clear(2, 1);
    }
    EXPECT_EQ(0u, storage_t::obj_tab_.size());
}



TEST(circular_buffer_MockRealStorage_TC, ExceedCapacity)
{
    typedef  mock_storage_t<decltype(*this)>  storage_t;
    typedef  circular_buffer<typename storage_t::value_type, storage_t>  buffer_t;
    {
    const std::array<typename buffer_t::value_type, 5u> vals = {'b', 'c', 'e', 'g', 'k'};
    buffer_t  buf{};
    ASSERT_EQ(1u, storage_t::obj_tab_.size());
    auto& storage = *storage_t::obj_tab_.front();

    // -------------------------------------------------------------------------
    // push 1st and 2nd value
    //                    H  T
    expect_call_push_back(0, 0, vals[0]);
    buf.push_back(vals[0]);

    expect_call_push_back(0, 1, vals[1]);
    buf.push_back(vals[1]);

    //               H  T
    expect_call_size(0, 2);
    EXPECT_EQ(2u, buf.size());

    // -------------------------------------------------------------------------
    // push 3rd value  - capacity exceeded
    //                    H  T
    expect_call_push_back(0, 2, vals[2]);

    EXPECT_THROW(buf.push_back(vals[2]), std::bad_alloc);

    //               H  T
    expect_call_size(0, 2);
    ASSERT_EQ(2u, buf.size());
    ASSERT_FALSE(buf.empty());

    // -------------------------------------------------------------------------
    // pop one element
    EXPECT_EQ(vals[0], buf.front());
    //                    H  T
    expect_call_pop_front(0, 2);
    buf.pop_front();

    //               H  T
    expect_call_size(1, 2);
    EXPECT_EQ(1u, buf.size());
    EXPECT_EQ(vals[1], buf.front());
    EXPECT_FALSE(buf.empty());

    // -------------------------------------------------------------------------
    // push 4th value  - wrapping storage
    //                    H  T
    expect_call_push_back(1, 2, vals[3]);
    buf.push_back(vals[3]);

    //               H  T
    expect_call_size(1, 0);
    EXPECT_EQ(2u, buf.size());
    EXPECT_EQ(vals[1], buf.front());
    EXPECT_FALSE(buf.empty());

    // -------------------------------------------------------------------------
    // push 5th value  - capacity exceeded
    //                    H  T
    expect_call_push_back(1, 0, vals[4]);

    EXPECT_THROW(buf.push_back(vals[4]), std::bad_alloc);

    //               H  T
    expect_call_size(1, 0);
    EXPECT_EQ(2u, buf.size());
    EXPECT_FALSE(buf.empty());

    // -------------------------------------------------------------------------
    // check value of elements in buffer
    EXPECT_EQ(vals[1], buf.front());
    //                    H  T
    expect_call_pop_front(1, 0);
    buf.pop_front();
    EXPECT_EQ(vals[3], buf.front());

    expect_call_pop_front(2, 0);
    buf.pop_front();
    EXPECT_THROW(buf.front(), std::range_error);
    EXPECT_TRUE(buf.empty());

    // -------------------------------------------------------------------------
    // Removing all elements while destructing buffer
    expect_call_clear(0, 0);
    }
    EXPECT_EQ(0u, storage_t::obj_tab_.size());
}
