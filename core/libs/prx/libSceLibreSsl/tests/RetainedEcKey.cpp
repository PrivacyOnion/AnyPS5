#include "prx/libSceLibreSsl/libSceLibreSsl.hpp"
#include "prx/libSceLibreSsl/include/Ec.hpp"
#include <array>
#include <cstdio>
#include <cstdlib>
#include <limits>
#include <source_location>
#include <stdexcept>
#include <thread>

namespace {

void Require(bool condition, std::source_location location = std::source_location::current()) {
    if (condition) return;
    std::fprintf(stderr, "check failed at line %u\n", location.line());
    std::abort();
}

void ConstructorAndRelease() {
    for (const int nid : {415, 714, 716}) {
        EcKey* original = EC_KEY_new_by_curve_name_nid_postfix(nid);
        Require(original != nullptr);
        const EcGroup* group = EC_KEY_get0_group_nid_postfix(original);
        const int degree = EC_GROUP_get_degree_nid_postfix(group);
        const int signatureSize = ECDSA_size_nid_postfix(original);
        LibreSslEvpPkey envelope{.type = 408, .references = 7, .key = original};
        EcKey* retained = EVP_PKEY_get1_EC_KEY_nid_postfix(&envelope);
        Require(retained == original && original->references.load() == 2);
        Require(envelope.references == 7);
        EC_KEY_free_nid_postfix(original);
        Require(retained->references.load() == 1);
        Require(EC_KEY_get0_group_nid_postfix(retained) == group);
        Require(EC_GROUP_get_degree_nid_postfix(EC_KEY_get0_group_nid_postfix(retained)) == degree);
        Require(ECDSA_size_nid_postfix(retained) == signatureSize);
        EC_KEY_free_nid_postfix(retained);
    }
    EC_KEY_free_nid_postfix(nullptr);
}

void AccessAndErrors() {
    EcKey* ec = EC_KEY_new_nid_postfix();
    Require(ec != nullptr && ec->references.load() == 1);
    LibreSslEvpPkey key{.type = 408, .references = 7, .key = ec, .saveParameters = 1};
    ERR_clear_error_nid_postfix();
    for (int count = 2; count != 12; ++count) {
        Require(EVP_PKEY_get1_EC_KEY_nid_postfix(&key) == ec);
        Require(ec->references.load() == count && key.references == 7);
    }
    Require(ERR_get_error_nid_postfix() == 0);
    key.type = 6;
    Require(EVP_PKEY_get1_EC_KEY_nid_postfix(&key) == nullptr);
    Require(ec->references.load() == 11 && key.references == 7);
    Require(ERR_get_error_nid_postfix() == 0x06fff08e);
    Require(ERR_get_error_nid_postfix() == 0);
    key.type = 408;
    key.key = nullptr;
    bool nullMember = false;
    try {
        EVP_PKEY_get1_EC_KEY_nid_postfix(&key);
    } catch (const std::invalid_argument&) {
        nullMember = true;
    }
    Require(nullMember && ERR_get_error_nid_postfix() == 0);
    key.key = ec;
    ec->references.store(std::numeric_limits<std::int32_t>::max());
    bool overflow = false;
    try {
        EVP_PKEY_get1_EC_KEY_nid_postfix(&key);
    } catch (const std::overflow_error&) {
        overflow = true;
    }
    Require(overflow && ec->references.load() == std::numeric_limits<std::int32_t>::max());
    Require(ERR_get_error_nid_postfix() == 0);
    ec->references.store(11);
    bool nullKey = false;
    try {
        EVP_PKEY_get1_EC_KEY_nid_postfix(nullptr);
    } catch (const std::invalid_argument&) {
        nullKey = true;
    }
    Require(nullKey);
    for (int count = 11; count != 0; --count) {
        Require(ec->references.load() == count);
        EC_KEY_free_nid_postfix(ec);
    }
}

void ConcurrentRetainsAndReleases() {
    EcKey* ec = EC_KEY_new_by_curve_name_nid_postfix(415);
    Require(ec != nullptr);
    LibreSslEvpPkey key{.type = 408, .references = 3, .key = ec, .saveParameters = 1};
    std::array<std::thread, 6> threads;
    for (auto& thread : threads) {
        thread = std::thread([&] {
            for (int repeat = 0; repeat < 1000; ++repeat) {
                EcKey* retained = EVP_PKEY_get1_EC_KEY_nid_postfix(&key);
                Require(retained == ec);
                Require(EC_GROUP_get_degree_nid_postfix(EC_KEY_get0_group_nid_postfix(retained)) == 256);
                EC_KEY_free_nid_postfix(retained);
            }
        });
    }
    for (auto& thread : threads) thread.join();
    Require(ec->references.load() == 1 && key.references == 3);
    EC_KEY_free_nid_postfix(ec);
}

} // namespace

int main() {
    ConstructorAndRelease();
    AccessAndErrors();
    ConcurrentRetainsAndReleases();
    std::puts("retained EC key lifecycle checks passed");
}
