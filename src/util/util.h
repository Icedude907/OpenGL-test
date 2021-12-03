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
    template <typename T>
    struct LenPtr{
        T* ptr;
        size_t len;

        // basic
        inline LenPtr(){}
        inline LenPtr(size_t len): ptr(new T[len]), len(len){}
        inline LenPtr(T* ptr, size_t len): ptr(ptr), len(len){}
        // copy
        inline LenPtr(const LenPtr<T>& fromThis): ptr(fromThis.ptr), len(fromThis.len){};
        constexpr LenPtr<T>& operator=(const LenPtr<T>&) = default;
        // move
        // ptrWLen(ptrWLen<T>&& fromThis): ptr(fromThis.ptr), len(fromThis.len){
        //     fromThis.ptr = 0;
        //     fromThis.len = 0;
        // }

        // conversion
        template <typename fromType>
        LenPtr(const LenPtr<fromType>& fromThis): ptr(reinterpret_cast<T*>(fromThis.ptr)), len(fromThis.len){}

        inline T& operator[](size_t i){ return ptr[i]; }
        inline void free(){
            delete[] ptr;
        }
    };
    // BROKEN: REDO
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

    struct GLFWTimer{
        double lastFrameTime;
        double delta;
        inline void start(){
            lastFrameTime = vkfw::getTime();
            delta = 0.0f;
        }
        inline void update(){
            auto time = vkfw::getTime();
            delta = time - lastFrameTime;
            lastFrameTime = time;
        }
    };
}