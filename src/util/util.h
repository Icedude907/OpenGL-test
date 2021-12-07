#pragma once

#include <vkfw/vkfw.hpp>

namespace Util{
    // A pointer that can implicitly be cast from and to any class.
    // Doesn't seem to be optimised
    struct AnyPtr{
        void* ptr;
        template <typename T> inline AnyPtr(T* ptr): ptr(reinterpret_cast<T*>(ptr)){}
                              inline AnyPtr(std::nullptr_t): ptr(nullptr){}
        template <typename T> inline operator T*(){ return reinterpret_cast<T*>(ptr); }
        template <typename T> inline T* operator->(){ return reinterpret_cast<T*>(ptr); }
    };
    template <typename T, typename Len_t = size_t>
    struct LenPtr{
        T* ptr;
        Len_t len;
        using ThisType = LenPtr<T, Len_t>;

        // basic
        inline LenPtr(){}
        inline LenPtr(Len_t len): ptr(new T[len]), len(len){}
        inline LenPtr(T* ptr, Len_t len): ptr(ptr), len(len){}
        // copy
        inline LenPtr(const ThisType& fromThis): ptr(fromThis.ptr), len(fromThis.len){};
        constexpr ThisType& operator=(const ThisType&) = default;

        // conversion
        template <typename fromT, typename fromLen_t>
        LenPtr(const LenPtr<fromT, fromLen_t>& fromThis): ptr(reinterpret_cast<T*>(fromThis.ptr)), len(fromThis.len){}

        inline T& operator[](size_t i){ return ptr[i]; }
        inline void free(){ delete[] ptr; }
    };
    template <typename T>
    struct PostfixAccumulator{
        T data;
        inline PostfixAccumulator(T data): data(data){}
        inline T postfixAdd(const T& rhs){
            T initial = data;
            data += rhs;
            return initial;
        }
    };
}