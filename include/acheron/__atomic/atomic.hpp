/* this file is a part of Acheron library which is under MIT license; see LICENSE for more info */

#pragma once

#include <atomic>
#include <type_traits>
#include <acheron/__libdef.hpp>

namespace ach
{
    #if defined(__x86__64) || defined(_M_X64) || defined(__i386__) || defined(_M_IX86)
    #define ACHERON_ARCH_X86
    #elif defined(__arm__) || defined(__aarch64__) || defined(_M_ARM) || defined(_M_ARM64)
    #define ACHERON_ARCH_ARM
    #else
    #define ACHERON_ARCH_GENERIC
    #endif

    /** 
     * @brief Memory order enums that have identical meaning across architectures
     */
    enum class memory_order
    {
        relaxed, /* no ordering constraints; the CPU can fully reorder the operations */
        acquire, /* no eads or writes can be reordered before this */
        release, /* no reads or writes can be reordered after this */
        acq_rel, /* combination of acquire and release */
        seq_cst /* full sequential consistency */
    };

    /**
     * @brief Architecture-aware atomic class - a wrapper around std::atomic
     * @tparam T The type to make atomic
     */
    template<typename T>
    class atomic
    {
    public:
        /**
         * @brief Convert the memory order to std::memory_order
         */
        static std::memory_order to_std_memory_order(memory_order order)
        {
            switch (order) 
            {
                case memory_order::relaxed: 
                    return std::memory_order_relaxed;
                case memory_order::acquire: 
                    return std::memory_order_acquire;
                case memory_order::release:
                    return std::memory_order_release;
                case memory_order::acq_rel: 
                    return std::memory_order_acq_rel;
                case memory_order::seq_cst:
                default: 
                    return std::memory_order_seq_cst;
            }
        }

        /**
         * @brief Get optimized load memory order for the current architecture
         */
        static std::memory_order get_optimized_load_order(memory_order order)
        {
            switch (order) 
            {
            #ifdef ACH_ARCH_X86
                /* on x86, loads have implicit acquire semantics, so `relaxed` can be used */
                case memory_order::relaxed: 
                    return std::memory_order_relaxed;
                case memory_order::acquire: 
                    return std::memory_order_relaxed; /* optimization */
            #else
                /* on ARM and other architectures, the requested ordering is maintained */
                case memory_order::relaxed: 
                    return std::memory_order_relaxed;
                case memory_order::acquire: 
                    return std::memory_order_acquire;
            #endif
                case memory_order::release: 
                    return std::memory_order_relaxed; /* not applicable for loads */
                case memory_order::acq_rel: 
                    return std::memory_order_acquire;
                case memory_order::seq_cst:
                default: 
                    return std::memory_order_seq_cst;
            }
        }

        /**
         * @brief Get optimized store memory order for the current architecture
         */
        static std::memory_order get_optimized_store_order(memory_order order) 
        {
            switch (order) 
            {
            #ifdef ACH_ARCH_X86
                case memory_order::relaxed: 
                    return std::memory_order_relaxed;
                case memory_order::release: 
                    return std::memory_order_relaxed;
            #else
                case memory_order::relaxed:
                    return std::memory_order_relaxed;
                case memory_order::release: 
                    return std::memory_order_release;
            #endif
                case memory_order::acquire: 
                    return std::memory_order_relaxed;
                case memory_order::acq_rel: 
                    return std::memory_order_release;
                case memory_order::seq_cst:
                default:
                return std::memory_order_seq_cst;
            }
        }

        /**
         * @brief Construct a new Atomic object
         * @param desired Initial value
         */
        explicit constexpr atomic(T desired = T()) noexcept : v(desired) {}

        /**
         * @brief Load the current value with optimized memory ordering
         * @param order Memory ordering constraint
         * @return T The current value
        */
        T load(memory_order order = memory_order::seq_cst) const noexcept
        {
            return v.load(get_optimized_load_order(order));
        }

        /**
         * @brief Store a new value with optimized memory ordering
         * @param desired Value to store
         * @param order Memory ordering constraint
         */
        void store(T desired, memory_order order = memory_order::seq_cst) noexcept 
        {
            v.store(desired, get_optimized_store_order(order));
        }

        /**
         * @brief Exchange the current value with a new one
         * @param desired Value to store
         * @param order Memory ordering constraint
         * @return T The previous value
         */
        T exchange(T desired, memory_order order = memory_order::seq_cst) noexcept 
        {
            return v.exchange(desired, to_std_memory_order(order));
        }

        /**
         * @brief Compare and exchange with weak guarantee
         * @param expected Expected value (updated if comparison fails)
         * @param desired Value to store if comparison succeeds
         * @param success_order Memory ordering for success
         * @param failure_order Memory ordering for failure
         * @return bool True if the exchange was successful
         */
        bool compare_exchange_weak(T& expected, T desired,
            memory_order success_order = memory_order::seq_cst,
            memory_order failure_order = memory_order::seq_cst) noexcept 
        {
            return v.compare_exchange_weak(
                expected,
                desired,
                to_std_memory_order(success_order),
                to_std_memory_order(failure_order)
            );
        }

        /**
         * @brief Compare and exchange with strong guarantee
         * @param expected Expected value; updated if comparison fails
         * @param desired Value to store if comparison succeeds
         * @param success_order Memory ordering for success
         * @param failure_order Memory ordering for failure
         * @return bool True if the exchange was successful
         */
        bool compare_exchange_strong(T& expected, T desired,
                memory_order success_order = memory_order::seq_cst,
                memory_order failure_order = memory_order::seq_cst) noexcept 
        {
            return v.compare_exchange_strong(
                expected,
                desired,
                to_std_memory_order(success_order),
                to_std_memory_order(failure_order)
            );
        }

        /**
         * @brief Atomic fetch-add operation
         * @param arg Value to add
         * @param order Memory ordering constraint
         * @return T The value before the addition
         */
        T fetch_add(T arg, memory_order order = memory_order::seq_cst) noexcept 
        {
            static_assert(std::is_integral<T>::value || std::is_pointer<T>::value, "fetch_add is only available for integral and pointer types");
            return v.fetch_add(arg, to_std_memory_order(order));
        }

        /**
         * @brief Atomic fetch-sub operation
         * @param arg Value to subtract
         * @param order Memory ordering constraint
         * @return T The value before the subtraction
         */
        T fetch_sub(T arg, memory_order order = memory_order::seq_cst) noexcept 
        {
            static_assert(std::is_integral<T>::value || std::is_pointer<T>::value, "fetch_sub is only available for integral and pointer types");
            return v.fetch_sub(arg, to_std_memory_order(order));
        }

        /**
         * @brief Atomic fetch-and operation
         * @param arg Value to bitwise AND
         * @param order Memory ordering constraint
         * @return T The value before the operation
         */
        T fetch_and(T arg, memory_order order = memory_order::seq_cst) noexcept 
        {
            static_assert(std::is_integral<T>::value, "fetch_and is only available for integral types");
            return v.fetch_and(arg, to_std_memory_order(order));
        }

        /**
         * @brief Atomic fetch-or operation
         * @param arg Value to bitwise OR
         * @param order Memory ordering constraint
         * @return T The value before the operation
         */
        T fetch_or(T arg, memory_order order = memory_order::seq_cst) noexcept 
        {
            static_assert(std::is_integral<T>::value, "fetch_or is only available for integral types");
            return v.fetch_or(arg, to_std_memory_order(order));
        }

        /**
         * @brief Atomic fetch-xor operation
         * @param arg Value to bitwise XOR
         * @param order Memory ordering constraint
         * @return T The value before the operation
         */
        T fetch_xor(T arg, memory_order order = memory_order::seq_cst) noexcept 
        {
            static_assert(std::is_integral<T>::value, "fetch_xor is only available for integral types");
            return v.fetch_xor(arg, to_std_memory_order(order));
        }

        T operator++() noexcept 
        {
            return fetch_add(1) + 1;
        }

        T operator++(int) noexcept 
        {
            return fetch_add(1);
        }

        T operator--() noexcept 
        {
            return fetch_sub(1) - 1;
        }

        T operator--(int) noexcept 
        {
            return fetch_sub(1);
        }

        T operator+=(T arg) noexcept 
        {
            return fetch_add(arg) + arg;
        }

        T operator-=(T arg) noexcept 
        {
            return fetch_sub(arg) - arg;
        }

        T operator&=(T arg) noexcept 
        {
            return fetch_and(arg) & arg;
        }

        T operator|=(T arg) noexcept 
        {
            return fetch_or(arg) | arg;
        }

        T operator^=(T arg) noexcept 
        {
            return fetch_xor(arg) ^ arg;
        }

    private:
        std::atomic<T> v;
    };

    /**
     * @brief Helper function to create an optimized memory fence
     * @param order Memory ordering constraint
     */
    LIBACHERON void thread_fence(memory_order order = memory_order::seq_cst) noexcept 
    {
        std::atomic_thread_fence(atomic<int>::to_std_memory_order(order));
    }

    /**
     * @brief Optimized acquire fence based on architecture
     */
    inline void acquire_fence() noexcept 
    {
    #ifdef ACH_ARCH_X86
        /* on X86, stores already have release semantics, so this can be a compiler fence */
        std::atomic_signal_fence(std::memory_order_acquire);
    #else
        /* on ARM and other platforms like PPC, we need full thread fence because it's not x86-TSO */
        std::atomic_thread_fence(std::memory_order_acquire);
    #endif
    }

    /**
     * @brief Optimized release fence based on architecture
     */
    LIBACHERON void release_fence() noexcept 
    {
    #ifdef ACH_ARCH_X86
        /* on X86, stores already have release semantics, so this can be a compiler fence */
        std::atomic_signal_fence(std::memory_order_release);
    #else
        /* on ARM and other platforms like PPC, we need full thread fence because it's not x86-TSO */
        std::atomic_thread_fence(std::memory_order_release);
    #endif
    }

    /* note: these are here just for `std` drop-ins compatibility */
    /**
     * @brief Architecture-agnostic acquire-release fence
     */
    LIBACHERON void acq_rel_fence() noexcept 
    {
        std::atomic_thread_fence(std::memory_order_acq_rel);
    }

    /**
     * @brief Architecture-agnostic sequential consistency fence
     */
    LIBACHERON void seq_cst_fence() noexcept 
    {
        std::atomic_thread_fence(std::memory_order_seq_cst);
    }
}
