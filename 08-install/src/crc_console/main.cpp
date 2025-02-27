#include <iostream>
#include <vector>

#include "crc/crc.hpp"

int main(int /*argc*/, char** /*argv*/)
{
    std::vector<uint8_t> buf{};
    std::istream::char_type byte{};

    while( std::cin.get(byte) )
    {
        buf.push_back(byte);
    }

    std::cerr << "buf(" << buf.size() << "): '" << buf.data() << "'" << std::endl;

    std::cout << std::hex << std::showbase << std::nouppercase
        << (unsigned) crc_sum(buf.data(), buf.size())
        << std::endl;

    return 0;
}
