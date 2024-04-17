// SPDX-License-Identifier: Apache-2.0

#ifndef SHIGENOY_LIBZSTDSTREAM_IMPL_HPP_INCLUDED
#define SHIGENOY_LIBZSTDSTREAM_IMPL_HPP_INCLUDED

#include <filesystem>
#include <fstream>
#include <memory>
#include <vector>

#include <zstd.h>

namespace shigenoy::zstdstream::internal {
class ZSTDContextFreener
{
public:
    void operator()(ZSTD_DCtx* ctx) { ZSTD_freeDCtx(ctx); };
};

class zstd_decompress_filter_impl
{
public:
    using char_type = char;

protected:
    const size_t buffInSize;
    const size_t buffOutSize;

    std::filesystem::path target_;
    std::ifstream ifs_;

    std::unique_ptr<ZSTD_DCtx, ZSTDContextFreener> ctx_{ nullptr };
    std::vector<char_type> buffIn_;
    std::vector<char_type> buffOut_;

public:
    zstd_decompress_filter_impl(const std::filesystem::path& target);
    zstd_decompress_filter_impl(const zstd_decompress_filter_impl& copy) = delete;
    zstd_decompress_filter_impl(zstd_decompress_filter_impl&& move)      = default;
    ~zstd_decompress_filter_impl()                                       = default;

public:
    size_t required_capacity() const;
    std::vector<char_type> read(size_t wonder);
};
} // namespace shigenoy::zstdstream::internal

#endif
