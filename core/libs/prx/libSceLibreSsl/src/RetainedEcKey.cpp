#include "prx/libSceLibreSsl/include/Api.hpp"
#include "prx/libSceLibreSsl/include/Error.hpp"
#include <atomic>
#include <limits>
#include <stdexcept>

static_assert(std::atomic_ref<std::int32_t>::required_alignment <= alignof(std::int32_t));

extern "C" LibreSslEcKey* APS5_VABI EVP_PKEY_get1_EC_KEY_nid_postfix(LibreSslEvpPkey* key) {
    if (key == nullptr) throw std::invalid_argument("EVP_PKEY_get1_EC_KEY: null key");
    if (key->type != 408) {
        LibreSslPutError_nid_no_patch(6, 0xfff, 142);
        return nullptr;
    }
    auto* ec = static_cast<LibreSslEcKey*>(key->key);
    if (ec == nullptr) throw std::invalid_argument("EVP_PKEY_get1_EC_KEY: null EC member");
    std::atomic_ref<std::int32_t> references(ec->references);
    auto previous = references.load();
    for (;;) {
        if (previous == std::numeric_limits<std::int32_t>::max())
            throw std::overflow_error("EVP_PKEY_get1_EC_KEY: reference count overflow");
        if (references.compare_exchange_weak(previous, previous + 1)) return ec;
    }
}
