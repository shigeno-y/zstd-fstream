//
// SPDX-License-Identifier:	BSL-1.0
//

/*
 * Base works are from POCO version 1.13.3.
 */

#ifndef SHIGENOY_ZSTDSTREAM_HPP_INCLUDED
#define SHIGENOY_ZSTDSTREAM_HPP_INCLUDED

#include "zstd.h"

#include "Poco/BufferedStreamBuf.h"
#include "Poco/Foundation.h"

#include <istream>
#include <memory>
#include <ostream>
#include <vector>

namespace shigenoy::zstdstream {
class ZSTDCContextFreener
{
public:
    void operator()(ZSTD_CCtx* ctx) { ZSTD_freeCCtx(ctx); };
};

class ZSTDDContextFreener
{
public:
    void operator()(ZSTD_DCtx* ctx) { ZSTD_freeDCtx(ctx); };
};

class Foundation_API ZstdStreamBuf : public Poco::BufferedStreamBuf
{
private:
    const size_t PRE_PROCESS_BUFFER_SIZE;
    const size_t POST_PROCESSED_BUFFER_SIZE;

    std::vector<char> pre_buffer_;
    std::vector<char> post_buffer_;
    std::istream* pIstr_{ nullptr };
    std::ostream* pOstr_{ nullptr };
    bool eof_{ false };
    std::unique_ptr<ZSTD_CCtx, ZSTDCContextFreener> cctx_{ nullptr };
    std::unique_ptr<ZSTD_DCtx, ZSTDDContextFreener> dctx_{ nullptr };

public:
    ZstdStreamBuf(std::istream& istr);
    /// Creates a DeflatingStreamBuf for compressing data read
    /// from the given input stream.

    ZstdStreamBuf(std::istream& istr, int windowBits);
    /// Creates a DeflatingStreamBuf for compressing data read
    /// from the given input stream.
    ///
    /// Please refer to the zlib documentation of deflateInit2() for a description
    /// of the windowBits parameter.

    ZstdStreamBuf(std::ostream& ostr, int level);
    /// Creates a DeflatingStreamBuf for compressing data passed
    /// through and forwarding it to the given output stream.

    ZstdStreamBuf(std::ostream& ostr, int windowBits, int level);
    /// Creates a DeflatingStreamBuf for compressing data passed
    /// through and forwarding it to the given output stream.
    ///
    /// Please refer to the zlib documentation of deflateInit2() for a description
    /// of the windowBits parameter.

    ~ZstdStreamBuf();
    /// Destroys the DeflatingStreamBuf.

    int close();
    /// Finishes up the stream.
    ///
    /// Must be called when deflating to an output stream.

protected:
    int readFromDevice(char* buffer, std::streamsize length);
    int writeToDevice(const char* buffer, std::streamsize length);
    virtual int sync();
};

class Foundation_API ZstdIOS : public virtual std::ios
/// The base class for ZstdOutputStream and DeflatingInputStream.
///
/// This class is needed to ensure the correct initialization
/// order of the stream buffer and base classes.
{
protected:
    ZstdStreamBuf buf_;

public:
    ZstdIOS(std::ostream& ostr, int level);
    /// Creates a ZstdIOS for compressing data passed
    /// through and forwarding it to the given output stream.

    ZstdIOS(std::ostream& ostr, int windowBits, int level);
    /// Creates a ZstdIOS for compressing data passed
    /// through and forwarding it to the given output stream.
    ///
    /// Please refer to the zlib documentation of deflateInit2() for a description
    /// of the windowBits parameter.

    ZstdIOS(std::istream& istr);
    /// Creates a ZstdIOS for compressing data read
    /// from the given input stream.

    ZstdIOS(std::istream& istr, int windowBits);
    /// Creates a ZstdIOS for compressing data read
    /// from the given input stream.
    ///
    /// Please refer to the zlib documentation of deflateInit2() for a description
    /// of the windowBits parameter.

    ~ZstdIOS();
    /// Destroys the ZstdIOS.

    ZstdStreamBuf* rdbuf();
    /// Returns a pointer to the underlying stream buffer.
};

class Foundation_API ZstdOutputStream : public std::ostream, public ZstdIOS
/// This stream compresses all data passing through it
/// using zlib's deflate algorithm.
/// After all data has been written to the stream, close()
/// must be called to ensure completion of compression.
/// Example:
///     std::ofstream ostr("data.gz", std::ios::binary);
///     ZstdOutputStream deflater(ostr, DeflatingStreamBuf::STREAM_GZIP);
///     deflater << "Hello, world!" << std::endl;
///     deflater.close();
///     ostr.close();
{
public:
    ZstdOutputStream(std::ostream& ostr, int level);
    /// Creates a ZstdOutputStream for compressing data passed
    /// through and forwarding it to the given output stream.

    ZstdOutputStream(std::ostream& ostr, int windowBits, int level);
    /// Creates a ZstdOutputStream for compressing data passed
    /// through and forwarding it to the given output stream.
    ///
    /// Please refer to the zlib documentation of deflateInit2() for a description
    /// of the windowBits parameter.

    ~ZstdOutputStream();
    /// Destroys the ZstdOutputStream.

    int close();
    /// Finishes up the stream.
    ///
    /// Must be called when deflating to an output stream.

protected:
    virtual int sync();
};

class Foundation_API ZstdInputStream : public std::istream, public ZstdIOS
/// This stream compresses all data passing through it
/// using zlib's deflate algorithm.
{
public:
    ZstdInputStream(std::istream& istr);
    /// Creates a DeflatingIOS for compressing data read
    /// from the given input stream.

    ZstdInputStream(std::istream& istr, int windowBits);
    /// Creates a DeflatingIOS for compressing data read
    /// from the given input stream.
    ///
    /// Please refer to the zlib documentation of deflateInit2() for a description
    /// of the windowBits parameter.

    ~ZstdInputStream();
    /// Destroys the DeflatingInputStream.
};

} // namespace shigenoy::zstdstream

#endif // Foundation_DeflatingStream_INCLUDED
