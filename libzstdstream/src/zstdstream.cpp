// SPDX-License-Identifier: Apache-2.0

#include "zstdstream/zstdstream.hpp"
#include "zstdstream/zstdstream_impl.hpp"

shigenoy::zstdstream::zstd_decompress_filter::zstd_decompress_filter(
    const std::filesystem::path& target)
    : impl_(std::make_unique<internal::zstd_decompress_filter_impl>(target))
{
}

shigenoy::zstdstream::zstd_decompress_filter::zstd_decompress_filter(
    zstd_decompress_filter&& move) = default;

shigenoy::zstdstream::zstd_decompress_filter::~zstd_decompress_filter() = default;

std::streamsize
shigenoy::zstdstream::zstd_decompress_filter::xsgetn(char_type* buf, std::streamsize n)
{
    return std::streamsize();
}
