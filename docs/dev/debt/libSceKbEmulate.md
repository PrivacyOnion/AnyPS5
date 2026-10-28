# libSceKbEmulate technical debt

### Unknown function info

- [sceKeyboardPadEmulateClose, sceKeyboardPadEmulateOpen](../../../core/libs/prx/libSceKbEmulate/Export.cpp) (libSceKbEmulate) - names and NIDs (`mFR2QN8HNVU`, `feMFMNgm+o0`) are present in the [PS5 3.20 export inventory](https://github.com/DNNDHH/PS5-3.20_Libs/blob/b2a77c09cd71c01f34c790cfbe98c751a13262ad/libSceKbEmulate.c). The prototype parameters (`int32_t userId, const void* param` for open and `int32_t handle` for close) and parameter structure layout remain unverified on hardware; open tracks integer handles when param is null and throws when non-null.
