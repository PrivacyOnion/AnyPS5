# libSceDeci4h technical debt

### Unknown function info

`libSceDeci4h` is registered in `core/libs/prx/libSceSysmodule/ModuleTable.hpp` (`kModuleTable` ID `0x000000ca`). The 7 exports in [Export.cpp](../../../core/libs/prx/libSceDeci4h/Export.cpp) are the `libSceDeci4h` entries corroborated by both [Orbital's IDA type database](https://github.com/AlexAltea/orbital/blob/cc408367da6a6ebbf5ac1ac559fefeadc73b2831/tools/ida/db_types.json) and the pinned [aerolib symbol database](https://github.com/zecoxao/sce_symbols/blob/2883963a0a514ba6e77407a08ef4730a65e88254/aerolib.csv) (`tools/nid_names.py` `compute_nid` matches each NID). The module is absent from the [PS5 3.20 export inventory](https://github.com/DNNDHH/PS5-3.20_Libs/tree/b2a77c09cd71c01f34c790cfbe98c751a13262ad), so whether this list is complete on PS5 firmware and the PS5 parameter layouts and behavior are unverified: every declaration takes no parameters in the import-resolution convention (no ABI claim) and throws through `NotImplemented_nid_no_patch`.

- [sceDeci4hClose](../../../core/libs/prx/libSceDeci4h/Export.cpp) (libSceDeci4h) - unknown signature (NID 3nQRdDyLMzE)
- [sceDeci4hCreateHostProcess](../../../core/libs/prx/libSceDeci4h/Export.cpp) (libSceDeci4h) - unknown signature (NID g7kM+dtQB0o)
- [sceDeci4hCreateHostProcessAndWait](../../../core/libs/prx/libSceDeci4h/Export.cpp) (libSceDeci4h) - unknown signature (NID BQqV4AR38B8)
- [sceDeci4hGetStatus](../../../core/libs/prx/libSceDeci4h/Export.cpp) (libSceDeci4h) - unknown signature (NID IGwoR-bZ-aQ)
- [sceDeci4hOpen](../../../core/libs/prx/libSceDeci4h/Export.cpp) (libSceDeci4h) - unknown signature (NID q2eqZBGasE0)
- [sceDeci4hRead](../../../core/libs/prx/libSceDeci4h/Export.cpp) (libSceDeci4h) - unknown signature (NID 5zcQehpL1Mc)
- [sceDeci4hWrite](../../../core/libs/prx/libSceDeci4h/Export.cpp) (libSceDeci4h) - unknown signature (NID swrsxmobY1c)
