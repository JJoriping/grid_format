#ifndef _MIXX_TAGGED_POINTER_H_
#define _MIXX_TAGGED_POINTER_H_
#include <mixx/mixx_env.h>

namespace mixx {

#if defined(MIXX_ARCH_AMD64)
#define MIXX_TAGGED_POINTER_SUPPORTED_ARCH
#else
//#error "Tagged pointer is not supported on this architecture"
#endif

#if defined(MIXX_ARCH_AMD64)

template <typename T>
class tagged_pointer {
public:
    using value_type = T;
    using tag_t = uint16_t;
    static_assert(sizeof(T*) == sizeof(uint64_t), "Pointer size must be 8; Check your environment");

    // default constructor: zero initialization
    tagged_pointer() noexcept: _compressed(nullptr) { }

    // copy constructor
    tagged_pointer(tagged_pointer const& other) = default;

    // construct from an explicit pointer value with a tag
    explicit tagged_pointer(T* const p, tag_t const tag = 0) { _pack(*this, p, tag); }

    // operator overloading for both lvalue & rvalue assignment
    MIXX_FORCEINLINE tagged_pointer& operator=(tagged_pointer const& rhs) {
        _compressed = rhs._compressed;
        return *this;
    }

    // operator overloading for an arbitrary pointer value
    MIXX_FORCEINLINE tagged_pointer& operator=(T* const p) {
        _pack(*this, p, get_tag());
        return *this;
    }

    MIXX_FORCEINLINE tagged_pointer& set_tag(tag_t const tag) {
        _set_tag(*this, tag);
        return *this;
    }

    MIXX_FORCEINLINE tagged_pointer& set_pointer(T* const p) {
        _set_pointer(*this, p);
        return *this;
    }

    MIXX_FORCEINLINE tagged_pointer& pack(T* const p, tag_t const tag) {
        _pack(*this, p, tag);
        return *this;
    }

    explicit MIXX_FORCEINLINE operator T*() const {
        return get_pointer();
    }

    MIXX_FORCEINLINE T& operator*() const {
        return *get_pointer();
    }

    MIXX_FORCEINLINE T* operator->() const {
        return get_pointer();
    }

    MIXX_FORCEINLINE operator bool() const {
        return get_pointer() != nullptr;
    }

    MIXX_FORCEINLINE bool operator==(tagged_pointer const& rhs) const {
        return get_pointer() == rhs.get_pointer();
    }

    MIXX_FORCEINLINE bool operator!=(tagged_pointer const& rhs) const {
        return get_pointer() != rhs.get_pointer();
    }

    MIXX_FORCEINLINE tag_t get_tag() const {
        return _tag[_tag_index];
    }

    MIXX_FORCEINLINE tag_t get_tag() const volatile {
        return _tag[_tag_index];
    }

    MIXX_FORCEINLINE T* get_pointer() const {
        return reinterpret_cast<T*>(get_compressed() & _pointer_mask);
    }

    MIXX_FORCEINLINE T* get_pointer() const volatile {
        return reinterpret_cast<T*>(get_compressed() & _pointer_mask);
    }

    MIXX_FORCEINLINE uint64_t get_compressed() const {
        return reinterpret_cast<uint64_t>(_compressed);
    }

    MIXX_FORCEINLINE uint64_t get_compressed() const volatile {
        return reinterpret_cast<uint64_t>(_compressed);
    }

protected:
    constexpr static uint64_t _pointer_mask = 0xffffffffffffUL; //(1L << 48L) - 1;
    constexpr static int _tag_index = 3;

    static MIXX_FORCEINLINE void _pack(tagged_pointer& dst, T* const p, tag_t const tag) {
        dst._compressed = p;
        dst._tag[_tag_index] = tag;
    }

    static MIXX_FORCEINLINE void _set_tag(tagged_pointer& dst, tag_t const tag) {
        dst._tag[_tag_index] = tag;
    }

    static MIXX_FORCEINLINE void _set_pointer(tagged_pointer& dst, T* const p) {
        const tag_t old = dst.get_tag();
        dst._compressed = p;
        dst.set_tag(old);
    }

    union {
        uint16_t _tag[4];
        T* _compressed;
    };
};

#endif

} // !namespace mixx

#endif // !_MIXX_TAGGED_POINTER_H_