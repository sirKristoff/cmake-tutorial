#include "crc/crc.hpp"

#include "crc_template.hpp"


typedef typename crc::algorithm< crc::params::crc8_maxim > CrcAlgo;

uint8_t crc_sum(uint8_t* buf, std::size_t len)
{
    return CrcAlgo::sum(buf, len);
}
