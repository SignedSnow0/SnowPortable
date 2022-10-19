#pragma once
#include <atomic>
#include "Core/Common.h"

namespace Snow {
    template<typename T>
    class ResourcePtr;

    template<typename T>
    class Resource {
    private:
        std::atomic<i32> mRefCount{ 0 };
        friend class ResourcePtr<T>;
    };

    template<typename T>
    class ResourcePtr {
    public:
        ResourcePtr()
            : mItem{ nullptr } { }

        ResourcePtr(std::nullptr_t)
            : mItem{ nullptr } { }

        ResourcePtr(T* item)
            : mItem{ item } {
            mItem->mRefCount++;
        }

        template<typename... Args>
        ResourcePtr(Args&&... args)
            : mItem{ new T(std::forward<Args>(args)...) } {
            mItem->mRefCount++;
        }

        ResourcePtr(const ResourcePtr& other)
            : mItem{ other.mItem } {
            if (mItem) {
                mItem->mRefCount++;
            }
        }

        ResourcePtr(ResourcePtr&& other) noexcept
            : mItem{ other.mItem } {
            other.mItem = nullptr;
        }

        ResourcePtr& operator=(const ResourcePtr& other) {
            if (other.mItem) {
                mItem = other.mItem;
                mItem->mRefCount++;
            }
            
            return *this;
        }

        ResourcePtr& operator=(ResourcePtr&& other) noexcept {
            if (other.mItem) {
                mItem = other.mItem;
                other.mItem = nullptr;
            }

            return *this;
        }

        ~ResourcePtr() {
            if (mItem) {
                mItem->mRefCount--;
                if (mItem->mRefCount <= 0) {
                    delete mItem;
                }
            }
        }

        operator T* () { return mItem; }
        operator const T* () const { return mItem; }

        operator bool() const { return mItem != nullptr; }

        T* operator&() { return mItem; }
        const T* operator&() const { return mItem; }

        T* operator->() { return mItem; }
        const T* operator->() const { return mItem; }

        T& operator*() { return *mItem; }
        const T& operator*() const { return *mItem; }

        bool operator==(const ResourcePtr& other) const { return std::addressof(mItem) == std::addressof(other.mItem); }
        bool operator==(const T* other) const { return std::addressof(mItem) == other; }
        
        bool operator!=(const ResourcePtr& other) const { return std::addressof(mItem) != std::addressof(other.mItem); }
        bool operator!=(const T* other) const { return std::addressof(mItem) != other; }

    private:
        T* mItem{ nullptr };
    };
}