#ifndef SIGNED_RLE_HPP
#define SIGNED_RLE_HPP

#include <vector>
#include <span>
#include <cstdint>
#include <optional>

namespace signed_RLE
{
    std::vector<int8_t> uncompress(std::span<const int8_t> compressed, std::optional<unsigned int> limit = std::nullopt);
    std::vector<int8_t> compress(std::span<const int8_t> raw);
};

#endif // SIGNED_RLE_HPP
