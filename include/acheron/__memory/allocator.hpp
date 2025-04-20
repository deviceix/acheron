/* this file is a part of Acheron library which is under MIT license; see LICENSE for more info */

#pragma once

#include <limits>
#include <memory>
#include <acheron/__libdef.hpp>
#if defined(__unix__) || defined(__unix) || (defined(__APPLE__) && defined(__MACH__))
/* we compile for Unix e.g. Linux, macOS and so on */
#include <sys/mman.h>
#elif defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#error "Windows is not supported yet, but will be soon; hopefully"
#else /* not supported at the moment, but will be soon; hopefully */
#error "your OS not supported at the moment, but will be soon; hopefully"
#endif

namespace ach
{
    /**
     * @brief Memory allocator with pool-based allocation strategy
     *
     * @note This allocator implements the C++ standard allocator interface while providing
     *  optimized memory allocation through size-based pools for small allocations and
     *  direct mmap for larger ones.
     *
     * @note This is not STL-compatible by any means. The containers may crash if this is used
     *  with the standard library
     * @tparam T Type of objects to allocate
     */
    template<typename T>
    class allocator
    {
    public:
        using value_type = T;
        using pointer = T *;
        using const_pointer = const T *;
        using reference = T &;
        using const_reference = const T &;
        using size_type = size_t;
        using difference_type = ptrdiff_t;
        using propagate_on_container_move_assignment = std::true_type;
        using is_always_equal = std::true_type;

        /**
         * @brief Rebinds the allocator to a different type
         *
         * This nested template provides a mechanism to obtain an allocator
         * for a different type while preserving all other allocator properties.
         * Required by the Standard Allocator concept.
         *
         * @tparam U The type to rebind the allocator to
         */
        template<typename U>
        struct rebind
        {
            using other = allocator<U>;
        };

        /**
         * @brief Default constructor
         *
         * Initializes a new allocator and prepares all size classes for allocation.
         * This constructor is marked constexpr and noexcept to allow for compile-time
         * initialization and exception safety guarantees.
         */
        constexpr allocator() noexcept
        {
            init_size_classes();
        }

        /**
         * @brief Copy constructor
         *
         * Creates a new allocator that is a copy of another allocator of the same type.
         * Since this allocator is stateless with respect to allocator equality, this operation
         * is trivial and marked as default.
         *
         * @param other The allocator to copy from
         */
        constexpr allocator(const allocator &) noexcept = default;

        /**
         * @brief Converting constructor
         *
         * Creates a new allocator from an allocator of a different type.
         * This allows for conversion between allocators of different types,
         * which is required by the Standard Allocator concept.
         *
         * @tparam U The type of the source allocator
         */
        template<typename U>
        constexpr allocator(const allocator<U> &) noexcept {}

        /**
         * @brief Destructor
         *
         * Cleans up all allocated memory pools. Iterates through each size class
         * and releases all associated memory pools back to the operating system.
         */
        ~allocator()
        {
            /* traverse and clean the pool :D */
            for (auto pool: pools)
            {
                while (pool)
                {
                    Pool *next = pool->next;
                    delete pool;
                    pool = next;
                }
            }
        }

        /**
         * @brief Allocate memory for n objects of type T
         *
         * Allocates contiguous storage for n objects of type T. For small allocations,
         * memory is drawn from size-specific pools. For large allocations, memory is
         * directly mapped from the operating system.
         *
         * @param n Number of objects to allocate memory for
         * @return pointer The pointer to the allocated memory, or nullptr if n is zero
         * @note The allocated memory is not initialized
         */
        [[nodiscard]] pointer allocate(size_type n)
        {
            if (n == 0)
                return nullptr;

            const size_type bytes_needed = n * sizeof(T);

            if (bytes_needed >= LARGE_THRESHOLD)
                return static_cast<pointer>(allocate_large(bytes_needed));
            return static_cast<pointer>(allocate_from_size_class(get_size_class(bytes_needed)));
        }

        /**
         * @brief Deallocate memory previously allocated with allocate
         *
         * Returns memory previously obtained from allocate back to the allocator.
         * For pool-based allocations, the memory is returned to the appropriate free list.
         * For large allocations, the memory is unmapped from the operating system.
         *
         * @param p Pointer to the memory to deallocate
         * @param n Number of objects the memory was allocated for; unused but required by standard
         * @note The behavior is undefined if p was not previously allocated by this allocator
         */
        void deallocate(pointer p, size_type) noexcept
        {
            if (!p)
                return;

            if (!BlockHeader::is_aligned(p))
                return;

            auto *header = reinterpret_cast<BlockHeader *>(
                reinterpret_cast<unsigned char *>(p) - sizeof(BlockHeader)
            );
            if (!header->is_valid())
                return;

            if (header->is_mmap())
            {
                const size_t total_size = header->size() + sizeof(BlockHeader);
                const size_t aligned_size = (total_size + PAGE_SIZE - 1) & ~(PAGE_SIZE - 1);
                munmap(header, aligned_size);
                return;
            }

            uint8_t size_class = header->size_class();
            if (size_class >= SIZE_CLASSES)
                return;

            header->set_free(true);
            header->next = free_lists[size_class];
            free_lists[size_class] = header;
        }

        /**
         * @brief Returns the maximum number of objects that can be allocated
         *
         * Determines the theoretical maximum number of objects of type T that can
         * be allocated by this allocator. This is constrained by the maximum value
         * of size_type divided by the size of T.
         *
         * @return size_type The maximum number of objects that can be allocated
         */
        [[nodiscard]] size_type max_size() const noexcept
        {
            return std::numeric_limits<size_type>::max() / sizeof(T);
        }

        /**
         * @brief Construct an object at the given address
         *
         * Constructs an object of type U at the given memory location using placement new.
         * The constructor is called with the provided arguments forwarded.
         *
         * @tparam U Type of object to construct
         * @tparam Args Types of arguments to forward to the constructor
         * @param p Pointer to memory where the object should be constructed
         * @param args Arguments to forward to the constructor
         * @note This is noexcept if the constructor of U is noexcept
         */
        template<typename U, typename... Args>
        void construct(U *p, Args &&... args) noexcept(std::is_nothrow_constructible_v<U, Args...>)
        {
            ::new(static_cast<void *>(p)) U(std::forward<Args>(args)...);
        }

        /**
         * @brief Destroy an object at the given address
         *
         * Calls the destructor of the object at the given memory location without
         * deallocating the memory. This is used to clean up objects before their
         * memory is deallocated.
         *
         * @tparam U Type of object to destroy
         * @param p Pointer to the object to destroy
         * @note This is noexcept if the destructor of U is noexcept
         */
        template<typename U>
        void destroy(U *p) noexcept(std::is_nothrow_destructible_v<U>)
        {
            p->~U();
        }

    private:
        /* notes: on x86_64, the cache line is guaranteed to be 64 fixed-bytes,
         * however; ARM-based is implementation defined. 64 bytes is a good-enough
         * guess as false sharing shouldn't happen because this allocator is
         * designed to run on a single thread. */
        static constexpr size_t CACHE_LINE_SIZE = 64;
        static constexpr size_t PAGE_SIZE = 4096;
        static constexpr size_t ALIGNMENT = CACHE_LINE_SIZE;

        static constexpr size_t TINY_THRESHOLD = 64;
        static constexpr size_t SMALL_THRESHOLD = 256;
        static constexpr size_t MEDIUM_THRESHOLD = 4096;
        static constexpr size_t LARGE_THRESHOLD = 1024 * 1024;

        static constexpr size_t SIZE_CLASSES = 32;
        static constexpr size_t TINY_CLASSES = 8;

        static constexpr auto HEADER_MAGIC = 0xDEADBEEF12345678;
        static constexpr uint64_t SIZE_MASK = 0x0000FFFFFFFFFFFF;
        static constexpr uint64_t CLASS_MASK = 0x00FF000000000000;
        static constexpr auto MAGIC_MASK = 0xF000000000000000;
        static constexpr auto MAGIC_VALUE = 0xA000000000000000;
        static constexpr uint64_t FREE_FLAG = 1ULL << 63;
        static constexpr uint64_t MMAP_FLAG = 1ULL << 62;

        class BlockHeader
        {
        public:
            uint64_t data;
            uint64_t magic;
            BlockHeader *next; /* next block in the free list */

            void init(const size_t size, uint8_t size_class, const bool is_free)
            {
                magic = HEADER_MAGIC;
                data = (size & SIZE_MASK) |
                       (static_cast<uint64_t>(size_class) << 48) |
                       (static_cast<uint64_t>(is_free) << 63) |
                       MAGIC_VALUE;
                next = nullptr;
            }

            /* this takes like 3 cycles at worse so it should be superfast */
            [[nodiscard]] bool is_valid() const
            {
                return (magic == HEADER_MAGIC) &&
                       ((data & MAGIC_MASK) == MAGIC_VALUE) &&
                       (size() <= (1ULL << 47));
            }

            [[nodiscard]] bool is_free() const
            {
                return (data & FREE_FLAG) != 0;
            }

            [[nodiscard]] bool is_mmap() const
            {
                return (data & MMAP_FLAG) != 0;
            }

            [[nodiscard]] size_t size() const
            {
                return data & SIZE_MASK;
            }

            [[nodiscard]] uint8_t size_class() const
            {
                return (data & CLASS_MASK) >> 48;
            }

            /* mutators */
            void set_free(const bool is_free)
            {
                data = (data & ~FREE_FLAG) | (static_cast<uint64_t>(is_free) << 63);
            }

            void set_mmap(const bool is_mmap)
            {
                data = (data & ~MMAP_FLAG) | (static_cast<uint64_t>(is_mmap) << 62);
            }

            static bool is_aligned(const void *ptr)
            {
                if ((reinterpret_cast<uintptr_t>(ptr) & (ALIGNMENT - 1)) != 0)
                    return false;

                const auto *header = reinterpret_cast<const BlockHeader *>(
                    static_cast<const char *>(ptr) - sizeof(BlockHeader));

                return header->magic == HEADER_MAGIC;
            }
        };

        struct SizeClass
        {
            uint16_t size;   /* base size of this class */
            uint16_t slot;   /* actual allocation size; with alignment */
            uint16_t blocks; /* number of blocks per page */
        };

        class alignas(PAGE_SIZE) Pool
        {
        public:
            uint8_t *memory; /* memory region */
            size_t cap;      /* total capacity in bytes */
            BlockHeader *free_list;
            Pool *next; /* next pool in chain */

            explicit Pool(const size_t size)
            {
                /* TODO: add win32 API here */
                memory = static_cast<uint8_t *>(mmap(nullptr, size, PROT_READ | PROT_WRITE,
                                                     MAP_PRIVATE | MAP_ANONYMOUS, -1, 0));
                if (memory == MAP_FAILED)
                    throw std::bad_alloc();

                cap = size;
                free_list = nullptr;
                next = nullptr;
            }

            ~Pool()
            {
                if (memory != nullptr)
                {
                    munmap(memory, cap);
                    memory = nullptr;
                }
            }
        };

        SizeClass size_classes[SIZE_CLASSES] = {};
        BlockHeader *free_lists[SIZE_CLASSES] = {};
        Pool *pools[SIZE_CLASSES] = {};

        void init_size_classes()
        {
            for (size_t i = 0; i < SIZE_CLASSES; ++i)
            {
                const size_t size = 1ULL << (i + 3); /* 8, 16, 32, ... */
                const size_t alignment = (size > ALIGNMENT) ? size : ALIGNMENT;
                const size_t slot_size = (size + sizeof(BlockHeader) + alignment - 1) & ~(alignment - 1);

                size_classes[i].size = size;
                size_classes[i].slot = slot_size;
                size_classes[i].blocks = PAGE_SIZE / slot_size;

                free_lists[i] = nullptr;
                pools[i] = nullptr;
            }
        }

        static uint8_t get_size_class(size_t size)
        {
            if (size <= TINY_THRESHOLD)
            {
                return (size - 1) >> 3; /* 8 bytes increments */
            }

            /* find the next power of 2 ceiling */
            size_t n = size - 1;
            n |= n >> 1;
            n |= n >> 2;
            n |= n >> 4;
            n |= n >> 8;
            n |= n >> 16;
            n |= n >> 32;
            return (63 - __builtin_clzll(n + 1)) - 3; /* log2 - 3 */
        }

        void allocate_pool(uint8_t size_class)
        {
            const size_t pool_size = PAGE_SIZE;
            Pool *new_pool = new Pool(pool_size);

            if (!new_pool->memory)
            {
                delete new_pool;
                return;
            }

            new_pool->next = pools[size_class];
            pools[size_class] = new_pool;

            /* make freelist */
            const SizeClass &sc = size_classes[size_class];
            const size_t block_size = sc.slot;
            const size_t num_blocks = sc.blocks;
            for (size_t i = 0; i < num_blocks; ++i)
            {
                auto *header = reinterpret_cast<BlockHeader *>(new_pool->memory + i * block_size);
                header->init(sc.size, size_class, true);
                header->next = new_pool->free_list;
                new_pool->free_list = header;
            }
        }

        void *allocate_from_size_class(uint8_t size_class)
        {
            if (free_lists[size_class])
            {
                BlockHeader *header = free_lists[size_class];
                if (!header)
                {
                    // Reset corrupted free list and try again
                    free_lists[size_class] = nullptr;
                    return allocate_from_size_class(size_class);
                }

                free_lists[size_class] = header->next;
                header->set_free(false);
                return reinterpret_cast<char *>(header) + sizeof(BlockHeader);
            }

            if (!pools[size_class] ||
                pools[size_class]->free_list == nullptr)
            {
                allocate_pool(size_class);
            }

            /* should have blocks now or hopefully should */
            if (pools[size_class] && pools[size_class]->free_list)
            {
                BlockHeader *header = pools[size_class]->free_list;
                if (!header)
                {
                    pools[size_class]->free_list = nullptr;
                    allocate_pool(size_class);
                    if (pools[size_class] && pools[size_class]->free_list)
                        return allocate_from_size_class(size_class);
                    return nullptr;
                }

                pools[size_class]->free_list = header->next;
                header->set_free(false);
                return reinterpret_cast<char *>(header) + sizeof(BlockHeader);
            }

            return nullptr; /* failed */
        }

        static void *allocate_large(size_t size)
        {
            constexpr auto header_size = sizeof(BlockHeader);
            const size_t total_size = size + header_size;
            const size_t aligned_size = (total_size + PAGE_SIZE - 1) & ~(PAGE_SIZE - 1);

            /* TODO: add win32 API here */
            void *mem = mmap(nullptr, aligned_size,
                             PROT_READ | PROT_WRITE,
                             MAP_PRIVATE | MAP_ANONYMOUS,
                             -1, 0);

            if (mem == MAP_FAILED)
                return nullptr;

            auto *header = static_cast<BlockHeader *>(mem);
            header->init(size, 255, false); /* special size class for large allocations */
            header->set_mmap(true);
            return static_cast<char *>(mem) + header_size;
        }
    };

    template<typename T1, typename T2>
    bool operator==(const allocator<T1> &, const allocator<T2> &) noexcept
    {
        return true; /* all instances are considered equal for stateless allocators */
    }

    template<typename T1, typename T2>
    bool operator!=(const allocator<T1> &lhs, const allocator<T2> &rhs) noexcept
    {
        return !(lhs == rhs);
    }
}
