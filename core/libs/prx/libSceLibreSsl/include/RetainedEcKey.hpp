#pragma once

#include "SceTypes.hpp"
#include <cstddef>
#include <cstdint>

struct LibreSslEcKey {
    std::int32_t version;
    void* group;
    void* publicKey;
    void* privateKey;
    std::uint32_t encodingFlags;
    std::int32_t conversionForm;
    std::int32_t references;
    std::int32_t flags;
    void* extraData;
};

struct LibreSslEvpPkey {
    std::int32_t type;
    std::int32_t savedType;
    std::int32_t references;
    const void* method;
    void* engine;
    void* key;
    std::int32_t saveParameters;
    void* attributes;
};

static_assert(sizeof(LibreSslEcKey) == 56);
static_assert(offsetof(LibreSslEcKey, references) == 40);
static_assert(sizeof(LibreSslEvpPkey) == 56);
static_assert(offsetof(LibreSslEvpPkey, key) == 32);

extern "C" LibreSslEcKey* APS5_VABI EVP_PKEY_get1_EC_KEY_nid_postfix(LibreSslEvpPkey* key);
