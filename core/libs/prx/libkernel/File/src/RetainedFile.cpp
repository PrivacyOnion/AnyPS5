#include "prx/libkernel/include/Api.hpp"
#include "prx/libc/include/Api.hpp"
#include "prx/libc/include/AppArchive.hpp"
#include "prx/libc/include/General.hpp"
#include "prx/libc/include/GuestArena.hpp"
#include "prx/libkernel/Equeue/Equeue.hpp"
#include "prx/libkernel/File/include/DescriptorControl.hpp"
#include "prx/libkernel/File/include/DirectoryDescriptor.hpp"
#include "prx/libkernel/File/include/File.hpp"
#include "prx/libkernel/File/include/RandomDevice.hpp"
#include "prx/libkernel/File/include/RetainedFile.hpp"
#include "prx/libkernel/File/include/SharedMemory.hpp"
#include "prx/libkernel/Socket/include/SocketRuntime.hpp"

#include <atomic>
#include <cerrno>
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <limits>
#include <memory>
#include <mutex>
#include <optional>
#include <shared_mutex>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <utility>

#ifdef _WIN32
#include <windows.h>
#include "prx/libc/include/WindowsDescriptor.hpp"
#include "prx/libkernel/File/include/WindowsFileTime.hpp"
#else
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/uio.h>
#include <unistd.h>
#endif

namespace {

constexpr int GuestEio = 5;
constexpr int GuestEbadf = 9;
constexpr int GuestEdeadlk = 11;
constexpr int GuestEfault = 14;
constexpr int GuestEisdir = 21;
constexpr int GuestEinval = 22;
constexpr int GuestEnospc = 28;
constexpr int GuestEspipe = 29;
constexpr int GuestEagain = 35;
constexpr int GuestEopnotsupp = 45;
constexpr int GuestEloop = 62;
constexpr int GuestEnametoolong = 63;
constexpr int GuestEnotempty = 66;
constexpr int GuestEdquot = 69;
constexpr int GuestEoverflow = 84;

constexpr int GuestOAppend = 0x0008;
constexpr int GuestOSync = 0x0080;

int GuestErrnoFromHost(int error) {
    int guest = error > 0 && error <= 34 ? error : GuestEio;
    if (error == EAGAIN) guest = GuestEagain;
#ifdef EDEADLK
    else if (error == EDEADLK) guest = GuestEdeadlk;
#endif
    else if (error == ENAMETOOLONG) guest = GuestEnametoolong;
    else if (error == ENOTEMPTY) guest = GuestEnotempty;
#ifdef ELOOP
    else if (error == ELOOP) guest = GuestEloop;
#endif
#ifdef EDQUOT
    else if (error == EDQUOT) guest = GuestEdquot;
#endif
#ifdef EOVERFLOW
    else if (error == EOVERFLOW) guest = GuestEoverflow;
#endif
#ifdef EOPNOTSUPP
    else if (error == EOPNOTSUPP) guest = GuestEopnotsupp;
#endif
    return guest;
}

std::int64_t FailGuest(int error) {
    *__error_nid_postfix() = error;
    return -1;
}

RetainedOpenFile* FailRetain(int error) {
    *__error_nid_postfix() = error;
    return nullptr;
}

std::recursive_mutex g_descriptorOwnershipMutex;
std::atomic<void (*)(int, int)> g_boundaryHook{nullptr};

}

namespace File {

std::unique_lock<std::recursive_mutex> LockDescriptorOwnership_nid_no_patch() {
    return std::unique_lock(g_descriptorOwnershipMutex);
}

void SetBoundaryHookForTest_nid_no_patch(void (*hook)(int, int)) {
    g_boundaryHook.store(hook, std::memory_order_release);
}

void InvokeBoundaryHook_nid_no_patch(int stage, int descriptor) {
    if (const auto hook = g_boundaryHook.load(std::memory_order_acquire)) hook(stage, descriptor);
}

}

namespace {

bool ExceedsTransferLimit(std::size_t bytes) {
    return bytes > static_cast<std::size_t>(std::numeric_limits<std::int64_t>::max());
}

struct RetainedState;

#ifndef _WIN32
constexpr int PrivateDescriptorBase = 4096;
std::unordered_map<int, std::weak_ptr<RetainedState>> g_privateDescriptors;
std::atomic<std::size_t> g_privateCount{0};

int AllocatePrivateDescriptor(int sourceFd, int avoidFd) {
    const int high = avoidFd >= PrivateDescriptorBase ? avoidFd + 1 : PrivateDescriptorBase;
    int fd = ::fcntl(sourceFd, F_DUPFD_CLOEXEC, high);
    if (fd >= 0 || (errno != EINVAL && errno != EMFILE)) return fd;
    const int above = avoidFd >= 3 ? avoidFd + 1 : 3;
    fd = ::fcntl(sourceFd, F_DUPFD_CLOEXEC, above);
    if (fd >= 0 || avoidFd <= 3 || (errno != EINVAL && errno != EMFILE)) return fd;
    return ::fcntl(sourceFd, F_DUPFD_CLOEXEC, 3);
}
#endif

struct RetainedState {
#ifdef _WIN32
    HANDLE handle = INVALID_HANDLE_VALUE;
    std::shared_ptr<std::mutex> positionedMutex;
    std::optional<std::filesystem::path> directoryPath;
    bool isSharedMemory = false;
    bool sharedMemoryWritable = false;
#else
    std::shared_mutex ioMutex;
    int hostFd = -1;
#endif
    std::shared_ptr<std::atomic<int>> statusFlags;
    std::optional<AppArchiveNode> archivedNode;
    bool isDirectory = false;
    bool isVirtualDirectory = false;

    void Close() {
#ifdef _WIN32
        if (handle == INVALID_HANDLE_VALUE) return;
        const HANDLE current = handle;
        handle = INVALID_HANDLE_VALUE;
        if (isSharedMemory) File::CloseSharedMemoryHandle_nid_no_patch(current);
        else ::CloseHandle(current);
#else
        std::unique_lock ioLock(ioMutex);
        const int fd = hostFd;
        if (fd < 0) return;
        hostFd = -1;
        {
            auto ownership = File::LockDescriptorOwnership_nid_no_patch();
            g_privateDescriptors.erase(fd);
            g_privateCount.store(g_privateDescriptors.size(), std::memory_order_release);
            ::close(fd);
        }
#endif
    }

    ~RetainedState() {
        const int savedHostErrno = errno;
        Close();
        errno = savedHostErrno;
    }
};

int SyncState(RetainedState& state) {
    if (state.isVirtualDirectory) return GuestEisdir;
    if (state.archivedNode) NotImplemented_nid_no_patch("fsync on a file read from app0.zar");
#ifdef _WIN32
    constexpr DWORD share = FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE;
    HANDLE handle = INVALID_HANDLE_VALUE;
    if (state.directoryPath) {
        handle = ::CreateFileW(state.directoryPath->c_str(), GENERIC_WRITE, share, nullptr, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, nullptr);
    } else {
        if (::FlushFileBuffers(state.handle)) return 0;
        if (::GetLastError() != ERROR_ACCESS_DENIED) {
            File::WindowsFileTime::Failure(::GetLastError());
            return GuestErrnoFromHost(errno);
        }
        handle = ::ReOpenFile(state.handle, GENERIC_WRITE, share, 0);
    }
    if (handle == INVALID_HANDLE_VALUE) {
        if (::GetLastError() == ERROR_ACCESS_DENIED) return 0;
        File::WindowsFileTime::Failure(::GetLastError());
        return GuestErrnoFromHost(errno);
    }
    const BOOL flushed = ::FlushFileBuffers(handle);
    const DWORD flushError = flushed ? ERROR_SUCCESS : ::GetLastError();
    ::CloseHandle(handle);
    if (flushed) return 0;
    File::WindowsFileTime::Failure(flushError);
    return GuestErrnoFromHost(errno);
#else
    std::shared_lock ioLock(state.ioMutex);
    if (state.hostFd < 0) return GuestEbadf;
    return ::fsync(state.hostFd) == 0 ? 0 : GuestErrnoFromHost(errno);
#endif
}

}

struct RetainedOpenFile {
    std::shared_ptr<RetainedState> state;
};

#ifndef _WIN32
namespace File {

bool IsPrivateDescriptor_nid_no_patch(int descriptor) {
    if (descriptor < 0 || g_privateCount.load(std::memory_order_acquire) == 0) return false;
    auto ownership = LockDescriptorOwnership_nid_no_patch();
    const auto found = g_privateDescriptors.find(descriptor);
    return found != g_privateDescriptors.end() && !found->second.expired();
}

bool EvacuatePrivateDescriptor_nid_no_patch(int descriptor) {
    if (descriptor < 0 || g_privateCount.load(std::memory_order_acquire) == 0) return true;
    for (;;) {
        std::shared_ptr<RetainedState> state;
        {
            auto ownership = LockDescriptorOwnership_nid_no_patch();
            const auto found = g_privateDescriptors.find(descriptor);
            if (found == g_privateDescriptors.end()) return true;
            state = found->second.lock();
            if (!state) {
                g_privateDescriptors.erase(found);
                g_privateCount.store(g_privateDescriptors.size(), std::memory_order_release);
                return true;
            }
        }
        std::unique_lock ioLock(state->ioMutex);
        auto ownership = LockDescriptorOwnership_nid_no_patch();
        if (state->hostFd != descriptor) continue;
        const int moved = AllocatePrivateDescriptor(descriptor, descriptor);
        if (moved < 0) return false;
        g_privateDescriptors.erase(descriptor);
        g_privateDescriptors[moved] = state;
        g_privateCount.store(g_privateDescriptors.size(), std::memory_order_release);
        state->hostFd = moved;
        ::close(descriptor);
        return true;
    }
}

bool EvacuatePrivateFrom_nid_no_patch(int start) {
    if (start < 0 || g_privateCount.load(std::memory_order_acquire) == 0) return true;
    auto ownership = LockDescriptorOwnership_nid_no_patch();
    for (int target = start; target >= 0; ++target) {
        if (IsPrivateDescriptor_nid_no_patch(target)) return EvacuatePrivateDescriptor_nid_no_patch(target);
        if (::fcntl(target, F_GETFD) < 0) return true;
    }
    return true;
}

}
#endif

extern "C" {

RetainedOpenFile* RetainOpenFile_nid_no_patch(int descriptor, bool requireRead, bool requireWrite) {
    if (descriptor < 0 || File::IsPrivateDescriptor_nid_no_patch(descriptor)) return FailRetain(GuestEbadf);
    if (GuestSockets::IsOpen(descriptor) || IsKqueueDescriptor_nid_no_patch(descriptor)) return FailRetain(GuestEbadf);
    if (File::IsRandomDevice(descriptor)) return FailRetain(GuestEopnotsupp);

    AppArchiveNode archiveNode{};
    if (AppArchiveDescriptor_nid_no_patch(descriptor, &archiveNode, nullptr)) {
        if (requireWrite) return FailRetain(GuestEbadf);
        auto state = std::make_shared<RetainedState>();
        state->archivedNode = archiveNode;
        state->isDirectory = archiveNode.Directory;
        return new RetainedOpenFile{std::move(state)};
    }

    if (File::IsVirtualDirectoryDescriptor(descriptor) && !File::IsNativeDirectoryDescriptor(descriptor)) {
        if (requireWrite) return FailRetain(GuestEbadf);
        auto state = std::make_shared<RetainedState>();
        state->isDirectory = true;
        state->isVirtualDirectory = true;
        return new RetainedOpenFile{std::move(state)};
    }

    auto state = std::make_shared<RetainedState>();
#ifdef _WIN32
    auto ownership = File::LockDescriptorOwnership_nid_no_patch();
    const auto positioned = File::LockPositionedTransfer(descriptor);
    const HANDLE source = WindowsDescriptorHandle_nid_no_patch(descriptor);
    if (source == INVALID_HANDLE_VALUE) return FailRetain(GuestEbadf);
    const bool isDirectory = File::IsDirectoryDescriptor(descriptor);
    const DWORD fileType = ::GetFileType(source);
    if (!isDirectory && fileType != FILE_TYPE_DISK) return FailRetain(fileType == FILE_TYPE_PIPE ? GuestEopnotsupp : GuestEbadf);
    if (isDirectory) {
        if (requireWrite) return FailRetain(GuestEbadf);
    } else {
        const int access = WindowsDescriptorAccess_nid_no_patch(source, descriptor, "RetainOpenFile");
        if (requireRead && access != 0 && access != 2) return FailRetain(GuestEbadf);
        if (requireWrite && access != 1 && access != 2) return FailRetain(GuestEbadf);
    }
    const int sharedWritable = File::SharedMemoryDescriptorWritable_nid_no_patch(descriptor);
    if (sharedWritable == 0 && requireWrite) return FailRetain(GuestEbadf);
    HANDLE duplicate = INVALID_HANDLE_VALUE;
    if (!::DuplicateHandle(::GetCurrentProcess(), source, ::GetCurrentProcess(), &duplicate, 0, FALSE, DUPLICATE_SAME_ACCESS))
        return FailRetain(GuestErrnoFromHost(WindowsOpenError_nid_no_patch(::GetLastError())));
    File::InvokeBoundaryHook_nid_no_patch(0, descriptor);
    state->handle = duplicate;
    state->positionedMutex = File::PositionedTransferMutex_nid_no_patch(descriptor);
    state->statusFlags = File::DescriptorStatus_nid_no_patch(descriptor);
    state->isDirectory = isDirectory;
    state->isVirtualDirectory = File::IsVirtualDirectoryDescriptor(descriptor);
    if (isDirectory) state->directoryPath = File::DirectoryDescriptorPath(descriptor);
    state->isSharedMemory = sharedWritable != -2;
    state->sharedMemoryWritable = sharedWritable == 1;
    return new RetainedOpenFile{std::move(state)};
#else
    int privateFd = -1;
    {
        auto ownership = File::LockDescriptorOwnership_nid_no_patch();
        privateFd = AllocatePrivateDescriptor(descriptor, -1);
        if (privateFd < 0) return FailRetain(GuestErrnoFromHost(errno));
        File::InvokeBoundaryHook_nid_no_patch(0, privateFd);
        const int flags = ::fcntl(privateFd, F_GETFL);
        if (flags < 0) {
            const int error = errno;
            ::close(privateFd);
            return FailRetain(GuestErrnoFromHost(error));
        }
        const int access = flags & O_ACCMODE;
        if ((requireRead && access != O_RDONLY && access != O_RDWR) ||
            (requireWrite && access != O_WRONLY && access != O_RDWR)) {
            ::close(privateFd);
            return FailRetain(GuestEbadf);
        }
        struct stat status{};
        if (::fstat(privateFd, &status) != 0) {
            const int error = errno;
            ::close(privateFd);
            return FailRetain(GuestErrnoFromHost(error));
        }
        const bool isDirectory = File::IsDirectoryDescriptor(descriptor) || S_ISDIR(status.st_mode);
        if (!isDirectory && !S_ISREG(status.st_mode)) {
            ::close(privateFd);
            return FailRetain(S_ISFIFO(status.st_mode) || S_ISCHR(status.st_mode) ? GuestEopnotsupp : GuestEbadf);
        }
        if (isDirectory && requireWrite) {
            ::close(privateFd);
            return FailRetain(GuestEbadf);
        }
        state->hostFd = privateFd;
        state->statusFlags = File::DescriptorStatus_nid_no_patch(descriptor);
        state->isDirectory = isDirectory;
        state->isVirtualDirectory = File::IsVirtualDirectoryDescriptor(descriptor);
        g_privateDescriptors[privateFd] = state;
        g_privateCount.store(g_privateDescriptors.size(), std::memory_order_release);
    }
    return new RetainedOpenFile{std::move(state)};
#endif
}

void ReleaseRetainedOpenFile_nid_no_patch(RetainedOpenFile* retained) {
    if (retained == nullptr) return;
    const int savedGuestErrno = *__error_nid_postfix();
    const int savedHostErrno = errno;
    if (retained->state) retained->state->Close();
    delete retained;
    errno = savedHostErrno;
    *__error_nid_postfix() = savedGuestErrno;
}

std::int64_t ReadRetainedOpenFile_nid_no_patch(RetainedOpenFile* retained, void* buffer, std::size_t bytes, std::int64_t offset) {
    if (retained == nullptr || !retained->state) return FailGuest(GuestEbadf);
    auto& state = *retained->state;
    if (state.isDirectory) return FailGuest(GuestEisdir);
    if (ExceedsTransferLimit(bytes) || offset < 0) return FailGuest(GuestEinval);
    if (buffer == nullptr && bytes != 0) return FailGuest(GuestEfault);
    const GuestArena::HostWrite destination(buffer, bytes);
    if (!destination.Open()) return FailGuest(GuestEfault);
    if (state.archivedNode)
        return static_cast<std::int64_t>(AppArchiveRead_nid_no_patch(state.archivedNode->Handle, static_cast<std::uint64_t>(offset), buffer, bytes));
#ifdef _WIN32
    if (bytes > static_cast<std::size_t>(std::numeric_limits<DWORD>::max()))
        throw std::runtime_error("ReadRetainedOpenFile: bytes exceeds platform limit");
    std::lock_guard positioned(*state.positionedMutex);
    LARGE_INTEGER position{};
    if (::GetFileType(state.handle) != FILE_TYPE_DISK || !::SetFilePointerEx(state.handle, LARGE_INTEGER{}, &position, FILE_CURRENT))
        return FailGuest(GuestEspipe);
    const auto at = static_cast<std::uint64_t>(offset);
    OVERLAPPED overlapped{};
    overlapped.Offset = static_cast<DWORD>(at);
    overlapped.OffsetHigh = static_cast<DWORD>(at >> 32u);
    char empty = 0;
    DWORD done = 0;
    const BOOL ok = ::ReadFile(state.handle, buffer == nullptr ? &empty : buffer, static_cast<DWORD>(bytes), &done, &overlapped);
    const DWORD error = ok ? ERROR_SUCCESS : ::GetLastError();
    if (!::SetFilePointerEx(state.handle, position, nullptr, FILE_BEGIN))
        throw std::runtime_error("ReadRetainedOpenFile: cannot restore the file offset");
    if (!ok) {
        if (error == ERROR_HANDLE_EOF) return 0;
        if (error == ERROR_NOACCESS || error == ERROR_INVALID_USER_BUFFER) return FailGuest(GuestEfault);
        return FailGuest(error == ERROR_ACCESS_DENIED ? GuestEbadf : GuestEio);
    }
    return static_cast<std::int64_t>(done);
#else
    std::shared_lock ioLock(state.ioMutex);
    if (state.hostFd < 0) return FailGuest(GuestEbadf);
    char empty = 0;
    const auto result = static_cast<std::int64_t>(::pread(state.hostFd, buffer == nullptr ? &empty : buffer, bytes, static_cast<off_t>(offset)));
    return result < 0 ? FailGuest(GuestErrnoFromHost(errno)) : result;
#endif
}

std::int64_t WriteRetainedOpenFile_nid_no_patch(RetainedOpenFile* retained, const void* buffer, std::size_t bytes, std::int64_t offset) {
    if (retained == nullptr || !retained->state) return FailGuest(GuestEbadf);
    auto& state = *retained->state;
    if (state.isVirtualDirectory) return FailGuest(GuestEisdir);
    if (state.isDirectory || state.archivedNode) return FailGuest(GuestEbadf);
    if (ExceedsTransferLimit(bytes) || offset < 0) return FailGuest(GuestEinval);
    if (buffer == nullptr && bytes != 0) return FailGuest(GuestEfault);
    GuestArena::GuestArenaPrepareHostRead_nid_no_patch(buffer, bytes);
    const int flags = state.statusFlags ? state.statusFlags->load(std::memory_order_acquire) : 0;
    const bool appends = (flags & GuestOAppend) != 0;
    const bool syncs = (flags & GuestOSync) != 0;
#ifdef _WIN32
    if (bytes > static_cast<std::size_t>(std::numeric_limits<DWORD>::max()))
        throw std::runtime_error("WriteRetainedOpenFile: bytes exceeds platform limit");
    std::int64_t written = 0;
    {
        std::lock_guard positioned(*state.positionedMutex);
        char empty = 0;
        const void* source = buffer == nullptr ? &empty : buffer;
        if (state.isSharedMemory) {
            written = File::PwriteSharedMemoryHandle_nid_no_patch(state.handle, state.sharedMemoryWritable, source, bytes, offset);
            if (written < 0) return FailGuest(GuestErrnoFromHost(errno));
        } else {
            LARGE_INTEGER position{};
            if (::GetFileType(state.handle) != FILE_TYPE_DISK || !::SetFilePointerEx(state.handle, LARGE_INTEGER{}, &position, FILE_CURRENT))
                return FailGuest(GuestEspipe);
            const auto at = appends ? ~std::uint64_t{} : static_cast<std::uint64_t>(offset);
            OVERLAPPED overlapped{};
            overlapped.Offset = static_cast<DWORD>(at);
            overlapped.OffsetHigh = static_cast<DWORD>(at >> 32u);
            DWORD done = 0;
            const BOOL ok = ::WriteFile(state.handle, source, static_cast<DWORD>(bytes), &done, &overlapped);
            const DWORD error = ok ? ERROR_SUCCESS : ::GetLastError();
            if (!::SetFilePointerEx(state.handle, position, nullptr, FILE_BEGIN))
                throw std::runtime_error("WriteRetainedOpenFile: cannot restore the file offset");
            if (!ok) {
                if (error == ERROR_NOACCESS || error == ERROR_INVALID_USER_BUFFER) return FailGuest(GuestEfault);
                if (error == ERROR_DISK_FULL || error == ERROR_HANDLE_DISK_FULL) return FailGuest(GuestEnospc);
                return FailGuest(error == ERROR_ACCESS_DENIED ? GuestEbadf : GuestEio);
            }
            written = static_cast<std::int64_t>(done);
        }
    }
    if (syncs && !state.isSharedMemory) {
        if (const int syncError = SyncState(state); syncError != 0)
            throw std::runtime_error("write: syncing an O_FSYNC file failed, error=" + std::to_string(syncError));
        File::RecordSynchronousWrite_nid_no_patch();
    }
    return written;
#else
    char empty = 0;
    const void* source = buffer == nullptr ? &empty : buffer;
    std::int64_t written = -2;
    if (appends) {
        std::unique_lock ioLock(state.ioMutex);
        if (state.hostFd < 0) return FailGuest(GuestEbadf);
        written = File::PwriteSharedMemory_nid_no_patch(state.hostFd, source, bytes, offset);
        if (written == -2) {
            struct stat status{};
            if (::fstat(state.hostFd, &status) == 0 && S_ISREG(status.st_mode)) {
                const off_t position = ::lseek(state.hostFd, 0, SEEK_CUR);
                if (position < 0) return FailGuest(GuestErrnoFromHost(errno));
                const iovec whole{const_cast<void*>(source), bytes};
                written = static_cast<std::int64_t>(::writev(state.hostFd, &whole, 1));
                const int savedError = errno;
                if (::lseek(state.hostFd, position, SEEK_SET) < 0)
                    throw std::runtime_error("WriteRetainedOpenFile: cannot restore the file offset");
                errno = savedError;
            } else {
                written = static_cast<std::int64_t>(::pwrite(state.hostFd, source, bytes, static_cast<off_t>(offset)));
            }
        }
        if (written < 0) return FailGuest(GuestErrnoFromHost(errno));
        if (syncs) {
            if (::fsync(state.hostFd) != 0)
                throw std::runtime_error("write: syncing an O_FSYNC file failed, error=" + std::to_string(GuestErrnoFromHost(errno)));
            File::RecordSynchronousWrite_nid_no_patch();
        }
        return written;
    }
    std::shared_lock ioLock(state.ioMutex);
    if (state.hostFd < 0) return FailGuest(GuestEbadf);
    const bool sharedMemory = File::IsSharedMemoryDescriptor_nid_no_patch(state.hostFd);
    written = File::PwriteSharedMemory_nid_no_patch(state.hostFd, source, bytes, offset);
    if (written == -2) written = static_cast<std::int64_t>(::pwrite(state.hostFd, source, bytes, static_cast<off_t>(offset)));
    if (written < 0) return FailGuest(GuestErrnoFromHost(errno));
    if (syncs && !sharedMemory) {
        if (::fsync(state.hostFd) != 0)
            throw std::runtime_error("write: syncing an O_FSYNC file failed, error=" + std::to_string(GuestErrnoFromHost(errno)));
        File::RecordSynchronousWrite_nid_no_patch();
    }
    return written;
#endif
}

int SyncRetainedOpenFile_nid_no_patch(RetainedOpenFile* retained) {
    if (retained == nullptr || !retained->state) return static_cast<int>(FailGuest(GuestEbadf));
    const int error = SyncState(*retained->state);
    return error == 0 ? 0 : static_cast<int>(FailGuest(error));
}

void RetainedFileSetBoundaryHookForTest_nid_no_patch(void (*hook)(int stage, int descriptor)) {
    File::SetBoundaryHookForTest_nid_no_patch(hook);
}

}
