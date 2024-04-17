// SPDX-License-Identifier: Apache-2.0

#ifndef SHIGENOY_LIBZSTDSTREAM_HPP_INCLUDED
#define SHIGENOY_LIBZSTDSTREAM_HPP_INCLUDED

#include <filesystem>
#include <memory>
#include <streambuf>

namespace shigenoy::zstdstream {
namespace internal {
class zstd_decompress_filter_impl;
}

class zstd_decompress_filter : public std::basic_streambuf<char>
{
public:
    using Super = std::basic_streambuf<char>;

private:
    std::unique_ptr<internal::zstd_decompress_filter_impl> impl_{ nullptr };

public:
    zstd_decompress_filter(const std::filesystem::path& target);
    zstd_decompress_filter(const zstd_decompress_filter& copy) = delete;
    zstd_decompress_filter(zstd_decompress_filter&& move);
    ~zstd_decompress_filter();

protected:
    virtual std::streamsize xsgetn(char_type* buf, std::streamsize n);
    virtual int_type underflow();
    virtual int_type uflow();

protected:
    virtual std::streamsize xsputn(const char_type* buf, std::streamsize n) = 0;
    virtual int sync()                                                      = 0;
    virtual int_type overflow(int_type nextValue)                           = 0;
};
} // namespace shigenoy::zstdstream

#endif
