// -----------------------------------------------------------------------------------------
// <copyright file="memory.h" company="Microsoft">
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

#include "cpprest/http_client.h"
#include "basic_types.h"

namespace azure { namespace storage { namespace core {
    extern void* (*malloc_ptr)(size_t size);
    extern void(*free_ptr)(void* ptr);
    extern void set_malloc(void* (*storage_malloc)(size_t size));
    extern void set_free(void(*storage_free)(void* ptr));
    extern void* storage_malloc(size_t size);
    extern void storage_free(void* ptr);

    // This is to test normal singleton class //debug
    class dummy_singleton {
    public:
        WASTORAGE_API static const dummy_singleton& instance();

        ~dummy_singleton()
        {
            std::cout << "dummy singleton deleted" << std::endl;
        }

    private:
        dummy_singleton()
        {
            std::cout << "dummy singleton initialized" << std::endl;
        }

        static dummy_singleton m_instance;
    };

    // This is to test normal lazy singleton class declared with static function variable //debug
    class dummy_static_func_singleton {
    public:
        WASTORAGE_API static const dummy_static_func_singleton& instance();

        ~dummy_static_func_singleton()
        {
            std::cout << "dummy singleton with static function variable deleted" << std::endl;
        }

    private:
        dummy_static_func_singleton()
        {
            std::cout << "dummy singleton with static function variable initialized" << std::endl;
        }
    };

    // This is to test a lazy-initialization singleton class //debug
    class dummy_lazy_singleton {
    public:
        WASTORAGE_API static const dummy_lazy_singleton* instance();

        ~dummy_lazy_singleton()
        {
            std::cout << "dummy lazy singleton deleted" << std::endl;
        }

    private:
        dummy_lazy_singleton()
        {
            std::cout << "dummy lazy singleton initialized" << std::endl;
        }

        static dummy_lazy_singleton* m_instance;
    };

}}}  // namespace azure::storage::core

// operator new/delete overloads
#pragma warning(disable: 4100)
extern void* operator new  (size_t size);
extern void* operator new[](size_t size);
extern void operator delete  (void* ptr);
extern void operator delete[](void* ptr);
extern void operator delete  (void* ptr, size_t size);
extern void operator delete[](void* ptr, size_t size);

extern void* operator new  (size_t size, const std::nothrow_t&) throw();
extern void* operator new[](size_t size, const std::nothrow_t&) throw();
extern void operator delete  (void* ptr, size_t size, const std::nothrow_t&);
extern void operator delete[](void* Ptr, size_t size, const std::nothrow_t&);
extern void operator delete  (void* ptr, const std::nothrow_t&);
extern void operator delete[](void* ptr, const std::nothrow_t&);
#pragma warning(default: 4100)

#ifdef(_WIN32)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
BOOL WINAPI DllMain(_In_ HINSTANCE, _In_ DWORD fdwReason, _In_ LPVOID);
#endif

#pragma warning(default: 4634)