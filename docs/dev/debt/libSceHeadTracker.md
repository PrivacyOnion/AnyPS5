# libSceHeadTracker technical debt

### Unknown function info

`libSceHeadTracker` is registered in `core/libs/prx/libSceSysmodule/ModuleTable.hpp` (`kModuleTable` ID `0x000000cb`). The 8 exports in [Export.cpp](../../../core/libs/prx/libSceHeadTracker/Export.cpp) are the `libSceHeadTracker` entries corroborated by both [Orbital's IDA type database](https://github.com/AlexAltea/orbital/blob/cc408367da6a6ebbf5ac1ac559fefeadc73b2831/tools/ida/db_types.json) and the pinned [aerolib symbol database](https://github.com/zecoxao/sce_symbols/blob/2883963a0a514ba6e77407a08ef4730a65e88254/aerolib.csv) (`tools/nid_names.py` `compute_nid` matches each NID). The module is absent from the [PS5 3.20 export inventory](https://github.com/DNNDHH/PS5-3.20_Libs/tree/b2a77c09cd71c01f34c790cfbe98c751a13262ad), so whether this list is complete on PS5 firmware and the PS5 parameter layouts and behavior are unverified: every declaration takes no parameters in the import-resolution convention (no ABI claim) and throws through `NotImplemented_nid_no_patch`.

- [sceHeadTrackerFinalize](../../../core/libs/prx/libSceHeadTracker/Export.cpp) (libSceHeadTracker) - unknown signature (NID 845enllqZOA)
- [sceHeadTrackerGetNumberOfRunningInstance](../../../core/libs/prx/libSceHeadTracker/Export.cpp) (libSceHeadTracker) - unknown signature (NID zNnPUDTRrMw)
- [sceHeadTrackerInitialize](../../../core/libs/prx/libSceHeadTracker/Export.cpp) (libSceHeadTracker) - unknown signature (NID N-wvaQnlXb0)
- [sceHeadTrackerQueryWorkingMemory](../../../core/libs/prx/libSceHeadTracker/Export.cpp) (libSceHeadTracker) - unknown signature (NID 22nfK8hJRj0)
- [sceHeadTrackerSearchCandidate](../../../core/libs/prx/libSceHeadTracker/Export.cpp) (libSceHeadTracker) - unknown signature (NID uogRLNVSArA)
- [sceHeadTrackerStart](../../../core/libs/prx/libSceHeadTracker/Export.cpp) (libSceHeadTracker) - unknown signature (NID Vft6N0EvQsw)
- [sceHeadTrackerStop](../../../core/libs/prx/libSceHeadTracker/Export.cpp) (libSceHeadTracker) - unknown signature (NID TaXlTcqvxNI)
- [sceHeadTrackerUpdate](../../../core/libs/prx/libSceHeadTracker/Export.cpp) (libSceHeadTracker) - unknown signature (NID P04ZezDSgk0)
