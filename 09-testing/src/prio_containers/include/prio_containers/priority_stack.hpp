#pragma once

#include <algorithm>  // for copy_backward stable_sort upper_bound
#include <functional>  // for less
#include <iterator>  // for next
#include <vector>  // for vector
#include <type_traits>  // for enable_if is_move_assignable_v is_move_constructible is_same
#include <utility> // for forward, move, swap


#if defined(__has_cpp_attribute) && __has_cpp_attribute(nodiscard)  // 201603L
# define nodiscard_atr [[nodiscard]]
#else
# define nodiscard_atr
#endif

#ifndef CXX20_constexpr
# if __cplusplus >= 202002L
#  define CXX20_constexpr constexpr
# else
#  define CXX20_constexpr
# endif
#endif


/**
 * @def PRIORITY_STACK_CONSTRUCT_AND_SORT_INPLACE
 * @brief Enables option for making container copy and in-place sorting after.
 */

// #define PRIORITY_STACK_CONSTRUCT_AND_SORT_INPLACE


/**
 * TODO:
 */
template
<
    class T,
    class Container = std::vector<T>,
    class Compare = std::less<typename Container::value_type>
>
class priority_stack
{

#if __cplusplus > 201703L
    static_assert(
        std::is_invocable_r_v<bool, Compare,
            typename Container::value_type,
            typename Container::value_type>,
	    "Compare must be a functor for comparing value_type");
#endif // C++17

// #if __cplusplus >= 201703L
    // Requirements on the first template parameter of container adaptors
    static_assert(std::is_same<T, typename Container::value_type>::value,
	    "value_type must be the same as the underlying container");
// #endif // C++17

protected:
    Container c_;
    Compare comp_;

public:
    typedef          Container                  container_type;
    typedef typename Container::allocator_type  allocator_type;
    typedef typename Container::value_type      value_type;
    typedef          Compare                    value_compare;
    typedef typename Container::size_type       size_type;
    typedef typename Container::reference       reference;
    typedef typename Container::const_reference const_reference;

    typedef typename  Container::difference_type        difference_type;
    typedef typename  Container::pointer                pointer;
    typedef typename  Container::const_pointer          const_pointer;
    typedef typename  Container::reverse_iterator       iterator;
    typedef typename  Container::const_reverse_iterator const_iterator;
    typedef typename  Container::iterator               reverse_iterator;
    typedef typename  Container::const_iterator         const_reverse_iterator;


    ////////////////////////////////////////////////////////////////////////////
    // Construct empty stack
    ////////////////////////////////////////////////////////////////////////////

    /**
     * @brief Default constructor.
     *
     * Value-initializes the comparator and the underlying container.
     */
    priority_stack()  noexcept
    : c_(), comp_()
    {}

    /**
     * @brief Copy-constructs the comparison functor comp_ with the contents of compare.
     * @param compare  comparison functor.
     *
     * Value-initializes the underlying container c_.
     */
    explicit
    priority_stack( const Compare& compare )  noexcept
    : c_(), comp_(compare)
    {}

    /**
     * @brief Constructs the underlying container using alloc as allocator.
     * @tparam Alloc  allocator type for container.
     * @param alloc  allocator.
     * @remark This overload participate in overload resolution only if
     *   @c std::uses_allocator<container_type,Alloc>::value is @c true, that is, if the underlying
     *   container is an allocator-aware container (true for all standard library containers).
     */
    template< class Alloc >
    explicit
    priority_stack( const Alloc& alloc )  noexcept
    : c_(alloc), comp_()
    {}

    /**
     * @brief Constructs the underlying container using alloc as allocator.
     * @tparam Alloc  allocator type for container.
     * @param compare  comparison functor.
     * @param alloc  allocator.
     * @remark This overload participate in overload resolution only if
     *   @c std::uses_allocator<container_type,Alloc>::value is @c true, that is, if the underlying
     *   container is an allocator-aware container (true for all standard library containers).
     */
    template< class Alloc >
    priority_stack( const Compare& compare, const Alloc& alloc )  noexcept
    : c_(alloc), comp_(compare)
    {}


    ////////////////////////////////////////////////////////////////////////////
    // Copy elements from **container** with unknown order
    ////////////////////////////////////////////////////////////////////////////

    /**
     * @brief Copy-constructs the underlying container c_ with the contents of cont.
     * @param cont  container for copy.
     */
    explicit
    priority_stack(
        const Container& cont )
    : priority_stack(Compare{}, cont)
    {}

    /**
     * @brief Copy-constructs the underlying container.
     * @param compare  comparison functor.
     * @param cont  container for copy.
     * @note Elements in the source container do not need to be in order according to the compare.
     * @note Stack makes priority order during construction.
     *
     * Copy-constructs the underlying container c_ with the contents of cont.
     * Copy-constructs the comparison functor comp_ with the contents of compare.
     */
    priority_stack(
        const Compare& compare,
        const Container& cont )
#ifdef PRIORITY_STACK_CONSTRUCT_AND_SORT_INPLACE
    : c_(cont),
      comp_(compare)
    {
        std::stable_sort(c_.begin(), c_.end(), comp_);
    }
#else  //\ PRIORITY_STACK_CONSTRUCT_AND_SORT_INPLACE
    : priority_stack(compare, cont, cont.get_allocator())
    {}
#endif  // ! PRIORITY_STACK_CONSTRUCT_AND_SORT_INPLACE

    /**
     * @brief Constructs the underlying container with the contents of cont and
     *   using alloc as allocator.
     * @tparam Alloc  allocator type for container.
     * @param cont  container for copy.
     * @param alloc  allocator.
     * @remark This overload participate in overload resolution only if
     *   @c std::uses_allocator<container_type,Alloc>::value is @c true, that is, if the underlying
     *   container is an allocator-aware container (true for all standard library containers).
     * @note Elements in the source container do not need to be in order according to the compare.
     * @note Stack makes priority order during construction.
     */
    template< class Alloc >
    priority_stack(
        const Container& cont,
        const Alloc& alloc )
    : priority_stack( Compare{}, cont, alloc )
    {}

    /**
     * @brief Constructs the underlying container with the contents of cont and
     *   using alloc as allocator.
     * @tparam Alloc  allocator type for container.
     * @param compare  comparison functor.
     * @param cont  container for copy.
     * @param alloc  allocator.
     * @note Elements in the source container do not need to be in order according to the compare.
     * @note Stack makes priority order during construction.
     *
     * Copy-constructs comp_ from compare.
     */
    template< class Alloc >
    priority_stack(
        const Compare& compare,
        const Container& cont,
        const Alloc& alloc )
#ifdef PRIORITY_STACK_CONSTRUCT_AND_SORT_INPLACE
    : c_(cont, alloc),
      comp_(compare)
    {
        std::stable_sort(c_.begin(), c_.end(), comp_);
    }
#else  //\ PRIORITY_STACK_CONSTRUCT_AND_SORT_INPLACE
    : c_(alloc),
      comp_(compare)
    {
        push(cont.begin(), cont.end());
    }
#endif  // ! PRIORITY_STACK_CONSTRUCT_AND_SORT_INPLACE

    ////////////////////////////////////////////////////////////////////////////
    // Move elements from **container** with unknown order
    ////////////////////////////////////////////////////////////////////////////

    /**
     * @brief Move-constructs the underlying container c_ with @c std::move(cont)
     * @param cont  container for copy.
     * @note Elements in the source container do not need to be in order according to the Compare.
     * @note Stack makes priority order during construction.
     */
    explicit
    priority_stack(
        Container&& cont )  noexcept
    : priority_stack(Compare{}, std::move(cont))
    {}

    /**
     * @brief Move-constructs the underlying container.
     * @param compare  comparison functor.
     * @param cont  container for copy.
     * @note Elements in the source container do not need to be in order according to the compare.
     * @note Stack makes priority order during construction.
     *
     * Move-constructs the underlying container c_ with @c std::move(cont).
     * Copy-constructs the comparison functor comp_ with compare.
     */
    priority_stack(
        const Compare& compare,
        Container&& cont )  noexcept
#ifdef PRIORITY_STACK_CONSTRUCT_AND_SORT_INPLACE
    : c_(std::move(cont)),
      comp_(compare)
    {
        std::stable_sort(c_.begin(), c_.end(), comp_);
    }
#else  //\ PRIORITY_STACK_CONSTRUCT_AND_SORT_INPLACE
    : priority_stack(
        compare,
        std::move(cont),
        typename Container::allocator_type{cont.get_allocator()})
    {}
#endif  // ! PRIORITY_STACK_CONSTRUCT_AND_SORT_INPLACE

    /**
     * @brief Constructs the underlying container with the contents of cont
     *   using move semantics while utilizing alloc as allocator.
     * @tparam Alloc  allocator type for container.
     * @param cont  container for copy.
     * @param alloc  allocator.
     * @remark This overload participate in overload resolution only if
     *   @c std::uses_allocator<container_type,Alloc>::value is @c true, that is, if the underlying
     *   container is an allocator-aware container (true for all standard library containers).
     * @note Elements in the source container do not need to be in order according to the Compare.
     * @note Stack makes priority order during construction.
     */
    template< class Alloc >
    priority_stack(
        Container&& cont,
        const Alloc& alloc )  noexcept
    : priority_stack( Compare{}, std::move(cont), alloc )
    {}

    /**
     * @brief Constructs the underlying container with the contents of cont
     *   using move semantics while using alloc as allocator.
     * @tparam Alloc  allocator type for container.
     * @param compare  comparison functor.
     * @param cont  container for copy.
     * @param alloc  allocator.
     * @note Elements in the source container do not need to be in order according to the compare.
     * @note Stack makes priority order during construction.
     *
     * Copy-constructs comp_ from compare.
     */
    template< class Alloc >
    priority_stack(
        const Compare& compare,
        Container&& cont,
        const Alloc& alloc )  noexcept
#ifdef PRIORITY_STACK_CONSTRUCT_AND_SORT_INPLACE
    : c_(std::move(cont), alloc),
      comp_(compare)
    {
        std::stable_sort(c_.begin(), c_.end(), comp_);
    }
#else  //\ PRIORITY_STACK_CONSTRUCT_AND_SORT_INPLACE
    : c_(alloc),
      comp_(compare)
    {
        c_.reserve(cont.size());
        for (auto&& element : cont)
        {
            push(std::move(element));
        }

        // Remove moved elements
        // Container{std::move(cont)};
        cont.clear();
    }
#endif  // ! PRIORITY_STACK_CONSTRUCT_AND_SORT_INPLACE

    ////////////////////////////////////////////////////////////////////////////
    // Copy elements from **iterator** range with unknown order
    ////////////////////////////////////////////////////////////////////////////

    /**
     * @brief Constructs c_ as if by c_(first, last) and comp_ from compare.
     * @tparam InputIt  satisfies @a LegacyInputIterator.
     * @param first  begin of iterator range.
     * @param last  end of iterator range.
     * @param compare  comparison functor.
     * @note Elements in @c (first,last) iterator range do not need to be in order according to
     *   the compare.
     * @note Stack makes priority order during construction.
     */
    template< class InputIt >
    priority_stack(
        InputIt first, InputIt last,
        const Compare& compare = Compare{} )
#ifdef PRIORITY_STACK_CONSTRUCT_AND_SORT_INPLACE
    : c_(first, last),
      comp_(compare)
    {
        std::stable_sort(c_.begin(), c_.end(), comp_);
    }
#else  //\ PRIORITY_STACK_CONSTRUCT_AND_SORT_INPLACE
    : priority_stack(compare)
    {
        push(first, last);
    }
#endif  // ! PRIORITY_STACK_CONSTRUCT_AND_SORT_INPLACE

    /**
     * @brief Constructs c_ as if by c_(first, last) and
     *   alloc is used for constructing the underlying container.
     * @tparam InputIt  satisfies @a LegacyInputIterator.
     * @tparam Alloc  allocator type for container.
     * @param first  begin of iterator range.
     * @param last  end of iterator range.
     * @param alloc  allocator.
     * @remark This overload participate in overload resolution only if
     *   @c std::uses_allocator<container_type,Alloc>::value is @c true, that is, if the underlying
     *   container is an allocator-aware container (true for all standard library containers).
     * @note Elements in @c (first,last) iterator range do not need to be in order according to
     *   the compare.
     * @note Stack makes priority order during construction.
     */
    template< class InputIt, class Alloc >
    priority_stack(
        InputIt first, InputIt last,
        const Alloc& alloc )
    : priority_stack(first, last, Compare{}, alloc)
    {}

    /**
     * @brief Constructs c_ as if by c_(first, last) and
     *   alloc is used for constructing the underlying container.
     * @tparam InputIt  satisfies @a LegacyInputIterator.
     * @tparam Alloc  allocator type for container.
     * @param first  begin of iterator range.
     * @param last  end of iterator range.
     * @param compare  comparison functor.
     * @param alloc  allocator.
     * @remark This overload participate in overload resolution only if
     *   @c std::uses_allocator<container_type,Alloc>::value is @c true, that is, if the underlying
     *   container is an allocator-aware container (true for all standard library containers).
     * @note Elements in @c (first,last) iterator range do not need to be in order according to
     *   the compare.
     * @note Stack makes priority order during construction.
     */
    template< class InputIt, class Alloc >
    priority_stack(
        InputIt first, InputIt last,
        const Compare& compare,
        const Alloc& alloc )
    : c_(alloc),
      comp_(compare)
    {
        push(first, last);
    }

    ////////////////////////////////////////////////////////////////////////////
    // Merge elements from **container** and **iterator** range with unknown order
    ////////////////////////////////////////////////////////////////////////////

    /**
     * @brief Copy-constructs c_ from cont and comp_ from compare,
     *   next push elements from iterator range.
     * @tparam InputIt  satisfies @a LegacyInputIterator.
     * @param first  begin of iterator range.
     * @param last  end of iterator range.
     * @param compare  comparison functor.
     * @param cont  container for copy.
     * @note Elements in the source container neither in @c (first,last) iterator range do not need
     *   to be in order according to the compare.
     * @note Stack makes priority order during construction.
     */
    template< class InputIt >
    priority_stack(
        InputIt first, InputIt last,
        const Compare& compare,
        const Container& cont )
    : priority_stack(
        cont.begin(), cont,end(), compare,
        typename Container::allocator_type{cont.get_allocator()})
    {
        push(first, last);
    }

    /**
     * @brief Move-constructs c_ from cont and copy-constructs comp_ from compare,
     *   next push elements from iterator range.
     * @tparam InputIt  satisfies @a LegacyInputIterator.
     * @param first  begin of iterator range.
     * @param last  end of iterator range.
     * @param compare  comparison functor.
     * @param cont  container for copy.
     * @note Elements in the source container neither in @c (first,last) iterator range do not need
     *   to be in order according to the compare.
     * @note Stack makes priority order during construction.
     */
    template< class InputIt >
    priority_stack(
        InputIt first, InputIt last,
        const Compare& compare,
        Container&& cont )
    : priority_stack(compare, std::move(cont))
    {
        push(first, last);
    }

    /**
     * @brief Copy-constructs c_ from cont and comp_ from compare,
     *   next push elements from iterator range.
     * @tparam InputIt  satisfies @a LegacyInputIterator.
     * @tparam Alloc  allocator type for container.
     * @param first  begin of iterator range.
     * @param last  end of iterator range.
     * @param compare  comparison functor.
     * @param cont  container for copy.
     * @param alloc  allocator.
     * @note Elements in the source container neither in @c (first,last) iterator range do not need
     *   to be in order according to the compare.
     * @note Stack makes priority order during construction.
     */
    template< class InputIt, class Alloc >
    priority_stack(
        InputIt first, InputIt last,
        const Compare& compare,
        const Container& cont,
        const Alloc& alloc )
    : priority_stack(
        cont.begin(), cont,end(), compare, alloc)
    {
        push(first, last);
    }

    /**
     * @brief Move-constructs c_ from cont and copy-constructs comp_ from compare,
     *   next push elements from iterator range.
     * @tparam InputIt  satisfies @a LegacyInputIterator.
     * @tparam Alloc  allocator type for container.
     * @param first  begin of iterator range.
     * @param last  end of iterator range.
     * @param compare  comparison functor.
     * @param cont  container for copy.
     * @param alloc  allocator.
     * @note Elements in the source container neither in @c (first,last) iterator range do not need
     *   to be in order according to the compare.
     * @note Stack makes priority order during construction.
     */
    template< class InputIt, class Alloc >
    priority_stack(
        InputIt first, InputIt last,
        const Compare& compare,
        Container&& cont,
        const Alloc& alloc )
    : priority_stack(compare, std::move(cont), alloc)
    {
        push(first, last);
    }


    ////////////////////////////////////////////////////////////////////////////
    // Copy/move elements from compatible stack
    ////////////////////////////////////////////////////////////////////////////

private:

    template < class Container_ >
    constexpr inline static
    typename std::enable_if<     std::is_move_constructible<Container_>::value >::type
    post_move_construct_c(Container_&&)
    {}

    template < class Container_ >
    constexpr inline static
    typename std::enable_if< not std::is_move_constructible<Container_>::value >::type
    post_move_construct_c(Container_&& cont)
    {  cont = Container_{};  }

public:

    /**
     * @brief Copy constructor.
     * @param src  source object.
     *
     * The underlying container is copy-constructed with @c src.c_.
     * The comparison functor is copy-constructed with @c src.comp_.
     */
    priority_stack( const priority_stack& src )
    : c_(src.c_),
      comp_(src.comp_)
    {}

    /**
     * @brief Move constructor.
     * @param src  source object.
     *
     * The underlying container is constructed with @c std::move(src.c_).
     * The comparison functor is constructed with @c std::move(src.comp_).
     */
    priority_stack( priority_stack&& src )  noexcept
    : c_(std::move(src.c_)),
      comp_(src.comp_)
    {
        post_move_construct_c(std::move(src.c_));
    }

    /**
     * @brief Constructs the underlying container with the contents of @c src.c_ and using alloc as allocator.
     * @tparam Alloc  allocator type for container.
     * @param src  source stack.
     * @param alloc  allocator.
     * @remark This overload participate in overload resolution only if
     *   @c std::uses_allocator<container_type,Alloc>::value is @c true, that is, if the underlying
     *   container is an allocator-aware container (true for all standard library containers).
     *
     * Copy-constructs comp from @c src.comp_.
     */
    template< class Alloc >
    priority_stack( const priority_stack& src, const Alloc& alloc )
    : c_(src.c_, alloc),
      comp_(src.comp_)
    {}

    /**
     * @brief Constructs the underlying container with the contents of src.
     * @tparam Alloc  allocator type for container.
     * @param src  source stack.
     * @param alloc  allocator.
     * @remark This overload participate in overload resolution only if
     *   @c std::uses_allocator<container_type,Alloc>::value is @c true, that is, if the underlying
     *   container is an allocator-aware container (true for all standard library containers).
     *
     * Using move semantics while utilizing alloc as allocator.
     * Move-constructs comp from @c src.comp_.
     */
    template< class Alloc >
    priority_stack( priority_stack&& src, const Alloc& alloc )  noexcept
    : c_(std::move(src.c_), alloc),
      comp_(src.comp_)
    {
        post_move_construct_c(std::move(src.c_));
    }

    ////////////////////////////////////////////////////////////////////////////
    // Destructor
    ////////////////////////////////////////////////////////////////////////////

    /**
     * @brief Destructs the stack.
     *
     * The destructors of the elements are called and the used storage is deallocated.
     * @note If the elements are pointers, the pointed-to objects are not destroyed.
     */
    CXX20_constexpr
    ~priority_stack()  noexcept
    {}

    ////////////////////////////////////////////////////////////////////////////
    // Assign operator
    ////////////////////////////////////////////////////////////////////////////

    CXX20_constexpr
    priority_stack& operator=( const priority_stack& rhs )  // (implicitly declared)
    {
        if (this != &rhs)
        {
            c_ = rhs.c_;
            comp_ = rhs.comp_;
        }
        return *this;
    }

private:

    template < class Container_ >
    constexpr inline
    typename std::enable_if<     std::is_move_assignable<Container_>::value >::type
    move_assign_c(Container_&& cont)
    {  c_ = std::move(cont);  }

    template < class Container_ >
    constexpr inline
    typename std::enable_if< not std::is_move_assignable<Container_>::value >::type
    move_assign_c(Container_&& cont)
    {  c_ = cont;  cont = Container_{};  }

public:

    CXX20_constexpr
    priority_stack& operator=( priority_stack&& rhs )  // (implicitly declared)
    {
        if (this != &rhs)
        {
            move_assign_c(std::move(rhs.c_));
            comp_ = rhs.comp_;
        }
        return *this;
    }

    ////////////////////////////////////////////////////////////////////////////
    // swap
    ////////////////////////////////////////////////////////////////////////////

    /**
     * @brief Swaps the contents.
     * @param other  container adaptor to exchange the contents with.
     *
     * Exchanges the contents of the container adaptor with those of other.
     */
    void swap( priority_stack& other )
    {
        std::swap(c_, other.c_);
        std::swap(comp_, other.comp_);
    }

    ////////////////////////////////////////////////////////////////////////////
    // Element access
    ////////////////////////////////////////////////////////////////////////////

    nodiscard_atr  CXX20_constexpr  iterator
      begin()       noexcept  {  return   c_.rbegin();  }
    nodiscard_atr  CXX20_constexpr  const_iterator
      begin() const noexcept  {  return   c_.rbegin();  }
    nodiscard_atr  CXX20_constexpr  const_iterator
     cbegin() const noexcept  {  return  c_.crbegin();  }
    nodiscard_atr  CXX20_constexpr  iterator
        end()       noexcept  {  return     c_.rend();  }
    nodiscard_atr  CXX20_constexpr  const_iterator
        end() const noexcept  {  return     c_.rend();  }
    nodiscard_atr  CXX20_constexpr  const_iterator
       cend() const noexcept  {  return    c_.crend();  }
    nodiscard_atr  CXX20_constexpr  reverse_iterator
     rbegin()       noexcept  {  return    c_.begin();  }
    nodiscard_atr  CXX20_constexpr  const_reverse_iterator
     rbegin() const noexcept  {  return    c_.begin();  }
    nodiscard_atr  CXX20_constexpr  const_reverse_iterator
    crbegin() const noexcept  {  return   c_.cbegin();  }
    nodiscard_atr  CXX20_constexpr  reverse_iterator
       rend()       noexcept  {  return      c_.end();  }
    nodiscard_atr  CXX20_constexpr  const_reverse_iterator
       rend() const noexcept  {  return      c_.end();  }
    nodiscard_atr  CXX20_constexpr  const_reverse_iterator
      crend() const noexcept  {  return     c_.cend();  }

    /**
     * @brief Accesses the top element.
     * @return reference to an element.
     *
     * Returns reference to the top element in the stack. This is the most recently pushed element.
     * This element will be removed on a call to pop().
     */
    nodiscard_atr  CXX20_constexpr
    reference top()  noexcept
    {  return c_.back();  }

    /**
     * @brief Accesses the top element.
     * @return non-mutable reference to an element.
     *
     * Returns reference to the top element in the stack. This is the most recently pushed element.
     * This element will be removed on a call to pop().
     */
    nodiscard_atr  CXX20_constexpr
    const_reference top() const  noexcept
    {  return c_.back();  }

    ////////////////////////////////////////////////////////////////////////////
    // Capacity
    ////////////////////////////////////////////////////////////////////////////

    /**
     * @brief Checks whether the container adaptor is empty.
     * @return @c true if the underlying container is empty, @c false otherwise.
     */
    nodiscard_atr  CXX20_constexpr
    bool empty() const  noexcept
    {  return c_.empty();  }

    /**
     * @brief Returns the number of elements.
     * @return The number of elements in the container adaptor.
     */
    nodiscard_atr  CXX20_constexpr
    size_type size() const  noexcept
    {  return c_.size();  }

    ////////////////////////////////////////////////////////////////////////////
    // Modifiers
    ////////////////////////////////////////////////////////////////////////////

    /**
     * @brief Inserts element at the top according to priority.
     * @param value  the value of the element to push.
     */
    void push( const value_type& value )
    {
        /** @brief Position in the container behind last element
         *  with priority equal the value's priority  */
        auto lastElemWithPrioItr = std::upper_bound(
            c_.begin(), c_.end(), value, comp_);
        // new element into inner position
        c_.insert(lastElemWithPrioItr, value);
    }

    /**
     * @brief Inserts element at the top.
     * @param value  the value of the element to push.
     */
    void push( value_type&& value )
    {
        /** @brief Position in the container behind last element
         *  with priority equal the value's priority  */
        auto lastElemWithPrioItr = std::upper_bound(
            c_.begin(), c_.end(), value, comp_);
        // new element into inner position
        c_.insert(lastElemWithPrioItr, std::move(value));
    }

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
    void emplace( Args&&... args )
    {
        c_.emplace_back(std::forward<Args>(args)...);
        std::stable_sort(c_.begin(), c_.end(), comp_);
    }

    /**
     * @brief Removes the top element.
     */
    CXX20_constexpr
    void pop()
    {  c_.pop_back();  }

protected:

    /**
     * @brief Inserts elements from iterator range at the top according to priority.
     * @tparam InputIt  satisfies @a LegacyInputIterator.
     * @param first  begin of iterator range.
     * @param last  end of iterator range.
     */
    template< class InputIt >
    void push( InputIt first, InputIt last )
    {
        c_.reserve(c_.size() + std::distance(first, last));
        std::for_each(first, last, [this](const value_type& elem){ this->push(elem); });
    }

    template< class T_, class C_, class Comp_ >
    friend bool operator==( const priority_stack<T_, C_, Comp_>& lhs,
                            const priority_stack<T_, C_, Comp_>& rhs );
    template< class T_, class C_, class Comp_ >
    friend bool operator!=( const priority_stack<T_, C_, Comp_>& lhs,
                            const priority_stack<T_, C_, Comp_>& rhs );
    template< class T_, class C_, class Comp_ >
    friend bool operator< ( const priority_stack<T_, C_, Comp_>& lhs,
                            const priority_stack<T_, C_, Comp_>& rhs );
    template< class T_, class C_, class Comp_ >
    friend bool operator<=( const priority_stack<T_, C_, Comp_>& lhs,
                            const priority_stack<T_, C_, Comp_>& rhs );
    template< class T_, class C_, class Comp_ >
    friend bool operator> ( const priority_stack<T_, C_, Comp_>& lhs,
                            const priority_stack<T_, C_, Comp_>& rhs );
    template< class T_, class C_, class Comp_ >
    friend bool operator>=( const priority_stack<T_, C_, Comp_>& lhs,
                            const priority_stack<T_, C_, Comp_>& rhs );

};  // class priority_stack


template< class T, class C, class Comp >
inline void swap( priority_stack<T, C>& lhs, priority_stack<T, C>& rhs )
{
    lhs.swap(rhs);
}


template< class T, class C, class Comp >
nodiscard_atr  inline  bool
operator==( const priority_stack<T, C, Comp>& lhs, const priority_stack<T, C, Comp>& rhs )
{  return lhs.c_ == rhs.c_;  }

template< class T, class C, class Comp >
nodiscard_atr  inline  bool
operator!=( const priority_stack<T, C, Comp>& lhs, const priority_stack<T, C, Comp>& rhs )
{  return !(lhs == rhs);  }

template< class T, class C, class Comp >
nodiscard_atr  inline  bool
operator< ( const priority_stack<T, C, Comp>& lhs, const priority_stack<T, C, Comp>& rhs )
{  return lhs.c_ < rhs.c_;  }

template< class T, class C, class Comp >
nodiscard_atr  inline  bool
operator<=( const priority_stack<T, C, Comp>& lhs, const priority_stack<T, C, Comp>& rhs )
{  return !(rhs < lhs);  }

template< class T, class C, class Comp >
nodiscard_atr  inline  bool
operator> ( const priority_stack<T, C, Comp>& lhs, const priority_stack<T, C, Comp>& rhs )
{  return rhs < lhs;  }

template< class T, class C, class Comp >
nodiscard_atr  inline  bool
operator>=( const priority_stack<T, C, Comp>& lhs, const priority_stack<T, C, Comp>& rhs )
{  return !(lhs < rhs);  }


// template< class T, class Container, class Alloc>
// struct std::uses_allocator<priority_stack<T, Container>, Alloc>
// : public std::uses_allocator<Container, Alloc>::type
// {};
