//
// DeflatingStream.cpp
//
// Library: Foundation
// Package: Streams
// Module:  ZLibStream
//
// Copyright (c) 2004-2006, Applied Informatics Software Engineering GmbH.
// and Contributors.
//
// SPDX-License-Identifier:	BSL-1.0
//

#include "zstdstream/zstdstream.hpp"

#include "Poco/Exception.h"

#include <cstring>
#include <iterator>

shigenoy::zstdstream::ZstdStreamBuf::ZstdStreamBuf(std::istream& istr)
    : Poco::BufferedStreamBuf(ZSTD_BLOCKSIZE_MAX + 3, std::ios::in)
    , PRE_PROCESS_BUFFER_SIZE(ZSTD_BLOCKSIZE_MAX + 3)
    , POST_PROCESSED_BUFFER_SIZE(ZSTD_DStreamOutSize())
    , pIstr_(&istr)
    , dctx_(ZSTD_createDCtx())
{
}

shigenoy::zstdstream::ZstdStreamBuf::ZstdStreamBuf(std::istream& istr, int windowBits)
    : Poco::BufferedStreamBuf(ZSTD_BLOCKSIZE_MAX + 3, std::ios::in)
    , PRE_PROCESS_BUFFER_SIZE(ZSTD_BLOCKSIZE_MAX + 3)
    , POST_PROCESSED_BUFFER_SIZE(ZSTD_DStreamOutSize())
    , pIstr_(&istr)
    , dctx_(ZSTD_createDCtx())
{
}

shigenoy::zstdstream::ZstdStreamBuf::ZstdStreamBuf(std::ostream& ostr, int level)
    : Poco::BufferedStreamBuf(ZSTD_BLOCKSIZE_MAX, std::ios::out)
    , PRE_PROCESS_BUFFER_SIZE(ZSTD_BLOCKSIZE_MAX)
    , POST_PROCESSED_BUFFER_SIZE(ZSTD_CStreamOutSize())
    , pOstr_(&ostr)
    , cctx_(ZSTD_createCCtx())
{
}

shigenoy::zstdstream::ZstdStreamBuf::ZstdStreamBuf(std::ostream& ostr, int windowBits, int level)
    : Poco::BufferedStreamBuf(ZSTD_BLOCKSIZE_MAX, std::ios::out)
    , PRE_PROCESS_BUFFER_SIZE(ZSTD_BLOCKSIZE_MAX)
    , POST_PROCESSED_BUFFER_SIZE(ZSTD_CStreamOutSize())
    , pOstr_(&ostr)
    , cctx_(ZSTD_createCCtx())
{
}

shigenoy::zstdstream::ZstdStreamBuf::~ZstdStreamBuf()
{
    try
    {
        close();
    }
    catch (...)
    {
    }
}

int
shigenoy::zstdstream::ZstdStreamBuf::close()
{
    Poco::BufferedStreamBuf::sync();
    pIstr_ = 0;
    if (pOstr_)
    {
        //TODO impl
        pOstr_->flush();
        pOstr_ = 0;
    }
    return 0;
}

int
shigenoy::zstdstream::ZstdStreamBuf::sync()
{
    if (Poco::BufferedStreamBuf::sync())
    {
        return -1;
    }

    if (pOstr_)
    {
        // TODO impl

        // NOTE: This breaks the Zip library and causes corruption in some files.
        // See GH #1828
        // _pOstr->flush();
    }
    return 0;
}

int
shigenoy::zstdstream::ZstdStreamBuf::readFromDevice(char* buffer, std::streamsize length)
{
    if (!pIstr_)
    {
        return 0;
    }

    if (this->post_buffer_.size() < length)
    {
        this->pre_buffer_.resize(PRE_PROCESS_BUFFER_SIZE);
        std::uninitialized_fill(this->pre_buffer_.begin(), this->pre_buffer_.end(), '\0');
        if (size_t read =
                this->pIstr_->readsome(this->pre_buffer_.data(), this->pre_buffer_.size());
            read > 0)
        {
            ZSTD_inBuffer input = { this->pre_buffer_.data(), read, 0 };
            std::vector<char> tmpBuf(POST_PROCESSED_BUFFER_SIZE, '\0');
            while (input.pos < input.size)
            {
                ZSTD_outBuffer output = { this->post_buffer_.data(), this->post_buffer_.size(), 0 };
                ZSTD_decompressStream(this->dctx_.get(), &output, &input);

                this->post_buffer_.reserve(this->post_buffer_.size() + output.pos);
                std::copy(tmpBuf.begin(), tmpBuf.end(), std::back_inserter(this->post_buffer_));
                this->post_buffer_.resize(output.pos);
                std::uninitialized_fill(tmpBuf.begin(), tmpBuf.end(), '\0');
            }
        }
    }

    // copy
    const size_t actual_copy = std::min<size_t>(this->post_buffer_.size(), length);
    std::memmove(buffer, this->post_buffer_.data(), actual_copy);
    {
        std::vector<char> tmp(this->post_buffer_.cbegin() + actual_copy, this->post_buffer_.cend());
        this->post_buffer_.swap(tmp);
    }
    static_cast<int>(actual_copy);
}

int
shigenoy::zstdstream::ZstdStreamBuf::writeToDevice(const char* buffer, std::streamsize length)
{
    if (length == 0 || !pOstr_)
    {
        return 0;
    }
    // TODO impl
    return static_cast<int>(0);
}

shigenoy::zstdstream::ZstdIOS::ZstdIOS(std::ostream& ostr, int level): buf_(ostr, level)
{
    poco_ios_init(&buf_);
}

shigenoy::zstdstream::ZstdIOS::ZstdIOS(std::ostream& ostr, int windowBits, int level)
    : buf_(ostr, windowBits, level)
{
    poco_ios_init(&buf_);
}

shigenoy::zstdstream::ZstdIOS::ZstdIOS(std::istream& istr): buf_(istr)
{
    poco_ios_init(&buf_);
}

shigenoy::zstdstream::ZstdIOS::ZstdIOS(std::istream& istr, int windowBits): buf_(istr, windowBits)
{
    poco_ios_init(&buf_);
}

shigenoy::zstdstream::ZstdIOS::~ZstdIOS()
{
}

shigenoy::zstdstream::ZstdStreamBuf*
shigenoy::zstdstream::ZstdIOS::rdbuf()
{
    return &buf_;
}

shigenoy::zstdstream::ZstdOutputStream::ZstdOutputStream(std::ostream& ostr, int level)
    : std::ostream(&buf_), ZstdIOS(ostr, level)
{
}

shigenoy::zstdstream::ZstdOutputStream::ZstdOutputStream(std::ostream& ostr,
                                                         int windowBits,
                                                         int level)
    : std::ostream(&buf_), ZstdIOS(ostr, windowBits, level)
{
}

shigenoy::zstdstream::ZstdOutputStream::~ZstdOutputStream()
{
}

int
shigenoy::zstdstream::ZstdOutputStream::close()
{
    return buf_.close();
}

int
shigenoy::zstdstream::ZstdOutputStream::sync()
{
    return buf_.pubsync();
}

shigenoy::zstdstream::ZstdInputStream::ZstdInputStream(std::istream& istr)
    : std::istream(&buf_), ZstdIOS(istr)
{
}

shigenoy::zstdstream::ZstdInputStream::ZstdInputStream(std::istream& istr, int windowBits)
    : std::istream(&buf_), ZstdIOS(istr, windowBits)
{
}

shigenoy::zstdstream::ZstdInputStream::~ZstdInputStream()
{
}
