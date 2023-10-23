#ifndef SIGNED_RLE_HPP
#define SIGNED_RLE_HPP

#include <vector>
#include <cstdint>

namespace signed_RLE
{
    std::vector<int8_t> uncompress(const std::vector<int8_t> & compressed, int limit = 0);
    std::vector<int8_t> compress(const std::vector<int8_t> & raw);
};

#endif // SIGNED_RLE_HPP
