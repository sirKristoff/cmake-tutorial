#pragma once

#include "circular_buffer/circular_buffer.hpp"


struct circular_buffer_base::Impl
{
    std::unique_ptr<IStorage> buf_;
    pointer head_;
    pointer tail_;


    Impl(std::unique_ptr<IStorage>&& storage);

    pointer front();

    bool empty()  const;
    size_type size()  const;

    void clear(typename IStorage::destroyer_f destroyer)  noexcept;
    void push_back(typename IStorage::creator_f creator);
    void pop_front(typename IStorage::destroyer_f destroyer);

    // buffered elements are wrapped
    bool is_wrapped()  const;
};
