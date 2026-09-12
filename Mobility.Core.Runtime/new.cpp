/*
 * PROJECT:    Mobility
 * FILE:       new.cpp
 * PURPOSE:    Implementation for the C++ Standard Library <new> header
 *
 * LICENSE:    The MIT License
 *
 * MAINTAINER: MouriNaruto (Kenji.Mouri@outlook.com)
 */

#include "new"

#include "cstdint"

#include <Mile.Mobility.Utilities.MemoryAccess.h>

namespace
{
    MO_UINT32 volatile g_NewHandlerLock = 0;
    std::new_handler g_NewHandler = nullptr;

    void AcquireNewHandlerLock() noexcept
    {
        while (0 != ::MoMileInterlockedCompareExchange32(
            &g_NewHandlerLock,
            1,
            0))
        {
        }
    }

    void ReleaseNewHandlerLock() noexcept
    {
        ::MoMileInterlockedExchange32(&g_NewHandlerLock, 0);
    }
}

namespace std
{
    const nothrow_t nothrow;

    new_handler get_new_handler() noexcept
    {
        AcquireNewHandlerLock();

        new_handler Handler = g_NewHandler;

        ReleaseNewHandlerLock();

        return Handler;
    }

    new_handler set_new_handler(
        new_handler Handler) noexcept
    {
        AcquireNewHandlerLock();

        new_handler PreviousHandler = g_NewHandler;
        g_NewHandler = Handler;

        ReleaseNewHandlerLock();

        return PreviousHandler;
    }
}

void* operator new(
    std::size_t Size)
{
    // TODO
    for (;;)
    {
        void* Block = nullptr;
        if (Block)
        {
            return Block;
        }

        std::new_handler Handler = std::get_new_handler();
        if (!Handler)
        {
            if (Size == SIZE_MAX)
            {
                // TODO: throw std::bad_array_new_length();
            }
            else
            {
                // TODO: throw std::bad_alloc();
            }
            return nullptr;
        }
        Handler();
    }
}

#ifdef __cpp_aligned_new
void* operator new(
    std::size_t Size,
    std::align_val_t Alignment)
{
    // TODO
    MO_UNREFERENCED_PARAMETER(Alignment);

    for (;;)
    {
        void* Block = nullptr;
        if (Block)
        {
            return Block;
        }

        std::new_handler Handler = std::get_new_handler();
        if (!Handler)
        {
            if (Size == SIZE_MAX)
            {
                // TODO: throw std::bad_array_new_length();
            }
            else
            {
                // TODO: throw std::bad_alloc();
            }
            return nullptr;
        }
        Handler();
    }
}
#endif

void* operator new(
    std::size_t Size,
    const std::nothrow_t& Tag) noexcept
{
    MO_UNREFERENCED_PARAMETER(Tag);
    /*try
    {
        return operator new(Size);
    }
    catch (...)
    {
        return nullptr;
    }*/
    return operator new(Size);
}

#ifdef __cpp_aligned_new
void* operator new(
    std::size_t Size,
    std::align_val_t Alignment,
    const std::nothrow_t& Tag) noexcept
{
    MO_UNREFERENCED_PARAMETER(Tag);
    /*try
    {
        return operator new(Size, Alignment);
    }
    catch (...)
    {
        return nullptr;
    }*/
    return operator new(Size, Alignment);
}
#endif

void operator delete(
    void* Block) noexcept
{
    // TODO
    MO_UNREFERENCED_PARAMETER(Block);
}

#ifdef __cpp_sized_deallocation
void operator delete(
    void* Block,
    std::size_t Size) noexcept
{
    MO_UNREFERENCED_PARAMETER(Size);
    operator delete(Block);
}
#endif

#ifdef __cpp_aligned_new
void operator delete(
    void* Block,
    std::align_val_t Alignment) noexcept
{
    // TODO
    MO_UNREFERENCED_PARAMETER(Block);
    MO_UNREFERENCED_PARAMETER(Alignment);
}

#ifdef __cpp_sized_deallocation
void operator delete(
    void* Block,
    std::size_t Size,
    std::align_val_t Alignment) noexcept
{
    MO_UNREFERENCED_PARAMETER(Size);
    operator delete(Block, Alignment);
}
#endif
#endif

void operator delete(
    void* Block,
    const std::nothrow_t& Tag) noexcept
{
    MO_UNREFERENCED_PARAMETER(Tag);
    operator delete(Block);
}

#ifdef __cpp_aligned_new
void operator delete(
    void* Block,
    std::align_val_t Alignment,
    const std::nothrow_t& Tag) noexcept
{
    MO_UNREFERENCED_PARAMETER(Tag);
    operator delete(Block, Alignment);
}
#endif

void* operator new[](
    std::size_t Size)
{
    return operator new(Size);
}

#ifdef __cpp_aligned_new
void* operator new[](
    std::size_t Size,
    std::align_val_t Alignment)
{
    return operator new(Size, Alignment);
}
#endif

void* operator new[](
    std::size_t Size,
    const std::nothrow_t& Tag) noexcept
{
    MO_UNREFERENCED_PARAMETER(Tag);
    /*try
    {
        return operator new[](Size);
    }
    catch (...)
    {
        return nullptr;
    }*/
    return operator new[](Size);
}

#ifdef __cpp_aligned_new
void* operator new[](
    std::size_t Size,
    std::align_val_t Alignment,
    const std::nothrow_t& Tag) noexcept
{
    MO_UNREFERENCED_PARAMETER(Tag);
    /*try
    {
        return operator new[](Size, Alignment);
    }
    catch (...)
    {
        return nullptr;
    }*/
    return operator new[](Size, Alignment);
}
#endif

void operator delete[](
    void* Block) noexcept
{
    operator delete(Block);
}

#ifdef __cpp_sized_deallocation
void operator delete[](
    void* Block,
    std::size_t Size) noexcept
{
    MO_UNREFERENCED_PARAMETER(Size);
    operator delete[](Block);
}
#endif

#ifdef __cpp_aligned_new
void operator delete[](
    void* Block,
    std::align_val_t Alignment) noexcept
{
    operator delete(Block, Alignment);
}

#ifdef __cpp_sized_deallocation
void operator delete[](
    void* Block,
    std::size_t Size,
    std::align_val_t Alignment) noexcept
{
    MO_UNREFERENCED_PARAMETER(Size);
    operator delete[](Block, Alignment);
}
#endif
#endif

void operator delete[](
    void* Block,
    const std::nothrow_t& Tag) noexcept
{
    MO_UNREFERENCED_PARAMETER(Tag);
    operator delete[](Block);
}

#ifdef __cpp_aligned_new
void operator delete[](
    void* Block,
    std::align_val_t Alignment,
    const std::nothrow_t& Tag) noexcept
{
    MO_UNREFERENCED_PARAMETER(Tag);
    operator delete[](Block, Alignment);
}
#endif
