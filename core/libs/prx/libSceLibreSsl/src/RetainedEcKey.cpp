#include "prx/libSceLibreSsl/include/Api.hpp"
#include "prx/libSceLibreSsl/include/Error.hpp"
#include "prx/libSceLibreSsl/include/Ec.hpp"
#include <stdexcept>

extern "C" EcKey* APS5_VABI EVP_PKEY_get1_EC_KEY_nid_postfix(LibreSslEvpPkey* key) {
    if (key == nullptr) throw std::invalid_argument("EVP_PKEY_get1_EC_KEY: null key");
    if (key->type != 408) {
        LibreSslPutError_nid_no_patch(6, 0xfff, 142);
        return nullptr;
    }
    return EcKeyRetain_nid_no_patch(static_cast<EcKey*>(key->key));
}
