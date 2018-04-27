// -----------------------------------------------------------------------------------------
// <copyright file="memory.cpp" company="Microsoft">
//    Copyright 2013 Microsoft Corporation
//
//    Licensed under the Apache License, Version 2.0 (the "License");
//    you may not use this file except in compliance with the License.
//    You may obtain a copy of the License at
//      http://www.apache.org/licenses/LICENSE-2.0
//
//    Unless required by applicable law or agreed to in writing, software
//    distributed under the License is distributed on an "AS IS" BASIS,
//    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//    See the License for the specific language governing permissions and
//    limitations under the License.
// </copyright>
// -----------------------------------------------------------------------------------------

#pragma once
#pragma warning(disable: 4634)

#include "stdafx.h"
#include "wascore/memory.h"
#include "cpprest/http_client.h"

namespace azure { namespace storage { namespace core {

    void* (*malloc_ptr)(size_t size) = nullptr;
    void(*free_ptr)(void* ptr) = nullptr;

    void set_malloc(void* (*storage_malloc)(size_t size))
    {
        if (storage_malloc)
        {
            malloc_ptr = storage_malloc;
            // TODO: Enable this when there CPPREST supports custom allocator.
            // web::set_malloc(storage_malloc);
        }
        else {
            malloc_ptr = &malloc;
            // TODO: Enable this when there CPPREST supports custom allocator.
            // web::set_malloc(malloc);
        }
    }

    void set_free(void(*storage_free)(void* ptr))
    {
        if (storage_free)
        {
            free_ptr = storage_free;
            // TODO: Enable this when there CPPREST supports custom free.
            // web::free_ptr(storage_free);
        }
        else {
            free_ptr = &free;
            // TODO: Enable this when there CPPREST supports custom free.
            // web::free_ptr(free);
        }
    }

    void* storage_malloc(size_t size)
    {
        return (*malloc_ptr)(size);
    }

    void storage_free(void* ptr)
    {
        (*free_ptr)(ptr);
    }

    dummy_singleton dummy_singleton::m_instance;
    dummy_lazy_singleton* dummy_lazy_singleton::m_instance;

    const dummy_singleton& dummy_singleton::instance()
    {
        return dummy_singleton::m_instance;
    };

    const dummy_static_func_singleton& dummy_static_func_singleton::instance()
    {
        static dummy_static_func_singleton inst;
        return inst;
    };

    const dummy_lazy_singleton * dummy_lazy_singleton::instance()
    {
        if (!m_instance)
        {
            m_instance = new dummy_lazy_singleton();
        }
        return dummy_lazy_singleton::m_instance;
    }

    class dummy_initializer {
        public:
            dummy_initializer() {
                set_malloc(malloc);
                set_free(free);
            }
    };

}}}  // namespace azure::storage::core

   // operator new/delete overloads
#pragma warning(disable: 4100)
void* operator new(size_t size)
{ 
    static azure::storage::core::dummy_initializer init;
    return azure::storage::core::storage_malloc(size);
}

void* operator new[](size_t size)
{
    static azure::storage::core::dummy_initializer init;
    return azure::storage::core::storage_malloc(size);
}
void* operator new(size_t size, const std::nothrow_t&) throw()
{
    static azure::storage::core::dummy_initializer init;
    return azure::storage::core::storage_malloc(size);
}
void* operator new[](size_t size, const std::nothrow_t&) throw()
{
    static azure::storage::core::dummy_initializer init;
    return azure::storage::core::storage_malloc(size);
}

void operator delete(void* ptr)
{ 
    azure::storage::core::storage_free(ptr);
}

void operator delete[](void* ptr)
{
    azure::storage::core::storage_free(ptr);
}

void operator delete(void* ptr, size_t size)
{
    azure::storage::core::storage_free(ptr);
}

void operator delete[](void* ptr, size_t size)
{
    azure::storage::core::storage_free(ptr);
}

void operator delete(void* ptr, size_t size, const std::nothrow_t&)
{
    azure::storage::core::storage_free(ptr);
}

void operator delete[](void* Ptr, size_t size, const std::nothrow_t&)
{
    azure::storage::core::storage_free(Ptr);
}

void operator delete(void* ptr, const std::nothrow_t&)
{
    azure::storage::core::storage_free(ptr);
}

void operator delete[](void* ptr, const std::nothrow_t&)
{
    azure::storage::core::storage_free(ptr);
}

#pragma warning(default: 4100)

#ifdef(_WIN32)
#include <windows.h>

BOOL WINAPI DllMain(_In_ HINSTANCE, _In_ DWORD fdwReason, _In_ LPVOID)
{
    if (fdwReason == DLL_PROCESS_ATTACH)
    {
        std::cout << "DLL attached." << std::endl;
    }

    if (fdwReason == DLL_PROCESS_DETACH)
    {
        azure::storage::core::set_malloc(nullptr);
        azure::storage::core::set_free(nullptr);
        std::cout << "DLL detached and customized allocator unset." << std::endl;
    }
    return TRUE;
}
#endif

#pragma warning(default: 4634)
