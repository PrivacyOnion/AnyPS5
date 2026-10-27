# libSceFace technical debt

### Unknown function info

`libSceFace` is registered in `core/libs/prx/libSceSysmodule/ModuleTable.hpp` (`kModuleTable` ID `0x00000038` (and `0x0038` in `kPublicModuleIds`)). The 12 exports in [Export.cpp](../../../core/libs/prx/libSceFace/Export.cpp) are the `libSceFace` entries corroborated by both [Orbital's IDA type database](https://github.com/AlexAltea/orbital/blob/cc408367da6a6ebbf5ac1ac559fefeadc73b2831/tools/ida/db_types.json) and the pinned [aerolib symbol database](https://github.com/zecoxao/sce_symbols/blob/2883963a0a514ba6e77407a08ef4730a65e88254/aerolib.csv) (`tools/nid_names.py` `compute_nid` matches each NID), excluding face tracking functions which belong to `libSceFaceTracker`. The module is absent from the [PS5 3.20 export inventory](https://github.com/DNNDHH/PS5-3.20_Libs/tree/b2a77c09cd71c01f34c790cfbe98c751a13262ad), so whether this list is complete on PS5 firmware and the PS5 parameter layouts and behavior are unverified: every declaration takes no parameters in the import-resolution convention (no ABI claim) and throws through `NotImplemented_nid_no_patch`.

- [sceFaceAgeGetWorkingMemorySize](../../../core/libs/prx/libSceFace/Export.cpp) (libSceFace) - unknown signature (NID EMiyK-GRjA8)
- [sceFaceAgeRangeIntegrate](../../../core/libs/prx/libSceFace/Export.cpp) (libSceFace) - unknown signature (NID mrF0OjeuOgw)
- [sceFaceAllPartsGetWorkingMemorySize](../../../core/libs/prx/libSceFace/Export.cpp) (libSceFace) - unknown signature (NID Go4qB43MgX8)
- [sceFaceAttributeGetWorkingMemorySize](../../../core/libs/prx/libSceFace/Export.cpp) (libSceFace) - unknown signature (NID FTnplTtBnw8)
- [sceFaceDetectionGetDefaultParam](../../../core/libs/prx/libSceFace/Export.cpp) (libSceFace) - unknown signature (NID ZyC0HzaV6VU)
- [sceFaceDetectionGetWorkingMemorySize](../../../core/libs/prx/libSceFace/Export.cpp) (libSceFace) - unknown signature (NID DYdXC4Ixa4s)
- [sceFaceIdentifyExGetWorkingMemorySize](../../../core/libs/prx/libSceFace/Export.cpp) (libSceFace) - unknown signature (NID kZ5Cc0KqzdI)
- [sceFaceIdentifyGetWorkingMemorySize](../../../core/libs/prx/libSceFace/Export.cpp) (libSceFace) - unknown signature (NID jCSXa2n1cXI)
- [sceFaceIdentifyLiteGetWorkingMemorySize](../../../core/libs/prx/libSceFace/Export.cpp) (libSceFace) - unknown signature (NID pk4p3CEJflE)
- [sceFacePartsGetWorkingMemorySize](../../../core/libs/prx/libSceFace/Export.cpp) (libSceFace) - unknown signature (NID uiyG1M9D8-4)
- [sceFaceShapeGetWorkingMemorySize](../../../core/libs/prx/libSceFace/Export.cpp) (libSceFace) - unknown signature (NID BsmV0JLSdpg)
- [sceFaceShapeTrack](../../../core/libs/prx/libSceFace/Export.cpp) (libSceFace) - unknown signature (NID 47S+J03eask)
