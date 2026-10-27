# libSceFontGsm technical debt

### Unknown function info

`libSceFontGsm` is registered in `core/libs/prx/libSceSysmodule/ModuleTable.hpp` (`kModuleTable` ID `0x00000135` (and `0x0135` in `kPublicModuleIds`)). The 6 export(s) in [Export.cpp](../../../core/libs/prx/libSceFontGsm/Export.cpp) come from the per-library stub list in [claimore22/ps5rs](https://github.com/claimore22/ps5rs/blob/5f252a658ff470bcb2b6cd15205fd8ca64ae4477/data/stubs/by_library/libSceFontGsm.txt) (`data/stubs/by_library/libSceFontGsm.txt`) and the pinned [aerolib symbol database](https://github.com/zecoxao/sce_symbols/blob/2883963a0a514ba6e77407a08ef4730a65e88254/aerolib.csv) (`tools/nid_names.py` `compute_nid` equals the listed NID for every entry). That repository does not document how the list was extracted and the module is absent from the [PS5 3.20 export inventory](https://github.com/DNNDHH/PS5-3.20_Libs/tree/b2a77c09cd71c01f34c790cfbe98c751a13262ad), so completeness, parameter layouts, and behavior are unverified: every declaration takes no parameters in the import-resolution convention (no ABI claim) and throws through `NotImplemented_nid_no_patch`.

- [sceFontGraphicsAgcDrawupFillTextureImage](../../../core/libs/prx/libSceFontGsm/Export.cpp) (libSceFontGsm) - unknown signature (NID PwZEP8ZBK2Y)
- [sceFontGraphicsAgcDrawupFillTexturePattern](../../../core/libs/prx/libSceFontGsm/Export.cpp) (libSceFontGsm) - unknown signature (NID z-m061OoEqQ)
- [sceFontGraphicsAgcSurfaceInit](../../../core/libs/prx/libSceFontGsm/Export.cpp) (libSceFontGsm) - unknown signature (NID qEGeDXvL3fc)
- [sceFontGraphicsSequenceGetAgcCommandsForJump](../../../core/libs/prx/libSceFontGsm/Export.cpp) (libSceFontGsm) - unknown signature (NID 6zxuBlsGF0I)
- [sceFontGraphicsSequenceGetAgcCommandsForSubmit](../../../core/libs/prx/libSceFontGsm/Export.cpp) (libSceFontGsm) - unknown signature (NID CeSI7C4ihjo)
- [sceFontSelectGraphicsAgc](../../../core/libs/prx/libSceFontGsm/Export.cpp) (libSceFontGsm) - unknown signature (NID XKfh4naaANE)
