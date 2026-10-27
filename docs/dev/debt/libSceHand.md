# libSceHand technical debt

### Unknown function info

`libSceHand` is registered in `core/libs/prx/libSceSysmodule/ModuleTable.hpp` (`kModuleTable` ID `0x00000093`). The 3 exports in [Export.cpp](../../../core/libs/prx/libSceHand/Export.cpp) are the `libSceHand` entries corroborated by both [Orbital's IDA type database](https://github.com/AlexAltea/orbital/blob/cc408367da6a6ebbf5ac1ac559fefeadc73b2831/tools/ida/db_types.json) and the pinned [aerolib symbol database](https://github.com/zecoxao/sce_symbols/blob/2883963a0a514ba6e77407a08ef4730a65e88254/aerolib.csv) (`tools/nid_names.py` `compute_nid` matches each NID), excluding tracker functions which belong to `libSceHandTracker`. The module is absent from the [PS5 3.20 export inventory](https://github.com/DNNDHH/PS5-3.20_Libs/tree/b2a77c09cd71c01f34c790cfbe98c751a13262ad), so whether this list is complete on PS5 firmware and the PS5 parameter layouts and behavior are unverified: every declaration takes no parameters in the import-resolution convention (no ABI claim) and throws through `NotImplemented_nid_no_patch`.

- [sceHandDetectionGetDefaultParam](../../../core/libs/prx/libSceHand/Export.cpp) (libSceHand) - unknown signature (NID lsog-msx-Qk)
- [sceHandDetectionGetWorkingMemorySize](../../../core/libs/prx/libSceHand/Export.cpp) (libSceHand) - unknown signature (NID 9UVGP8KVB2U)
- [sceHandDetectionLocalGetDefaultParam](../../../core/libs/prx/libSceHand/Export.cpp) (libSceHand) - unknown signature (NID Zwj3yUNv-O4)
