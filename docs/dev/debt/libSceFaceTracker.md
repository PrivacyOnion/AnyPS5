# libSceFaceTracker technical debt

### Unknown function info

`libSceFaceTracker` is registered in `core/libs/prx/libSceSysmodule/ModuleTable.hpp` (`kModuleTable` ID `0x000000d8` (and `0x00d8` in `kPublicModuleIds`)). The 21 exports in [Export.cpp](../../../core/libs/prx/libSceFaceTracker/Export.cpp) are the `libSceFaceTracker` entries corroborated by both [Orbital's IDA type database](https://github.com/AlexAltea/orbital/blob/cc408367da6a6ebbf5ac1ac559fefeadc73b2831/tools/ida/db_types.json) and the pinned [aerolib symbol database](https://github.com/zecoxao/sce_symbols/blob/2883963a0a514ba6e77407a08ef4730a65e88254/aerolib.csv) (`tools/nid_names.py` `compute_nid` matches each NID). The module is absent from the [PS5 3.20 export inventory](https://github.com/DNNDHH/PS5-3.20_Libs/tree/b2a77c09cd71c01f34c790cfbe98c751a13262ad), so whether this list is complete on PS5 firmware and the PS5 parameter layouts and behavior are unverified: every declaration takes no parameters in the import-resolution convention (no ABI claim) and throws through `NotImplemented_nid_no_patch`.

- [sceFaceTrackerAddUserFeature](../../../core/libs/prx/libSceFaceTracker/Export.cpp) (libSceFaceTracker) - unknown signature (NID XuqmYJfdEzI)
- [sceFaceTrackerFinalize](../../../core/libs/prx/libSceFaceTracker/Export.cpp) (libSceFaceTracker) - unknown signature (NID kgoY4sueNqU)
- [sceFaceTrackerGetDefaultConfig](../../../core/libs/prx/libSceFaceTracker/Export.cpp) (libSceFaceTracker) - unknown signature (NID N9K6D-eO6UI)
- [sceFaceTrackerGetResult](../../../core/libs/prx/libSceFaceTracker/Export.cpp) (libSceFaceTracker) - unknown signature (NID IV-ddS1D+U0)
- [sceFaceTrackerGetTrackIdByUserId](../../../core/libs/prx/libSceFaceTracker/Export.cpp) (libSceFaceTracker) - unknown signature (NID GJQTuIKPCLM)
- [sceFaceTrackerGetUserFeature](../../../core/libs/prx/libSceFaceTracker/Export.cpp) (libSceFaceTracker) - unknown signature (NID Ls+C7--QDuo)
- [sceFaceTrackerGetUserIdByTrackId](../../../core/libs/prx/libSceFaceTracker/Export.cpp) (libSceFaceTracker) - unknown signature (NID wG6+Pqh1prQ)
- [sceFaceTrackerGetWorkingMemorySize](../../../core/libs/prx/libSceFaceTracker/Export.cpp) (libSceFaceTracker) - unknown signature (NID zN4908PoI3k)
- [sceFaceTrackerInitialize](../../../core/libs/prx/libSceFaceTracker/Export.cpp) (libSceFaceTracker) - unknown signature (NID SknJO3Bt+-o)
- [sceFaceTrackerRegisterFixUserIdCallback](../../../core/libs/prx/libSceFaceTracker/Export.cpp) (libSceFaceTracker) - unknown signature (NID glkVxMoEu-E)
- [sceFaceTrackerRegisterLostTargetCallback](../../../core/libs/prx/libSceFaceTracker/Export.cpp) (libSceFaceTracker) - unknown signature (NID knPtjtHAlek)
- [sceFaceTrackerRegisterRecoveryFromLostTargetCallback](../../../core/libs/prx/libSceFaceTracker/Export.cpp) (libSceFaceTracker) - unknown signature (NID 85RI6pMiI6c)
- [sceFaceTrackerRegisterStartTrackingCallback](../../../core/libs/prx/libSceFaceTracker/Export.cpp) (libSceFaceTracker) - unknown signature (NID amMmIL1kj-k)
- [sceFaceTrackerRegisterStopTrackingCallback](../../../core/libs/prx/libSceFaceTracker/Export.cpp) (libSceFaceTracker) - unknown signature (NID YPqj8Ky3WDY)
- [sceFaceTrackerRegisterUser](../../../core/libs/prx/libSceFaceTracker/Export.cpp) (libSceFaceTracker) - unknown signature (NID VsVD7+SYlNc)
- [sceFaceTrackerReset](../../../core/libs/prx/libSceFaceTracker/Export.cpp) (libSceFaceTracker) - unknown signature (NID i0DbE5x06fw)
- [sceFaceTrackerSearchTarget](../../../core/libs/prx/libSceFaceTracker/Export.cpp) (libSceFaceTracker) - unknown signature (NID vnp7yntvlPY)
- [sceFaceTrackerStartTracking](../../../core/libs/prx/libSceFaceTracker/Export.cpp) (libSceFaceTracker) - unknown signature (NID bAKsVCOJMu8)
- [sceFaceTrackerStopTracking](../../../core/libs/prx/libSceFaceTracker/Export.cpp) (libSceFaceTracker) - unknown signature (NID Tb-iOd+G2Zs)
- [sceFaceTrackerUnregisterUser](../../../core/libs/prx/libSceFaceTracker/Export.cpp) (libSceFaceTracker) - unknown signature (NID CAIoDnCFX70)
- [sceFaceTrackerUpdate](../../../core/libs/prx/libSceFaceTracker/Export.cpp) (libSceFaceTracker) - unknown signature (NID ZHWeGLW4X34)
