#include <array>
#include <cstdint>
#include <cuchar>
#include <utility>

namespace crc {

template< uint8_t nbits > struct bits;
template<> struct bits< 8>{ typedef uint8_t value_t; };
template<> struct bits<16>{ typedef uint16_t value_t; };
template<> struct bits<32>{ typedef uint32_t value_t; };

template< uint8_t nbits >
constexpr uint8_t reflect_bits( uint8_t byte )
    requires(8==nbits)
{
    return (byte * 0x0202020202ULL & 0x010884422010ULL) % 0x3ff;
}

template< uint8_t nbits >
constexpr uint16_t reflect_bits( uint16_t halfword )
    requires(16==nbits)
{
    return static_cast<uint16_t>(reflect_bits<8>(halfword)) << 8
        | reflect_bits<8>(halfword >> 8);
}

template< uint8_t nbits >
constexpr uint32_t reflect_32bits( uint32_t word )
    requires(16==nbits)
{
    return static_cast<uint32_t>(reflect_bits<16>(word)) << 16
        | reflect_bits<16>(word >> 16);
}


namespace params {

/**
 * Parametrised CRC algorithms
 * copied from site:
 * https://reveng.sourceforge.io/crc-catalogue/1-15.htm
 *
 *    AlgoName          Width   Polynomial  Reversed  InitialValue  FinalXorValue   ReflectIn   ReflectOut  Uses
 */
#define CRC_PARAMS    \
    X(crc8_8h2f,        8,      0x2F,       0xF4,     0xFF,         0xFF,           false,      false,      "Autosar")                                                   \
    X(crc8_bluetooth,   8,      0xA7,       0xE5,     0x00,         0x00,           true,       true,       "Bluetooth header error correction")                         \
    X(crc8_cdma2000,    8,      0x9B,       0xD9,     0xFF,         0x00,           false,      false,      "mobile networks")                                           \
    X(crc8_darc,        8,      0x39,       0x9C,     0x00,         0x00,           true,       true,       "Data Radio Channel")                                        \
    X(crc8_dvb_s2,      8,      0xD5,       0xAB,     0x00,         0x00,           false,      false,      "DVB-S2")                                                    \
    X(crc8_ebu,         8,      0x1D,       0xB8,     0xFF,         0x00,           true,       true,       "AES/EBU digital audio interface")                           \
    X(crc8_gsm_a,       8,      0x1D,       0xB8,     0x00,         0x00,           false,      false,      "mobile networks")                                           \
    X(crc8_gsm_b,       8,      0x49,       0x92,     0x00,         0xFF,           false,      false,      "mobile networks")                                           \
    X(crc8_hitag,       8,      0x1D,       0xB8,     0xFF,         0x00,           false,      false,      "RFID applications")                                         \
    X(crc8_icode,       8,      0x1D,       0xB8,     0xFD,         0x00,           false,      false,      "I-CODE RFID labels")                                        \
    X(crc8_itu,         8,      0x07,       0xE0,     0x00,         0x55,           false,      false,      "Asynchronous Transfer Mode Header Error Control sequence")  \
    X(crc8_lte,         8,      0x9B,       0xD9,     0x00,         0x00,           false,      false,      "mobile networks")                                           \
    X(crc8_maxim,       8,      0x31,       0x8C,     0x00,         0x00,           true,       true,       "1-Wire bus")                                                \
    X(crc8_mifare,      8,      0x1D,       0xB8,     0xC7,         0x00,           false,      false,      "NFC Mifare cards")                                          \
    X(crc8_nrsc_5,      8,      0x31,       0x8C,     0xFF,         0x00,           false,      false,      "Audio Transport in NRSC-5 digital radio")                   \
    X(crc8_opensafety,  8,      0x2F,       0xF4,     0x00,         0x00,           false,      false,      "openSAFETY")                                                \
    X(crc8_rohc,        8,      0x07,       0xE0,     0xFF,         0x00,           true,       true,       "Robust Header Compression for RTP/UDP/IP")                  \
    X(crc8_sae_j1850,   8,      0x1D,       0xB8,     0xFF,         0xFF,           false,      false,      "AES3; OBD")                                                 \
    X(crc8_sae_j1850_0, 8,      0x1D,       0xB8,     0x00,         0x00,           false,      false,      "AES3; OBD")                                                 \
    X(crc8_smbus,       8,      0x07,       0xE0,     0x00,         0x00,           false,      false,      "System Management Bus, ATM HEC, ISDN HEC")                  \
    X(crc8_wcdma,       8,      0x9B,       0xD9,     0x00,         0x00,           true,       true,       "mobile networks")


#define X(algoName_, width_, polynomial_, reversed_, initialValue_, finalXorValue_, reflectIn_, reflectOut_, uses_)  \
  \
struct algoName_  \
{  \
    typedef bits<width_>::value_t value_t;  \
    enum {  \
        width = width_,  \
        poly = polynomial_,  \
        rev_poly = reversed_,  \
        init = initialValue_,  \
        xorout = finalXorValue_,  \
        refin = reflectIn_,  \
        refout = reflectOut_,  \
    };  \
};  \

CRC_PARAMS

} // namespace params
#undef X


template< class Params >
struct algorithm : public Params
{
    static constexpr const std::array<typename Params::value_t, (0b1U<<Params::width)>  // TODO: change 'Params::width' to 8
        lookup_table = algorithm::make_lookup_table();

    template< class DataPtr >
    static constexpr typename Params::value_t
    sum(
        DataPtr buf,
        std::size_t len,
        typename Params::value_t crc = typename Params::value_t{} )
    {
        return (Params::xorout ^ sum_without_final_xor(buf, len, crc)); 
    }

    template< class DataPtr >
    static constexpr typename Params::value_t
    sum_without_final_xor(
        DataPtr buf,
        std::size_t len,
        typename Params::value_t crc = typename Params::value_t{} )
    {  // TODO: Create 16 and 32 bit variant based on http://www.sunshine2k.de/articles/coding/crc/understanding_crc.html
        if (!crc)
            crc ^= Params::init;
        while (len--)
        {
            crc = lookup_table[ (crc ^ *buf) ];
            ++buf;
        }
        return (crc); 
    }

private:
    constexpr static typename Params::value_t
    dynamic_sum_chunk( typename Params::value_t val )
    {
        if (!Params::refin)
            val = reflect_bits<Params::width>(val);

        for (unsigned bit = 0; bit < Params::width; bit++)
            val = val & 1 ?
                    Params::rev_poly ^ (val >> 1) :
                    val >> 1;

        if (!Params::refout)
            val = reflect_bits<Params::width>(val);
        return val;
    }

    constexpr static auto make_lookup_table()
    {
        return algorithm::make_lookup_table(
                std::make_index_sequence<(0b1U<<Params::width)>{}  // TODO: change 'Params::width' to 8
            );
    }

    template< std::size_t ... idx >
    constexpr static auto make_lookup_table( std::index_sequence<idx...> )
    {
        return std::array<typename Params::value_t, sizeof...(idx)>{
                dynamic_sum_chunk(idx)...
            };
    }
};

}  // namespace crc


/**
 * do tests on site:
 * http://www.sunshine2k.de/coding/javascript/crc/crc_js.html
 */
// int main()
// {
//     typedef crc::algorithm< crc::params::crc8_itu > CrcAlgo;
//     CrcAlgo::value_t first_chunk = 1;
//     const auto buf = std::to_array< CrcAlgo::value_t >({2, 3});
//     return CrcAlgo::sum(buf.data(), buf.size()/**/, CrcAlgo::sum_without_final_xor(&first_chunk, 1)/**/);
// }
