#pragma once

#include <vkfw/vkfw.hpp>

namespace Util{
    /// A pointer that can implicitly be cast from and to any class.
    /// Made purely out of hate for void* casting. C++ has some unusual decisions
    struct AnyPtr{
        // Not sure if the compiler is optimising this properly.
        void* ptr;
        template <typename T> inline AnyPtr(T* ptr): ptr(reinterpret_cast<T*>(ptr)){}
                              inline AnyPtr(std::nullptr_t): ptr(nullptr){}
        template <typename T> inline operator T*(){ return reinterpret_cast<T*>(ptr); }
        template <typename T> inline T* operator->(){ return reinterpret_cast<T*>(ptr); }
    };

    /// A pointer + length wrapper type for keeping track of fixed arrays on the heap.
    /// Deletes the underlying ptr on being free()d
    template <typename T, typename Len_t = size_t>
    struct ArrPtr{
        T* ptr;
        Len_t len;
        using ThisType = ArrPtr<T, Len_t>;

        // basic
        inline ArrPtr(){}
        inline ArrPtr(Len_t len): ptr(new T[len]), len(len){}
        inline ArrPtr(T* ptr, Len_t len): ptr(ptr), len(len){}
        // copy
        inline ArrPtr(const ThisType& fromThis): ptr(fromThis.ptr), len(fromThis.len){};
        constexpr ThisType& operator=(const ThisType&) = default;

        // conversion
        template <typename fromT, typename fromLen_t>
        ArrPtr(const ArrPtr<fromT, fromLen_t>& fromThis): ptr(reinterpret_cast<T*>(fromThis.ptr)), len(fromThis.len){}

        inline T& operator[](size_t i){ return ptr[i]; }
        inline void free(){ delete[] ptr; }
    };

    /// Utility class that accumulates values, but returns the initial value instead.
    /// Used to automatically link buffers in main().
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