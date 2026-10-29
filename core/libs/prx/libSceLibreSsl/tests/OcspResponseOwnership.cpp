#include "prx/libSceLibreSsl/libSceLibreSsl.hpp"
#include "prx/libc/include/ApplicationHeap.hpp"
#include <algorithm>
#include <array>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <source_location>
#include <stdexcept>
#include <vector>

namespace {

struct Allocation {
    void* pointer;
    std::size_t size;
    bool wiped;
    bool freed = false;
};

std::vector<Allocation> allocations;
std::vector<void*> released;

void Require(bool condition, std::source_location location = std::source_location::current()) {
    if (condition) return;
    std::fprintf(stderr, "check failed at line %u\n", location.line());
    std::abort();
}

void* Allocate(std::size_t size, bool wiped = false) {
    void* pointer = std::malloc(size);
    Require(pointer != nullptr);
    std::memset(pointer, 0xa5, size);
    allocations.push_back({pointer, size, wiped});
    return pointer;
}

template <class T>
T* Make() {
    auto* value = static_cast<T*>(Allocate(sizeof(T)));
    *value = {};
    return value;
}

Asn1String* MakeString() {
    auto* value = Make<Asn1String>();
    value->length = 3;
    value->data = static_cast<unsigned char*>(Allocate(3, true));
    return value;
}

void APS5_VABI Release_nid_no_patch(void* pointer) {
    if (pointer == nullptr) return;
    const auto found = std::find_if(allocations.begin(), allocations.end(), [pointer](const auto& allocation) {
        return allocation.pointer == pointer && !allocation.freed;
    });
    Require(found != allocations.end());
    if (found->wiped) {
        const auto* bytes = static_cast<const unsigned char*>(pointer);
        Require(std::all_of(bytes, bytes + found->size, [](unsigned char byte) { return byte == 0; }));
    }
    found->freed = true;
    released.push_back(pointer);
    std::free(pointer);
}

void* APS5_VABI Allocate_nid_no_patch(std::size_t) {
    throw std::runtime_error("OCSP release unexpectedly allocated");
}

void* APS5_VABI Calloc_nid_no_patch(std::size_t, std::size_t) { throw std::runtime_error("unexpected calloc"); }
void* APS5_VABI Reallocate_nid_no_patch(void*, std::size_t) { throw std::runtime_error("unexpected realloc"); }
void* APS5_VABI Align_nid_no_patch(std::size_t, std::size_t) { throw std::runtime_error("unexpected aligned allocation"); }
void* APS5_VABI Realign_nid_no_patch(void*, std::size_t, std::size_t) { throw std::runtime_error("unexpected realignment"); }
int APS5_VABI PosixAlign_nid_no_patch(void**, std::size_t, std::size_t) { throw std::runtime_error("unexpected posix_memalign"); }

void Reset() {
    Require(std::all_of(allocations.begin(), allocations.end(), [](const auto& allocation) { return allocation.freed; }));
    allocations.clear();
    released.clear();
}

void EmptyAndStatus() {
    OCSP_RESPONSE_free_nid_postfix(nullptr);
    auto* empty = Make<OcspResponse>();
    OCSP_RESPONSE_free_nid_postfix(empty);
    Require(released == std::vector<void*>{empty});
    Reset();
    auto* response = Make<OcspResponse>();
    auto* status = response->responseStatus = MakeString();
    std::vector<void*> expected{status->data, status, response};
    OCSP_RESPONSE_free_nid_postfix(response);
    Require(released == expected);
    Reset();
}

void EntireGraph() {
    auto* response = Make<OcspResponse>();
    auto* status = response->responseStatus = MakeString();
    auto* bytes = response->responseBytes = Make<OcspResponseBytes>();
    auto* body = bytes->response = MakeString();
    auto* oid = bytes->responseType = Make<LibreSslAsn1Object>();
    oid->flags = 1 | 4 | 8;
    oid->shortName = static_cast<char*>(Allocate(5));
    oid->longName = static_cast<char*>(Allocate(9));
    oid->length = 4;
    oid->data = static_cast<unsigned char*>(Allocate(4, true));
    std::vector<void*> expected{body->data, body, const_cast<char*>(oid->shortName), const_cast<char*>(oid->longName),
        const_cast<unsigned char*>(oid->data), oid, bytes, status->data, status, response};
    OCSP_RESPONSE_free_nid_postfix(response);
    Require(released == expected);
    Reset();
}

void BorrowedAndStatic() {
    unsigned char borrowed[] = {1, 2, 3};
    LibreSslAsn1Object oid{"short", "long", 7, -3, borrowed, 0};
    auto* response = Make<OcspResponse>();
    auto* bytes = response->responseBytes = Make<OcspResponseBytes>();
    auto* body = bytes->response = Make<Asn1String>();
    body->data = borrowed;
    body->length = -3;
    body->flags = 16;
    bytes->responseType = &oid;
    OCSP_RESPONSE_free_nid_postfix(response);
    Require(released == std::vector<void*>({body, bytes, response}));
    Require(borrowed[0] == 1 && borrowed[1] == 2 && borrowed[2] == 3);
    Require(oid.data == borrowed && oid.length == -3 && oid.flags == 0);
    Reset();
    response = Make<OcspResponse>();
    bytes = response->responseBytes = Make<OcspResponseBytes>();
    oid = {};
    oid.flags = 4 | 8;
    oid.shortName = static_cast<char*>(Allocate(4));
    oid.longName = static_cast<char*>(Allocate(6));
    oid.data = static_cast<unsigned char*>(Allocate(2, true));
    oid.length = 2;
    bytes->responseType = &oid;
    OCSP_RESPONSE_free_nid_postfix(response);
    Require(oid.shortName == nullptr && oid.longName == nullptr && oid.data == nullptr && oid.length == 0);
    Reset();
}

void InvalidLengthPreservesGraph() {
    auto* response = Make<OcspResponse>();
    auto* status = response->responseStatus = MakeString();
    auto* bytes = response->responseBytes = Make<OcspResponseBytes>();
    bytes->response = MakeString();
    status->length = -1;
    bool rejected = false;
    try {
        OCSP_RESPONSE_free_nid_postfix(response);
    } catch (const std::invalid_argument&) {
        rejected = true;
    }
    Require(rejected && released.empty());
    status->length = 3;
    OCSP_RESPONSE_free_nid_postfix(response);
    Reset();
}

} // namespace

int main() {
    std::array<void*, 10> api{reinterpret_cast<void*>(Allocate_nid_no_patch), reinterpret_cast<void*>(Release_nid_no_patch),
        reinterpret_cast<void*>(Calloc_nid_no_patch), reinterpret_cast<void*>(Reallocate_nid_no_patch),
        reinterpret_cast<void*>(Align_nid_no_patch), reinterpret_cast<void*>(Realign_nid_no_patch), reinterpret_cast<void*>(PosixAlign_nid_no_patch)};
    ApplicationHeapRegister_nid_no_patch(api.data());
    EmptyAndStatus();
    EntireGraph();
    BorrowedAndStatic();
    InvalidLengthPreservesGraph();
    std::puts("OCSP response ownership checks passed");
}
