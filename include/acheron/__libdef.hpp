/* this file is a part of Acheron library which is under MIT license; see LICENSE for more info */

#pragma once

#if defined(_WIN32) || defined(__CYGWIN__)
    #ifdef LIBACHERON_EXPORTS
        #define ACHERON_API __declspec(dllexport)
    #else
        #define ACHERON_API __declspec(dllimport)
    #endif
    #define ACHERON_LOCAL
#else
    #if defined(__GNUC__) || defined(__clang__)
        #define ACHERON_API __attribute__ ((visibility ("default")))
        #define ACHERON_LOCAL __attribute__ ((visibility ("hidden")))
    #else
        #define ACHERON_API
        #define ACHERON_LOCAL
    #endif
#endif

namespace ach
{
    using uint8_t = unsigned char;
    using uint16_t = unsigned short;
    using uint32_t = unsigned int;
    using uint64_t = unsigned long;

    using int8_t = char;
    using int16_t = short;
    using int32_t = int;
    using int64_t = long;

    using size_t = unsigned long;

    using uintptr_t = unsigned long;
    using intptr_t = long;
    using ptrdiff_t = typeof(static_cast<int*>(nullptr) - static_cast<int*>(nullptr));
}

#define LIBACHERON inline

#if defined(__GNUC__) || defined(__clang__)
    #define ACHERON_LIKELY(x)   __builtin_expect(!!(x), 1)
    #define ACHERON_UNLIKELY(x) __builtin_expect(!!(x), 0)
#else
    #define ACHERON_LIKELY(x)   (x)
    #define ACHERON_UNLIKELY(x) (x)
#endif

#define ACHERON_STATIC_ASSERT(cond, msg) static_assert(cond, msg)

#define ACHERON_NOCOPY(T) \
    T(const T&) = delete; \
    T& operator=(const T&) = delete;

#define ACHERON_NOMOVE(T) \
    T(T&&) = delete; \
    T& operator=(T&&) = delete;

#define ACHERON_MAKE(T, ...) \
    static T create(__VA_ARGS__)

#if defined(__GNUC__) || defined(__clang__)
    #define ACHERON_FORCE_INLINE __attribute__((always_inline)) inline
    #define ACHERON_NOINLINE __attribute__((noinline))
#elif defined(_MSC_VER)
    #define ACHERON_FORCE_INLINE __forceinline
    #define ACHERON_NOINLINE __declspec(noinline)
#else
    #define ACHERON_FORCE_INLINE inline
    #define ACHERON_NOINLINE
#endif
