#include "prx/libSceLibreSsl3/libSceLibreSsl3.hpp"

#include <atomic>
#include <limits>
#include <stdexcept>

namespace {

static_assert(std::atomic_ref<std::int32_t>::required_alignment <= alignof(std::int32_t));
static_assert(sizeof(LibreSslX509Store) == 136);
static_assert(offsetof(LibreSslX509Store, references) == 128);

int Retain_nid_no_patch(std::int32_t& references) {
    std::atomic_ref<std::int32_t> count(references);
    auto previous = count.load();
    for (;;) {
        if (previous == std::numeric_limits<std::int32_t>::max())
            throw std::overflow_error("LibreSSL3 key reference count overflow");
        if (count.compare_exchange_weak(previous, previous + 1))
            return previous > 0;
    }
}

} // namespace

extern "C" {

int APS5_VABI EVP_PKEY_up_ref_nid_postfix(LibreSsl3EvpPkey* key) {
    if (!key)
        throw std::invalid_argument("EVP_PKEY_up_ref requires a key");
    return Retain_nid_no_patch(key->references);
}

int APS5_VABI EC_KEY_up_ref_nid_postfix(LibreSsl3EcKey* key) {
    if (!key)
        throw std::invalid_argument("EC_KEY_up_ref requires a key");
    return Retain_nid_no_patch(key->references);
}

int APS5_VABI SSL_CTX_up_ref_nid_postfix(LibreSsl3SslCtx* ctx) {
    if (!ctx)
        throw std::invalid_argument("SSL_CTX_up_ref requires a context");
    return Retain_nid_no_patch(ctx->references);
}

int APS5_VABI X509_STORE_up_ref_nid_postfix(LibreSslX509Store* store) {
    if (!store)
        throw std::invalid_argument("X509_STORE_up_ref requires a store");
    return Retain_nid_no_patch(store->references);
}

void* APS5_VABI EVP_PKEY_get0_DH_nid_postfix(LibreSsl3EvpPkey* key) {
    if (!key)
        throw std::invalid_argument("EVP_PKEY_get0_DH requires a key");
    if (key->type == NidDhKeyAgreement)
        return key->key;
    return nullptr;
}

void* APS5_VABI EVP_PKEY_get0_DSA_nid_postfix(LibreSsl3EvpPkey* key) {
    if (!key)
        throw std::invalid_argument("EVP_PKEY_get0_DSA requires a key");
    if (key->type == NidDsa)
        return key->key;
    return nullptr;
}

void* APS5_VABI EVP_PKEY_get0_RSA_nid_postfix(LibreSsl3EvpPkey* key) {
    if (!key)
        throw std::invalid_argument("EVP_PKEY_get0_RSA requires a key");
    if (key->type == NidRsaEncryption || key->type == NidRsassaPss)
        return key->key;
    return nullptr;
}

}
