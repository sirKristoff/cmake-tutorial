#include "prio_containers/priority_stack.hpp"

#include <deque>
#include <gtest/gtest.h>
#include <iostream>
#include <string>


#ifndef __cpp_lib_exchange_function  // 201304L
namespace std {
template<class T, class U = T>
T exchange(T& obj, U&& new_value)
{
    T old_value = std::move(obj);
    obj = std::forward<U>(new_value);
    return old_value;
}
}
#endif  // ! __cpp_lib_exchange_function

#ifndef __cpp_lib_quoted_string_io  // 201304L
namespace std {
template< class String >
String quoted( const String str)
{  return "\"" + str + "\"";  }
}
#endif  // ! __cpp_lib_quoted_string_io


////////////////////////////////////////////////////////////////////////////////
// Helper definitions
////////////////////////////////////////////////////////////////////////////////

struct Prio
{
    int prio;
    std::string value;

    Prio() = default;
    Prio(const Prio&) = default;
    Prio(Prio&& src) : prio(std::exchange(src.prio, 0)), value(std::move(src.value)) {}
    Prio(int p, const std::string& v) : prio(p), value(v) {}
    ~Prio() = default;
    Prio& operator=(const Prio&) = default;
    Prio& operator=(Prio&& rhs)
    {  prio = std::exchange(rhs.prio, 0); value = std::move(rhs.value); return *this;  } ;

    bool operator<(const Prio& rhs) const
    {  return this->prio < rhs.prio;  }
    bool operator>(const Prio& rhs) const
    {  return this->prio > rhs.prio;  }

    bool operator==(const Prio& rhs) const
    {  return this->prio == rhs.prio && this->value == rhs.value;  }

    friend std::ostream& operator<<(std::ostream& ostr, const Prio& obj)
    { ostr << "{" << obj.prio << "," << std::quoted(obj.value) << "}";  return ostr;  }
};

typedef  priority_stack<int>  IntStack_t;
typedef  priority_stack<Prio>  PrioStack_t;
typedef  typename PrioStack_t::container_type  PrioContainer_t;


#define VERIFY_STACK_EMPTY(stack_){  \
    EXPECT_EQ(stack_.begin(), stack_.end());  \
    EXPECT_EQ(stack_.cbegin(), stack_.cend());  \
    EXPECT_EQ(stack_.rbegin(), stack_.rend());  \
    EXPECT_EQ(stack_.crbegin(), stack_.crend());  \
    EXPECT_TRUE(stack_.empty());  \
    EXPECT_EQ(0, (stack_.end() - stack_.begin()));  \
    EXPECT_EQ(0u, stack_.size());  \
}

#define VERIFY_STACK_SIZE(expectedSize_, stack_){  \
    EXPECT_NE(stack_.begin(), stack_.end());  \
    EXPECT_NE(stack_.cbegin(), stack_.cend());  \
    EXPECT_NE(stack_.rbegin(), stack_.rend());  \
    EXPECT_NE(stack_.crbegin(), stack_.crend());  \
    EXPECT_FALSE(stack_.empty());  \
    EXPECT_EQ(expectedSize_, (stack_.end() - stack_.begin()));  \
    EXPECT_EQ(expectedSize_, stack_.size());  \
}

#define VERIFY_STACK_ELEMS(expected_, stack_){  \
    auto stackItr = stack_.begin();  \
    for (unsigned pos = 0u; pos < expected_.size(); ++pos, ++stackItr)  \
    {  \
        ASSERT_NE(stackItr, stack_.end())  << "Missing items in stack!";  \
        EXPECT_EQ(expected_[pos], *stackItr)  << "pos: " << pos;  \
    }  \
    EXPECT_EQ(stackItr, stack_.end())  << "Unexpected element in stack!";  \
}

#define VERIFY_STACK_POPING_ELEMS(expected_, stack_){  \
    for (unsigned pos = 0u; pos < expected_.size(); ++pos)  \
    {  \
        ASSERT_FALSE(stack_.empty())  << "Missing items in stack!";  \
        EXPECT_EQ(expected_[pos], stack_.top())  << "pos: " << pos;  \
        stack_.pop();  \
    }  \
    EXPECT_TRUE(stack_.empty())  << "Unexpected element in stack!";  \
}


////////////////////////////////////////////////////////////////////////////////
// Test data
////////////////////////////////////////////////////////////////////////////////

const int prio = 13;
const std::string value{"string value"};
const Prio theObj{prio, value};
const Prio theObj314{314, "pi"};
const Prio emptyObj{0, ""};

const PrioContainer_t thePrios = {{20, "20p-0"}, {18, "18p-1"}, {23, "23p-2"}, {20, "20p-3"}, {20, "20p-4"}};

#define printContainer(cont_) {  \
    std::string container_name = #cont_;  \
    std::cerr << container_name << ":";  \
    for (unsigned i = std::max(0, 14-(int)container_name.size()); i; --i)  \
        std::cerr << " ";  \
    for (auto& elem : cont_)  \
        std::cerr << elem << ", ";  \
    std::cerr << "\n";  \
}


////////////////////////////////////////////////////////////////////////////////
// Test Cases
////////////////////////////////////////////////////////////////////////////////

// priority_stack()
TEST(priority_stack_EmptyStack_TC, DefaultCtor)
{
    IntStack_t stack{};

    VERIFY_STACK_EMPTY(stack);
}


// priority_stack( const Compare& )
TEST(priority_stack_EmptyStack_TC, CompareCtor)
{
    // custom comparison functor
    auto compare = [](long lhs, long rhs) {return std::greater<long>{}(lhs,rhs); };
    const priority_stack<long, std::deque<long>, decltype(compare)> stack{ compare };

    VERIFY_STACK_EMPTY(stack);
}


// priority_stack( const Alloc& )
TEST(priority_stack_EmptyStack_TC, AllocCtor)
{
    IntStack_t stack{ IntStack_t::allocator_type{} };

    VERIFY_STACK_EMPTY(stack);
}


// priority_stack( const Compare&, const Alloc& )
TEST(priority_stack_EmptyStack_TC, CompareAllocCtor)
{
    IntStack_t stack{
      IntStack_t::value_compare{},
      IntStack_t::allocator_type{} };

    VERIFY_STACK_EMPTY(stack);
}


// void push( const value_type& )
// void pop()
TEST(priority_stack_PushOne_TC, Value)
{
    PrioStack_t stack{};

    stack.push(theObj);

    VERIFY_STACK_SIZE(1, stack);
    EXPECT_EQ(theObj, stack.top());

    stack.pop();

    VERIFY_STACK_EMPTY(stack);
}


// void push( value_type&& )
TEST(priority_stack_PushOne_TC, MoveTemporary)
{
    PrioStack_t stack{};

    stack.push(Prio{prio, value});

    VERIFY_STACK_SIZE(1, stack);
    EXPECT_EQ(theObj, stack.top());
    EXPECT_EQ(prio, stack.top().prio);
    EXPECT_EQ(value, stack.top().value);
}


// void push( value_type&& )
TEST(priority_stack_PushOne_TC, MoveObj)
{
    PrioStack_t stack{};
    Prio obj{prio, value};

    stack.push(std::move(obj));

    VERIFY_STACK_SIZE(1, stack);
    EXPECT_EQ(prio, stack.top().prio);
    EXPECT_EQ(value, stack.top().value);
    EXPECT_EQ(emptyObj, obj);
}


// void emplace( Args&&... args )
TEST(priority_stack_PushOne_TC, Emplace)
{
    PrioStack_t stack{};
    stack.emplace(prio, value);

    VERIFY_STACK_SIZE(1, stack);
    EXPECT_EQ(prio, stack.top().prio);
    EXPECT_EQ(value, stack.top().value);
}


//  priority_stack( const Container& )
TEST(priority_stack_CopyContainer_TC, Reference)
{
    const PrioContainer_t container =
        {{20,"20p-0"}, {18,"18p-1"}, {23,"23p-2"}, {20,"20p-3"}, {20,"20p-4"}};
    const PrioContainer_t expectedOrder =
        {{23,"23p-2"}, {20,"20p-4"}, {20,"20p-3"}, {20,"20p-0"}, {18,"18p-1"}};
    ASSERT_EQ(container.size(), expectedOrder.size())  << "Inconsistent test data!";

    PrioStack_t stack{container};

    printContainer(container);
    printContainer(expectedOrder);
    printContainer(stack);

    VERIFY_STACK_SIZE(expectedOrder.size(), stack);
    VERIFY_STACK_ELEMS(expectedOrder, stack);
    VERIFY_STACK_POPING_ELEMS(expectedOrder, stack);
}


//  priority_stack( const Container&, const Alloc& )
TEST(priority_stack_CopyContainer_TC, Alloc)
{
    const PrioContainer_t container =
        {{0,"0p-3"}, {0,"0p-2"}, {0,"0p-1"}, {0,"0p-0"}};
    const PrioContainer_t expectedOrder =
        {{0,"0p-0"}, {0,"0p-1"}, {0,"0p-2"}, {0,"0p-3"}};
    ASSERT_EQ(container.size(), expectedOrder.size())  << "Inconsistent test data!";

    PrioStack_t stack{container, PrioStack_t::allocator_type{}};

    printContainer(container);
    printContainer(expectedOrder);
    printContainer(stack);

    VERIFY_STACK_SIZE(expectedOrder.size(), stack);
    VERIFY_STACK_ELEMS(expectedOrder, stack);
    VERIFY_STACK_POPING_ELEMS(expectedOrder, stack);
}


//  priority_stack( const Compare&, const Container& )
TEST(priority_stack_CopyContainer_TC, OppositeCompare)
{
    typedef  priority_stack< Prio, PrioContainer_t,
                             std::greater< typename PrioContainer_t::value_type > >
             PrioStackOpComp_t;

    const PrioContainer_t container =
        {{5,"5p-5"}, {1,"1p-1"}, {7,"7p-6"}, {5,"5p-4"}, {1,"1p-0"}, {5,"5p-3"}, {2,"2p-2"}};
    const PrioContainer_t expectedOrder =
        {{1,"1p-0"}, {1,"1p-1"}, {2,"2p-2"}, {5,"5p-3"}, {5,"5p-4"}, {5,"5p-5"}, {7,"7p-6"}};
    ASSERT_EQ(container.size(), expectedOrder.size())  << "Inconsistent test data!";

    PrioStackOpComp_t stack{
        typename PrioStackOpComp_t::value_compare{},
        container};

    printContainer(container);
    printContainer(expectedOrder);
    printContainer(stack);

    VERIFY_STACK_SIZE(expectedOrder.size(), stack);
    VERIFY_STACK_ELEMS(expectedOrder, stack);
    VERIFY_STACK_POPING_ELEMS(expectedOrder, stack);
}


//  priority_stack( const Compare&, const Container&, const Alloc& )
TEST(priority_stack_CopyContainer_TC, OppositeCompareAlloc)
{
    typedef  priority_stack< Prio, PrioContainer_t,
                             std::greater< typename PrioContainer_t::value_type > >
             PrioStackOpComp_t;

    const PrioContainer_t container =
        {{5,"5p-5"}, {1,"1p-1"}, {7,"7p-6"}, {5,"5p-4"}, {1,"1p-0"}, {5,"5p-3"}, {2,"2p-2"}};
    const PrioContainer_t expectedOrder =
        {{1,"1p-0"}, {1,"1p-1"}, {2,"2p-2"}, {5,"5p-3"}, {5,"5p-4"}, {5,"5p-5"}, {7,"7p-6"}};
    ASSERT_EQ(container.size(), expectedOrder.size())  << "Inconsistent test data!";


    PrioStackOpComp_t stack{
        typename PrioStackOpComp_t::value_compare{},
        container,
        typename PrioStack_t::allocator_type{}};

    printContainer(container);
    printContainer(expectedOrder);
    printContainer(stack);

    VERIFY_STACK_SIZE(expectedOrder.size(), stack);
    VERIFY_STACK_ELEMS(expectedOrder, stack);
    VERIFY_STACK_POPING_ELEMS(expectedOrder, stack);
}


//  priority_stack( const Container&& )
TEST(priority_stack_MoveContainer_TC, RvReference)
{
    PrioContainer_t container =
        {{5,"5p-3"}, {1,"1p-6"}, {7,"7p-0"}, {5,"5p-2"}, {1,"1p-5"}, {5,"5p-1"}, {2,"2p-4"}};
    const PrioContainer_t expectedOrder =
        {{7,"7p-0"}, {5,"5p-1"}, {5,"5p-2"}, {5,"5p-3"}, {2,"2p-4"}, {1,"1p-5"}, {1,"1p-6"}};
    ASSERT_EQ(container.size(), expectedOrder.size())  << "Inconsistent test data!";

    printContainer(container);
    printContainer(expectedOrder);

    PrioStack_t stack{std::move(container)};
    printContainer(stack);

    EXPECT_TRUE(container.empty())  << "Source container is not being moved!";

    VERIFY_STACK_SIZE(expectedOrder.size(), stack);
    VERIFY_STACK_ELEMS(expectedOrder, stack);
    VERIFY_STACK_POPING_ELEMS(expectedOrder, stack);
}

// TODO: write more tests
