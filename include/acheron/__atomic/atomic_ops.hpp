/* this file is a part of Acheron library which is under MIT license; see LICENSE for more info */

#pragma once

#include <acheron/__atomic/atomic.hpp>

namespace ach
{
    /**
     * @brief Store a value with optimized memory ordering for publication
     * @tparam T Type to store
     * @param atom Atomic object to store into
     * @param value Value to store
     */
    template <typename T>
    void publish_store(atomic<T>& atom, T value) noexcept 
    {
    #ifdef ACH_ARCH_X86
        /* on x86, a simple store is sufficient for publication */
        atom.store(value, memory_order::relaxed);
    #else
        /* on ARM, we need a release store */
        atom.store(value, memory_order::release);
     #endif
     }
 
    /**
     * @brief Load a value with optimized memory ordering for consumption
     * @tparam T Type to load
     * @param atom Atomic object to load from
     * @return T The loaded value
     */
    template <typename T>
    T consume_load(const atomic<T>& atom) noexcept 
    {
    #ifdef ACH_ARCH_X86
        /* on x86, a simple load is sufficient for consumption */
        return atom.load(memory_order::relaxed);
    #else
         /* on ARM, we need an acquire load */
        return atom.load(memory_order::acquire);
    #endif
     }
 
    /**
     * @brief Atomic fetch-add with architecture-optimized memory ordering
     * @tparam T Type to operate on
     * @param atom Atomic object to modify
     * @param value Value to add
     * @return T The value before addition
     */
    template <typename T>
    T fetch_add(atomic<T>& atom, T value) noexcept 
    {
        static_assert(std::is_integral<T>::value || std::is_pointer<T>::value,
                        "fetch_add is only available for integral and pointer types");
    #ifdef ACH_ARCH_X86
        /* on x86, RMW operations already provide strong ordering guarantees */
         return atom.fetch_add(value, memory_order::relaxed);
    #else
        /* on ARM, we need acq_rel for safe synchronization */
        return atom.fetch_add(value, memory_order::acq_rel);
    #endif
    }
 
    /**
     * @brief Atomic fetch-sub with architecture-optimized memory ordering
     * @tparam T Type to operate on
     * @param atom Atomic object to modify
     * @param value Value to subtract
     * @return T The value before subtraction
     */
    template <typename T>
    T fetch_sub(atomic<T>& atom, T value) noexcept 
    {
        static_assert(std::is_integral<T>::value || std::is_pointer<T>::value,
                      "fetch_sub is only available for integral and pointer types");
    #ifdef ACH_ARCH_X86
        return atom.fetch_sub(value, memory_order::relaxed);
    #else
        return atom.fetch_sub(value, memory_order::acq_rel);
    #endif
    }
 
    /**
     * @brief Compare-and-exchange with architecture-optimized memory ordering
     * @tparam T Type to operate on
     * @param atom Atomic object to modify
     * @param expected Expected value (updated if comparison fails)
     * @param desired Value to store if comparison succeeds
     * @return bool True if the exchange was successful
     */
    template <typename T>
    bool compare_exchange(atomic<T>& atom, T& expected, T desired) noexcept 
    {
    #ifdef ACH_ARCH_X86
        return atom.compare_exchange_strong(expected, desired, 
                                            memory_order::relaxed, 
                                            memory_order::relaxed);
    #else
        return atom.compare_exchange_strong(expected, desired, 
                                            memory_order::acq_rel, 
                                            memory_order::acquire);
    #endif
    }
 
    /**
     * @brief Atomic exchange with architecture-optimized memory ordering
     * @tparam T Type to operate on
     * @param atom Atomic object to modify
     * @param value Value to store
     * @return T The previous value
     */
    template <typename T>
    T exchange(atomic<T>& atom, T value) noexcept 
    {
    #ifdef ACH_ARCH_X86
        return atom.exchange(value, memory_order::relaxed);
    #else
        return atom.exchange(value, memory_order::acq_rel);
    #endif
    }
 
    /**
     * @brief Atomic increment with architecture-optimized memory ordering
     * @tparam T Type to operate on
     * @param atom Atomic object to modify
     * @return T The value after increment
     */
    template <typename T>
    T increment(atomic<T>& atom) noexcept 
    {
        static_assert(std::is_integral<T>::value, "increment is only available for integral types");
        return fetch_add(atom, static_cast<T>(1)) + static_cast<T>(1);
    }
 
     /**
      * @brief Atomic decrement with architecture-optimized memory ordering
      * @tparam T Type to operate on
      * @param atom Atomic object to modify
      * @return T The value after decrement
      */
    template <typename T>
    T decrement_optimal(atomic<T>& atom) noexcept 
    {
        static_assert(std::is_integral<T>::value, "decrement is only available for integral types");
        return fetch_sub_optimal(atom, static_cast<T>(1)) - static_cast<T>(1);
    }
     
    /**
     * @brief Helper for double-checked locking pattern
     * @tparam T Type to operate on
     * @param atom Atomic flag to check
     * @param init_func Function to initialize value
     */
    template <typename T, typename Func>
    void double_checked_init(atomic<bool>& initialized, T& value, Func init_func) noexcept 
    {
        /* check without acquiring lock first */
        if (!consume_load(initialized)) 
        {
            /* if still not initialized after acquiring lock */
            if (!consume_load(initialized))
            {
                /* initialize the value */
                init_func(value);
            #ifdef ACH_ARCH_X86
                publish_store(initialized, true);
            #else
                /* for full visibility, we need to release the fence */
                release_fence();
                publish_store(initialized, true);
            #endif
            }
        }
     }
     
    /**
     * @brief Correctly implement a spin-wait with architecture-specific optimizations
     * @tparam T Type to check
     * @param atom The atomic value to wait on
     * @param expected_value The value to wait for
     */
    template <typename T>
    void spin_wait_until_equals(const atomic<T>& atom, T expected_value) 
    {
    #ifdef ACH_ARCH_X86
        /* on x86, a simple relaxed load is sufficient and will be optimized by CPU */
        while (atom.load(memory_order::relaxed) != expected_value) 
        {
            #if defined(_MSC_VER)
                _mm_pause();
            #elif defined(__GNUC__)
                __builtin_ia32_pause();
            #endif
         }
    #else
        while (atom.load(memory_order::acquire) != expected_value) 
        {
            /* wait hint */
            #if defined(__ARM_ARCH) && __ARM_ARCH >= 8
                __asm__ volatile("yield" ::: "memory");
            #endif
        }
    #endif
    }
}
