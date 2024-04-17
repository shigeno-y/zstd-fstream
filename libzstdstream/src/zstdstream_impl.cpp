// SPDX-License-Identifier: Apache-2.0

#include "zstdstream/zstdstream_impl.hpp"

#include <iterator>

shigenoy::zstdstream::internal::zstd_decompress_filter_impl::zstd_decompress_filter_impl(
    const std::filesystem::path& target)
    : buffInSize(ZSTD_DStreamInSize())
    , buffOutSize(ZSTD_DStreamOutSize())
    , target_(std::filesystem::canonical(target))
    , ifs_(target_, std::ios_base::in | std::ios_base::binary)
    , ctx_(ZSTD_createDCtx())
{
    this->buffIn_.resize(this->buffInSize);
    this->buffOut_.resize(this->buffOutSize);
}

size_t
shigenoy::zstdstream::internal::zstd_decompress_filter_impl::required_capacity() const
{
    return this->buffOutSize;
}

std::vector<shigenoy::zstdstream::internal::zstd_decompress_filter_impl::char_type>
shigenoy::zstdstream::internal::zstd_decompress_filter_impl::read(size_t wonder)
{
    std::vector<char_type> ret{};
    if (this->ifs_)
    {
        size_t const toRead = buffInSize;
        size_t read;

        if ((read = this->ifs_.readsome(this->buffIn_.data(), toRead)))
        {
            ZSTD_inBuffer input = { buffIn_.data(), read, 0 };
            /* Given a valid frame, zstd won't consume the last byte of the frame
             * until it has flushed all of the decompressed data of the frame.
             * Therefore, instead of checking if the return code is 0, we can
             * decompress just check if input.pos < input.size.
             */
            while (input.pos < input.size)
            {
                ZSTD_outBuffer output = { buffOut_.data(), buffOut_.size(), 0 };
                /* The return code is zero if the frame is complete, but there may
                 * be multiple frames concatenated together. Zstd will automatically
                 * reset the context when a frame is complete. Still, calling
                 * ZSTD_DCtx_reset() can be useful to reset the context to a clean
                 * state, for instance if the last decompression call returned an
                 * error.
                 */
                ZSTD_decompressStream(this->ctx_.get(), &output, &input);
                ret.reserve(ret.size() + this->buffOutSize);
                std::copy(buffOut_.cbegin(), buffOut_.cend(), std::back_inserter(ret));
            }
        }
    }
    return ret;
}