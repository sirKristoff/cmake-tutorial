#pragma once

#include <functional>  // for less
#include <vector>
#include <type_traits>  // for is_same

template
<
    class T,
    class Container = std::vector<T>
>
class stack
{
#if __cplusplus >= 201703L
    // Requirements on the first template parameter of container adaptors
    static_assert(std::is_same<_Tp, typename Container::value_type>::value,
	    "value_type must be the same as the underlying container");
#endif // C++17

protected:
    Container c_;

public:
    typedef          Container                  container_type;
    typedef typename Container::value_type      value_type;
    typedef typename Container::size_type       size_type;
    typedef typename Container::reference       reference;
    typedef typename Container::const_reference const_reference;

    stack();

    explicit stack( const Container& cont );

    explicit stack( Container&& cont );

    stack( const stack& src );  // (implicitly declared)

    stack( stack&& src );  // (implicitly declared)

    template< class Alloc >
    explicit stack( const Alloc& alloc );

    template< class Alloc >
    stack( const Container& cont, const Alloc& alloc );

    template< class Alloc >
    stack( Container&& cont, const Alloc& alloc );

    template< class Alloc >
    stack( const stack& src, const Alloc& alloc );

    template< class Alloc >
    stack( stack&& src, const Alloc& alloc );

#if  __cplusplus >= 202002L  // (since C++20)
    template< class InputIt >
    stack( InputIt first, InputIt last );

    template< class InputIt, class Alloc >
    stack( InputIt first, InputIt last, const Alloc& alloc );
#endif // C++20

#if  __cplusplus >= 202302L  // (since C++23)
    template< container-compatible-range<T> R>
    stack( std::from_range_t, R&& rg );

    template< container-compatible-range<T> R, class Alloc >
    stack( std::from_range_t, R&& rg, const Alloc& alloc );
#endif // C++23

    /**
     * @brief Destructs the stack.
     *
     * The destructors of the elements are called and the used storage is deallocated.
     * @note If the elements are pointers, the pointed-to objects are not destroyed.
     */
    ~stack();

    stack& operator=( const stack& rhs );  // (implicitly declared)

    stack& operator=( stack&& rhs );  // (implicitly declared)

    // Element access

    /**
     * @brief Accesses the top element.
     * @return reference to an element.
     *
     * Returns reference to the top element in the stack. This is the most recently pushed element.
     * This element will be removed on a call to pop().
     */
    reference top();

    /**
     * @brief Accesses the top element.
     * @return non-mutable reference to an element.
     *
     * Returns reference to the top element in the stack. This is the most recently pushed element.
     * This element will be removed on a call to pop().
     */
    const_reference top() const;

    // Capacity

    /**
     * @brief Checks whether the container adaptor is empty.
     * @return @c true if the underlying container is empty, @c false otherwise.
     */
    bool empty() const;

    /**
     * @brief Returns the number of elements.
     * @return The number of elements in the container adaptor.
     */
    size_type size() const;

    // Modifiers

    /**
     * @brief Inserts element at the top.
     * @param value  the value of the element to push.
     */
    void push( const value_type& value );

    /**
     * @brief Inserts element at the top.
     * @param value  the value of the element to push.
     */
    void push( value_type&& value );

#if __cplusplus >= 201703L  // (since C++17)
    template<typename... _Args>
    decltype(auto)
    emplace(_Args&&... __args);
#else // C++17
    /**
     * @brief Constructs element in-place at the top.
     * @tparam Args  Types of arguments for an element's constructor.
     * @param args  arguments to forward to the constructor of the element.
     *
     * Pushes a new element on top of the stack.  The element is constructed in-place, i.e. no copy
     * or move operations are performed.  The constructor of the element is called with exactly
     * the same arguments as supplied to the function.
     */
    template< class... Args >
    void emplace( Args&&... args );
#endif

    /**
     * @brief Removes the top element.
     */
    void pop();

    /**
     * @brief Swaps the contents.
     * @param other  container adaptor to exchange the contents with.
     *
     * Exchanges the contents of the container adaptor with those of other.
     */
    void swap( stack& other );

    template< class T, class Container >
    friend bool operator==( const stack<T, Container>& lhs, const stack<T, Container>& rhs );

    template< class T, class Container >
    friend bool operator!=( const stack<T, Container>& lhs, const stack<T, Container>& rhs );

    template< class T, class Container >
    friend bool operator< ( const stack<T, Container>& lhs, const stack<T, Container>& rhs );

    template< class T, class Container >
    friend bool operator<=( const stack<T, Container>& lhs, const stack<T, Container>& rhs );

    template< class T, class Container >
    friend bool operator> ( const stack<T, Container>& lhs, const stack<T, Container>& rhs );

    template< class T, class Container >
    friend bool operator>=( const stack<T, Container>& lhs, const stack<T, Container>& rhs );
};


template< class T, class Container >
bool operator==( const stack<T, Container>& lhs,
                 const stack<T, Container>& rhs );

template< class T, class Container >
bool operator!=( const stack<T, Container>& lhs,
                 const stack<T, Container>& rhs );

template< class T, class Container >
bool operator< ( const stack<T, Container>& lhs,
                 const stack<T, Container>& rhs );

template< class T, class Container >
bool operator<=( const stack<T, Container>& lhs,
                 const stack<T, Container>& rhs );

template< class T, class Container >
bool operator> ( const stack<T, Container>& lhs,
                 const stack<T, Container>& rhs );

template< class T, class Container >
bool operator>=( const stack<T, Container>& lhs,
                 const stack<T, Container>& rhs );
