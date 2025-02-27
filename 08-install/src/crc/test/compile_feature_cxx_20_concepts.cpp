#include <concepts>
#include <cstdint>
#include <iterator>
#include <type_traits>


template<typename T>
concept NumericType =
    (std::unsigned_integral<T> || std::is_enum_v<T>) &&
    std::convertible_to<T, uint8_t> &&
    std::convertible_to<T, uint16_t> &&
    std::convertible_to<T, uint32_t> &&
requires(T val)
{
    {val ^ (uint8_t)0xFFU};
    {val ^ (uint16_t)0xFFU};
    {val ^ (uint32_t)0xFFU};
};

template< class T >  requires NumericType<T>
struct C
{
    template< class DataForwardIterator >
    requires std::forward_iterator<DataForwardIterator>
    constexpr T
    zero(DataForwardIterator it)
    {
        (void) it;
        return T{};
    }
};

int main()
{
    C<unsigned> obj;
    return obj.zero(&obj);
}
