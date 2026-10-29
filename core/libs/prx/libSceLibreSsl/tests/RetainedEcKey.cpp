#include "prx/libSceLibreSsl/libSceLibreSsl.hpp"
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

void AccessAndErrors() {
    LibreSslEcKey ec{};
    ec.references = 1;
    LibreSslEvpPkey key{.type = 408, .references = 7, .key = &ec, .saveParameters = 1};
    ERR_clear_error_nid_postfix();
    for (int count = 2; count != 12; ++count) {
        Require(EVP_PKEY_get1_EC_KEY_nid_postfix(&key) == &ec);
        Require(ec.references == count && key.references == 7);
    }
    Require(ERR_get_error_nid_postfix() == 0);
    key.type = 6;
    Require(EVP_PKEY_get1_EC_KEY_nid_postfix(&key) == nullptr);
    Require(ec.references == 11 && key.references == 7);
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
    Require(nullMember);
    Require(ERR_get_error_nid_postfix() == 0);
    key.key = &ec;
    ec.references = std::numeric_limits<std::int32_t>::max();
    bool overflow = false;
    try {
        EVP_PKEY_get1_EC_KEY_nid_postfix(&key);
    } catch (const std::overflow_error&) {
        overflow = true;
    }
    Require(overflow && ec.references == std::numeric_limits<std::int32_t>::max());
    Require(ERR_get_error_nid_postfix() == 0);
    bool nullKey = false;
    try {
        EVP_PKEY_get1_EC_KEY_nid_postfix(nullptr);
    } catch (const std::invalid_argument&) {
        nullKey = true;
    }
    Require(nullKey);
}

void ConcurrentRetains() {
    LibreSslEcKey ec{};
    ec.references = 1;
    LibreSslEvpPkey key{.type = 408, .references = 3, .key = &ec, .saveParameters = 1};
    std::array<std::thread, 6> threads;
    for (auto& thread : threads) {
        thread = std::thread([&] {
            for (int repeat = 0; repeat < 1000; ++repeat)
                Require(EVP_PKEY_get1_EC_KEY_nid_postfix(&key) == &ec);
        });
    }
    for (auto& thread : threads) thread.join();
    Require(ec.references == 6001 && key.references == 3);
}

} // namespace

int main() {
    AccessAndErrors();
    ConcurrentRetains();
    std::puts("retained EC key checks passed");
}
