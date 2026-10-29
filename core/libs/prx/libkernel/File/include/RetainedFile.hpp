#ifndef CORE_LIBS_PRX_LIBKERNEL_FILE_RETAINEDFILE_HPP
#define CORE_LIBS_PRX_LIBKERNEL_FILE_RETAINEDFILE_HPP

#include <cstddef>
#include <cstdint>

namespace File {

#ifdef _WIN32
inline bool IsPrivateDescriptor_nid_no_patch(int) { return false; }
inline bool EvacuatePrivateDescriptor_nid_no_patch(int) { return true; }
inline bool EvacuatePrivateFrom_nid_no_patch(int) { return true; }
#else
bool IsPrivateDescriptor_nid_no_patch(int descriptor);
bool EvacuatePrivateDescriptor_nid_no_patch(int descriptor);
bool EvacuatePrivateFrom_nid_no_patch(int start);
#endif

}

extern "C" {

struct RetainedOpenFile;

RetainedOpenFile* RetainOpenFile_nid_no_patch(int descriptor, bool requireRead, bool requireWrite);
void ReleaseRetainedOpenFile_nid_no_patch(RetainedOpenFile* retained);
std::int64_t ReadRetainedOpenFile_nid_no_patch(RetainedOpenFile* retained, void* buffer, std::size_t bytes, std::int64_t offset);
std::int64_t WriteRetainedOpenFile_nid_no_patch(RetainedOpenFile* retained, const void* buffer, std::size_t bytes, std::int64_t offset);
int SyncRetainedOpenFile_nid_no_patch(RetainedOpenFile* retained);

}

#endif
