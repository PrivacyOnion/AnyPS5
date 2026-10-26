# libSceCdlgPlayerReview technical debt

### Silent stubs

- [scePlayerReviewDialogGetStatus and scePlayerReviewDialogUpdateStatus](../../../core/libs/prx/libSceCdlgPlayerReview/Export.cpp) return internal status values without driving player review dialog presentation.

### Unknown function info

- [scePlayerReviewDialogOpen](../../../core/libs/prx/libSceCdlgPlayerReview/Export.cpp) uses a recovered 168-byte parameter layout; the 40-byte target record at byte 60 and mode semantics remain unresolved.

### Functional

- [Player review dialog](../../../core/libs/prx/libSceCdlgPlayerReview/Export.cpp) (libSceCdlgPlayerReview) shows nothing: `scePlayerReviewDialogOpen` finishes at once and the result reports that the user canceled, as `sceWebBrowserDialogOpen` does
