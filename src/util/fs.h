#pragma once
#include <stdint.h>
#include <exception>
#include <filesystem>
#include <tuple>

#include "util.h"

namespace fs{
    class File{
        public:
        using filebuf_t = Util::ArrPtr<uint8_t>;
        private:
        std::filesystem::path filepath;
        filebuf_t filebuf;

        public:
        inline File(std::filesystem::path &filepath): filepath(filepath){}
        inline File(File&& fromThis){
            filepath = std::move(fromThis.filepath);
            filebuf  = std::move(fromThis.filebuf);
        }
        inline ~File(){
            filebuf.free();
        }
        
        inline bool isFile(){ return std::filesystem::is_regular_file(filepath); }
        void load();

        inline filebuf_t buf(){ return filebuf; }
        inline uint8_t* raw(){ return filebuf.ptr; }
        inline size_t length(){ return filebuf.len; }

        // Access contents
        inline uint8_t& operator[](size_t i){ return filebuf[i]; }
        // Access with bounds checking
        inline std::tuple<uint8_t, bool> at(size_t i){ 
            if(i < filebuf.len){
                return {filebuf[i], true};
            }else{
                return {0, false};
            }
         }
    };
}