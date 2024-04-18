// SPDX-License-Identifier: Apache-2.0

#include "zstdstream/zstdstream.hpp"

#include <Poco/StreamCopier.h>

#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>

int
main()
{
    const std::filesystem::path origin{ __FILE__ };
    const std::filesystem::path input = origin.parent_path() / "CMakeLists.txt.zst";

    std::ifstream is(input, std::ios_base::binary | std::ios_base::in);

    shigenoy::zstdstream::ZstdInputStream zi(is);

    while (zi)
    {
        std::cout << zi.get();
    }
}