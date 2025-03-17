#include "circular_buffer/circular_buffer.hpp"
#include "circular_buffer_impl.hpp"


circular_buffer_base::circular_buffer_base(
    std::unique_ptr<IStorage>&& storage)
: pImpl_(std::make_unique<Impl>(std::move(storage)))
{
}

circular_buffer_base::~circular_buffer_base()
{
}

circular_buffer_base::pointer  circular_buffer_base::front()
{
    return pImpl_->front();
}

bool  circular_buffer_base::empty() const
{
    return pImpl_->empty();
}

circular_buffer_base::size_type  circular_buffer_base::size() const
{
    return pImpl_->size();
}

void circular_buffer_base::clear(typename IStorage::destroyer_f destroyer)  noexcept
{
    pImpl_->clear(destroyer);
}

void  circular_buffer_base::push_back(typename IStorage::creator_f creator)
{
    pImpl_->push_back(creator);
}

void  circular_buffer_base::pop_front(typename IStorage::destroyer_f destroyer)
{
    pImpl_->pop_front(destroyer);
}

IStorage& circular_buffer_base::get_storage()  const noexcept
{
    return *pImpl_->buf_;
}
