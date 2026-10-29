# libScePlayerSelectionDialog technical debt

### Silent stubs

- [libScePlayerSelectionDialog](../../../core/libs/prx/libScePlayerSelectionDialog/Export.cpp) shows no selection UI and immediately reports cancellation. Account discovery, selection and successful-result list delivery are not implemented.

### Unknown function info

- [scePlayerSelectionDialogParamInitialize](../../../core/libs/prx/libScePlayerSelectionDialog/Export.cpp) (libScePlayerSelectionDialog) - zeroes the parameter, sets the common dialog base size and address-derived magic, and sets the public size. The console defaults for caller-owned fields and the exact behavior when reinitializing a populated parameter remain unverified.
- [scePlayerSelectionDialogOpen](../../../core/libs/prx/libScePlayerSelectionDialog/Export.cpp) (libScePlayerSelectionDialog) - checks the common dialog base parameter before the not-initialized state, then rejects a `userId` that is not a logged-in user and a `maxPlayers` or `playerCount` over 256. That order and those limits are what this implementation does; they are not measured on a console, so the guest tests that check them describe this implementation, not a console contract.
