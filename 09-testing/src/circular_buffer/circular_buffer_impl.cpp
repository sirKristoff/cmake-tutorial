#include "circular_buffer_impl.hpp"

#include <cassert>  // for assert
#include <type_traits>  // for make_signed


circular_buffer_base::Impl::Impl(
    std::unique_ptr<IStorage>&& storage)
: buf_(std::move(storage)),
  head_(buf_->begin()),
  tail_(head_)
{
}

bool
circular_buffer_base::Impl::empty() const
{
    return head_ == tail_;
}

circular_buffer_base::pointer
circular_buffer_base::Impl::front()
{
    if (empty())
        return nullptr;
    return head_;
}

circular_buffer_base::size_type
circular_buffer_base::Impl::size() const
{
    const typename IStorage::difference_type diff =
        buf_->distance(head_, tail_);

    if ( diff < 0 )
    {  // buffered elements are wrapped
        return  buf_->capacity() + diff;
    }

    return diff;
}

void
circular_buffer_base::Impl::clear(typename IStorage::destroyer_f destroyer)  noexcept
{
    if (empty())
        return;

    if (is_wrapped())
    {
        auto begin_ = buf_->begin();
        while (head_ != begin_)
        {
            destroyer(head_);
            buf_->advance(head_);
        }
    }
    while (head_ != tail_)
    {
        destroyer(head_);
        buf_->advance(head_);
    }

    std::tie(head_, tail_) =
        buf_->shrink_to_fit(
            {head_, tail_},
            IStorage::const_element_range{});
}

void
circular_buffer_base::Impl::push_back(typename IStorage::creator_f creator)
{
    auto origin_capacity = buf_->capacity();
    if (size() == origin_capacity -1)
    { // not enough space for new elements in future
        if (is_wrapped())
            std::tie(head_, tail_) =
                buf_->reserve(
                    2 * origin_capacity,
                    {head_, buf_->end()},
                    {buf_->begin(), tail_}
                );
        else
            std::tie(head_, tail_) =
                buf_->reserve(
                    2 * origin_capacity,
                    {head_, tail_},
                    IStorage::const_element_range{}
                );
        if (size() == buf_->capacity())
            return;  // TODO: create some error mechanism
    }

    creator(tail_);
    buf_->advance(tail_);
}

void
circular_buffer_base::Impl::pop_front(typename IStorage::destroyer_f destroyer)
{
    // assert(!empty() && "Try to pop element from empty buffer!");
    if (empty())
        return;

    auto front_elem = head_;
    buf_->advance(head_);
    destroyer(front_elem);

    if (is_wrapped())
        std::tie(head_, tail_) =
            buf_->shrink_to_fit(
                {head_, buf_->end()},
                {buf_->begin(), tail_});
    else
        std::tie(head_, tail_) =
            buf_->shrink_to_fit(
                {head_, tail_},
                IStorage::const_element_range{});
}


bool
circular_buffer_base::Impl::is_wrapped()  const
{
    // return buf_->distance(head_, tail_) < 0;
    return tail_ < head_;
}
