# libSceCdlgPlayerReview technical debt

### Unknown function info

- [scePlayerReviewDialogOpen](../../../core/libs/prx/libSceCdlgPlayerReview/Export.cpp) uses a recovered 168-byte parameter layout; the 40-byte target record at byte 60 and mode semantics remain unresolved.

### Functional

- [Player review dialog](../../../core/libs/prx/libSceCdlgPlayerReview/Export.cpp) (libSceCdlgPlayerReview) shows nothing: `scePlayerReviewDialogOpen` finishes at once and the result reports that the user canceled, as `sceWebBrowserDialogOpen` does
- [Dialog lifecycle](../../../core/libs/prx/libSceCdlgPlayerReview/Export.cpp) (libSceCdlgPlayerReview) - dialog lifecycle after `FINISHED` is not measured on a console.
