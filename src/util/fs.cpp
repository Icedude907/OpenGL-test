#include "fs.h"

#include <fstream>

namespace fs{
    void File::load(){
        filebuf.len = std::filesystem::file_size(filepath);
        std::ifstream input(filepath, std::ios::binary | std::ios::in);
        filebuf.ptr = new uint8_t[filebuf.len];
        std::copy( 
            std::istreambuf_iterator<char>(input), 
            std::istreambuf_iterator<char>( ),
            filebuf.ptr);
    }
}