#pragma once
#include "../../dokan/dokan.h"
#include "../../dokan/fileinfo.h"
#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <winbase.h>
#include <functional>

#ifndef DOKAN_MAX_PATH
//#define WIN10_ENABLE_LONG_PATH
#ifdef WIN10_ENABLE_LONG_PATH
//dirty but should be enough
#define DOKAN_MAX_PATH 32768
#else
#define DOKAN_MAX_PATH MAX_PATH
#endif // DEBUG
#endif // !DOKAN_MAX_PATH

static __declspec(thread) int RootIndex = 0;
#define MAX_ROOT_COUNT 2
static int RootCounts = 0;
static WCHAR RootDirectories[MAX_ROOT_COUNT][DOKAN_MAX_PATH];
static NTSTATUS DOKAN_CALLBACK
MirrorCreateFile(LPCWSTR FileName, PDOKAN_IO_SECURITY_CONTEXT SecurityContext,
                 ACCESS_MASK DesiredAccess, ULONG FileAttributes,
                 ULONG ShareAccess, ULONG CreateDisposition,
                 ULONG CreateOptions, PDOKAN_FILE_INFO DokanFileInfo);

static NTSTATUS DOKAN_CALLBACK MirrorReadFile(LPCWSTR FileName, LPVOID Buffer,
                                              DWORD BufferLength,
                                              LPDWORD ReadLength,
                                              LONGLONG Offset,
                                              PDOKAN_FILE_INFO DokanFileInfo);

static NTSTATUS DOKAN_CALLBACK MirrorWriteFile(LPCWSTR FileName, LPCVOID Buffer,
                                               DWORD NumberOfBytesToWrite,
                                               LPDWORD NumberOfBytesWritten,
                                               LONGLONG Offset,
                                               PDOKAN_FILE_INFO DokanFileInfo);
static NTSTATUS DOKAN_CALLBACK
MirrorFlushFileBuffers(LPCWSTR FileName, PDOKAN_FILE_INFO DokanFileInfo);

static NTSTATUS DOKAN_CALLBACK MirrorGetFileInformation(
    LPCWSTR FileName, LPBY_HANDLE_FILE_INFORMATION HandleFileInformation,
    PDOKAN_FILE_INFO DokanFileInfo);

static NTSTATUS DOKAN_CALLBACK
MirrorFindFiles(LPCWSTR FileName,
                PFillFindData FillFindData, // function pointer
                PDOKAN_FILE_INFO DokanFileInfo);

static NTSTATUS DOKAN_CALLBACK MirrorDeleteFile(LPCWSTR FileName,
                                                PDOKAN_FILE_INFO DokanFileInfo);

static NTSTATUS DOKAN_CALLBACK
MirrorDeleteDirectory(LPCWSTR FileName, PDOKAN_FILE_INFO DokanFileInfo);
static NTSTATUS DOKAN_CALLBACK
MirrorMoveFile(LPCWSTR FileName, // existing file name
               LPCWSTR NewFileName, BOOL ReplaceIfExisting,
               PDOKAN_FILE_INFO DokanFileInfo);

static NTSTATUS DOKAN_CALLBACK MirrorLockFile(LPCWSTR FileName,
                                              LONGLONG ByteOffset,
                                              LONGLONG Length,
                                              PDOKAN_FILE_INFO DokanFileInfo);

static NTSTATUS DOKAN_CALLBACK MirrorSetEndOfFile(
    LPCWSTR FileName, LONGLONG ByteOffset, PDOKAN_FILE_INFO DokanFileInfo);

static NTSTATUS DOKAN_CALLBACK MirrorSetAllocationSize(
    LPCWSTR FileName, LONGLONG AllocSize, PDOKAN_FILE_INFO DokanFileInfo);

static NTSTATUS DOKAN_CALLBACK MirrorSetFileAttributes(
    LPCWSTR FileName, DWORD FileAttributes, PDOKAN_FILE_INFO DokanFileInfo);

static NTSTATUS DOKAN_CALLBACK
MirrorSetFileTime(LPCWSTR FileName, CONST FILETIME *CreationTime,
                  CONST FILETIME *LastAccessTime, CONST FILETIME *LastWriteTime,
                  PDOKAN_FILE_INFO DokanFileInfo);

static NTSTATUS DOKAN_CALLBACK MirrorUnlockFile(LPCWSTR FileName,
                                                LONGLONG ByteOffset,
                                                LONGLONG Length,
                                                PDOKAN_FILE_INFO DokanFileInfo);
static NTSTATUS DOKAN_CALLBACK MirrorGetFileSecurity(
    LPCWSTR FileName, PSECURITY_INFORMATION SecurityInformation,
    PSECURITY_DESCRIPTOR SecurityDescriptor, ULONG BufferLength,
    PULONG LengthNeeded, PDOKAN_FILE_INFO DokanFileInfo);
static NTSTATUS DOKAN_CALLBACK MirrorSetFileSecurity(
    LPCWSTR FileName, PSECURITY_INFORMATION SecurityInformation,
    PSECURITY_DESCRIPTOR SecurityDescriptor, ULONG SecurityDescriptorLength,
    PDOKAN_FILE_INFO DokanFileInfo);

static NTSTATUS DOKAN_CALLBACK MirrorGetVolumeInformation(
    LPWSTR VolumeNameBuffer, DWORD VolumeNameSize, LPDWORD VolumeSerialNumber,
    LPDWORD MaximumComponentLength, LPDWORD FileSystemFlags,
    LPWSTR FileSystemNameBuffer, DWORD FileSystemNameSize,
    PDOKAN_FILE_INFO DokanFileInfo);

static NTSTATUS DOKAN_CALLBACK MirrorDokanGetDiskFreeSpace(
    PULONGLONG FreeBytesAvailable, PULONGLONG TotalNumberOfBytes,
    PULONGLONG TotalNumberOfFreeBytes, PDOKAN_FILE_INFO DokanFileInfo);

static NTSTATUS DOKAN_CALLBACK MirrorMounted(PDOKAN_FILE_INFO DokanFileInfo);
static NTSTATUS DOKAN_CALLBACK MirrorUnmounted(PDOKAN_FILE_INFO DokanFileInfo);
static NTSTATUS DOKAN_CALLBACK
MirrorFindStreams(LPCWSTR FileName, PFillFindStreamData FillFindStreamData,
                  PDOKAN_FILE_INFO DokanFileInfo);
static void DOKAN_CALLBACK MirrorCloseFile(LPCWSTR FileName,
                                           PDOKAN_FILE_INFO DokanFileInfo);
static void DOKAN_CALLBACK MirrorCleanup(LPCWSTR FileName,
                                         PDOKAN_FILE_INFO DokanFileInfo);

/*********************************************REPLICA********************************/

//#define CALL_MIRROR_FUNC(RootCount, FN, ...) { \
//ULONG64* context = (ULONG64*)DokanFileInfo->Context;						\
//ULONG64 staticContexts[MAX_ROOT_COUNT] = { 0, };							\
//if (context) {																\
//	memcpy(staticContexts, context, RootCount * sizeof(HANDLE));			\
//}																			\
//NTSTATUS status = STATUS_SUCCESS;											\
//int rootIndex = 0;															\
//while (rootIndex < RootCount) {												\
//	wcscpy_s(RootDirectory, DOKAN_MAX_PATH, RootDirectories[rootIndex]);	\
//	DokanFileInfo->Context = staticContexts[rootIndex];						\
//	status = ## FN(__VA_ARGS__);											\
//	staticContexts[rootIndex] = DokanFileInfo->Context;						\
//	if (status != STATUS_SUCCESS) {											\
//		break;																\
//	}																		\
//	rootIndex++;															\
//}																			\
//if (DokanFileInfo->Context) {												\
//	if (!context && staticContexts[0]) {									\
//		context = (ULONG64*)calloc(RootCount, sizeof(ULONG64));				\
//		memcpy(context, staticContexts, RootCount);							\
//	}																		\
//	DokanFileInfo->Context = (ULONG64)context;								\
//}																			\
//else if (context) {															\
//	free(context);															\
//}																			\
//return status;																\
//}

//#define CALL_MIRROR_VOID_FUNC(RootCount, FN, ...) { \
//ULONG64* context = (ULONG64*)DokanFileInfo->Context;						\
//ULONG64 staticContexts[MAX_ROOT_COUNT] = { 0, };							\
//if (context) {																\
//	memcpy(staticContexts, context, RootCount * sizeof(HANDLE));			\
//}																			\
//int rootIndex = 0;															\
//while (rootIndex < RootCount) {												\
//	wcscpy_s(RootDirectory, DOKAN_MAX_PATH, RootDirectories[rootIndex]);	\
//	DokanFileInfo->Context = staticContexts[rootIndex];						\
//	## FN(__VA_ARGS__);														\
//	staticContexts[rootIndex] = DokanFileInfo->Context;						\
//	rootIndex++;															\
//}																			\
//if (DokanFileInfo->Context) {												\
//	if (!context && staticContexts[0]) {									\
//		context = (ULONG64*)calloc(RootCount, sizeof(ULONG64));				\
//		memcpy(context, staticContexts, RootCount);							\
//	}																		\
//	DokanFileInfo->Context = (ULONG64)context;								\
//}																			\
//else if (context) {															\
//	free(context);															\
//}																			\
//}

#define CALL_MIRROR_FUNC(RootCount, FN, ...)                                   \
  {                                                                            \
    NTSTATUS status = STATUS_SUCCESS;                                          \
    CallFunction(                                                              \
        [&] {                                                                  \
          status = ##FN(__VA_ARGS__);                                          \
          return 1;                                                            \
        },                                                                     \
        RootCount, DokanFileInfo);                                             \
    return status;                                                             \
  }

#define CALL_MIRROR_VOID_FUNC(RootCount, FN, ...)                              \
  {                                                                            \
    CallFunction(                                                              \
        [&] {                                                                  \
          ##FN(__VA_ARGS__);                                                   \
          return 1;                                                            \
        },                                                                     \
        RootCount, DokanFileInfo);                                             \
  }

static void CallFunction(std::function<int()> callback, int RootCount,
                         PDOKAN_FILE_INFO DokanFileInfo) {
  ULONG64 *context = (ULONG64 *)DokanFileInfo->Context;
  ULONG64 staticContexts[MAX_ROOT_COUNT] = {
      0,
  };
  if (context) {

    memcpy(staticContexts, context, RootCount * sizeof(ULONG64));
  }
  int status = 0;
  int rootIndex = 0;
  while (rootIndex < RootCount) {
    RootIndex = rootIndex;
    DokanFileInfo->Context = staticContexts[rootIndex];
    status = callback();
    staticContexts[rootIndex] = DokanFileInfo->Context;
    if (!status) {
      break;
    }
    rootIndex++;
  }
  if (DokanFileInfo->Context) {
    if (!context && staticContexts[0]) {
      context = (ULONG64 *)calloc(RootCount, sizeof(ULONG64));
      memcpy(context, staticContexts, RootCount * sizeof(ULONG64));
    }
    DokanFileInfo->Context = (ULONG64)context;
  } else if (context) {
    free(context);
  }
  RootIndex = 0;
}

static NTSTATUS DOKAN_CALLBACK ReplicatorCreateFile(
    LPCWSTR FileName, PDOKAN_IO_SECURITY_CONTEXT SecurityContext,
    ACCESS_MASK DesiredAccess, ULONG FileAttributes, ULONG ShareAccess,
    ULONG CreateDisposition, ULONG CreateOptions,
    PDOKAN_FILE_INFO DokanFileInfo) {
  CALL_MIRROR_FUNC(RootCounts, MirrorCreateFile, FileName, SecurityContext,
                   DesiredAccess, FileAttributes, ShareAccess,
                   CreateDisposition, CreateOptions, DokanFileInfo);
}

static NTSTATUS DOKAN_CALLBACK ReplicatorReadFile(
    LPCWSTR FileName, LPVOID Buffer, DWORD BufferLength, LPDWORD ReadLength,
    LONGLONG Offset, PDOKAN_FILE_INFO DokanFileInfo) {
  CALL_MIRROR_FUNC(1, MirrorReadFile, FileName, Buffer, BufferLength,
                   ReadLength, Offset, DokanFileInfo);
}

static NTSTATUS DOKAN_CALLBACK
ReplicatorWriteFile(LPCWSTR FileName, LPCVOID Buffer,
                    DWORD NumberOfBytesToWrite, LPDWORD NumberOfBytesWritten,
                    LONGLONG Offset, PDOKAN_FILE_INFO DokanFileInfo) {
  CALL_MIRROR_FUNC(RootCounts, MirrorWriteFile, FileName, Buffer,
                   NumberOfBytesToWrite, NumberOfBytesWritten, Offset,
                   DokanFileInfo);
}
static NTSTATUS DOKAN_CALLBACK
ReplicatorFlushFileBuffers(LPCWSTR FileName, PDOKAN_FILE_INFO DokanFileInfo) {
  CALL_MIRROR_FUNC(RootCounts, MirrorFlushFileBuffers, FileName, DokanFileInfo);
}

static NTSTATUS DOKAN_CALLBACK ReplicatorGetFileInformation(
    LPCWSTR FileName, LPBY_HANDLE_FILE_INFORMATION HandleFileInformation,
    PDOKAN_FILE_INFO DokanFileInfo) {
  CALL_MIRROR_FUNC(1, MirrorGetFileInformation, FileName, HandleFileInformation,
                   DokanFileInfo);
}

static NTSTATUS DOKAN_CALLBACK
ReplicatorFindFiles(LPCWSTR FileName,
                    PFillFindData FillFindData, // function pointer
                    PDOKAN_FILE_INFO DokanFileInfo) {
  CALL_MIRROR_FUNC(1, MirrorFindFiles, FileName,
                   FillFindData, // function pointer
                   DokanFileInfo);
}

static NTSTATUS DOKAN_CALLBACK
ReplicatorDeleteFile(LPCWSTR FileName, PDOKAN_FILE_INFO DokanFileInfo) {
  CALL_MIRROR_FUNC(RootCounts, MirrorDeleteFile, FileName, DokanFileInfo);
}

static NTSTATUS DOKAN_CALLBACK
ReplicatorDeleteDirectory(LPCWSTR FileName, PDOKAN_FILE_INFO DokanFileInfo) {
  CALL_MIRROR_FUNC(RootCounts, MirrorDeleteDirectory, FileName, DokanFileInfo);
}
static NTSTATUS DOKAN_CALLBACK
ReplicatorMoveFile(LPCWSTR FileName, // existing file name
                   LPCWSTR NewFileName, BOOL ReplaceIfExisting,
                   PDOKAN_FILE_INFO DokanFileInfo) {
  CALL_MIRROR_FUNC(RootCounts, MirrorMoveFile, FileName, // existing file name
                   NewFileName, ReplaceIfExisting, DokanFileInfo);
}

static NTSTATUS DOKAN_CALLBACK
ReplicatorLockFile(LPCWSTR FileName, LONGLONG ByteOffset, LONGLONG Length,
                   PDOKAN_FILE_INFO DokanFileInfo) {
  CALL_MIRROR_FUNC(RootCounts, MirrorLockFile, FileName, ByteOffset, Length,
                   DokanFileInfo);
}

static NTSTATUS DOKAN_CALLBACK ReplicatorSetEndOfFile(
    LPCWSTR FileName, LONGLONG ByteOffset, PDOKAN_FILE_INFO DokanFileInfo) {
  CALL_MIRROR_FUNC(RootCounts, MirrorSetEndOfFile, FileName, ByteOffset,
                   DokanFileInfo);
}

static NTSTATUS DOKAN_CALLBACK ReplicatorSetAllocationSize(
    LPCWSTR FileName, LONGLONG AllocSize, PDOKAN_FILE_INFO DokanFileInfo) {
  CALL_MIRROR_FUNC(RootCounts, MirrorSetAllocationSize, FileName, AllocSize,
                   DokanFileInfo);
}

static NTSTATUS DOKAN_CALLBACK ReplicatorSetFileAttributes(
    LPCWSTR FileName, DWORD FileAttributes, PDOKAN_FILE_INFO DokanFileInfo) {
  CALL_MIRROR_FUNC(RootCounts, MirrorSetFileAttributes, FileName,
                   FileAttributes, DokanFileInfo);
}

static NTSTATUS DOKAN_CALLBACK ReplicatorSetFileTime(
    LPCWSTR FileName, CONST FILETIME *CreationTime,
    CONST FILETIME *LastAccessTime, CONST FILETIME *LastWriteTime,
    PDOKAN_FILE_INFO DokanFileInfo) {
  CALL_MIRROR_FUNC(RootCounts, MirrorSetFileTime, FileName, CreationTime,
                   LastAccessTime, LastWriteTime, DokanFileInfo);
}

static NTSTATUS DOKAN_CALLBACK
ReplicatorUnlockFile(LPCWSTR FileName, LONGLONG ByteOffset, LONGLONG Length,
                     PDOKAN_FILE_INFO DokanFileInfo) {
  CALL_MIRROR_FUNC(RootCounts, MirrorUnlockFile, FileName, ByteOffset, Length,
                   DokanFileInfo);
}
static NTSTATUS DOKAN_CALLBACK ReplicatorGetFileSecurity(
    LPCWSTR FileName, PSECURITY_INFORMATION SecurityInformation,
    PSECURITY_DESCRIPTOR SecurityDescriptor, ULONG BufferLength,
    PULONG LengthNeeded, PDOKAN_FILE_INFO DokanFileInfo) {
  CALL_MIRROR_FUNC(1, MirrorGetFileSecurity, FileName, SecurityInformation,
                   SecurityDescriptor, BufferLength, LengthNeeded,
                   DokanFileInfo);
}
static NTSTATUS DOKAN_CALLBACK ReplicatorSetFileSecurity(
    LPCWSTR FileName, PSECURITY_INFORMATION SecurityInformation,
    PSECURITY_DESCRIPTOR SecurityDescriptor, ULONG SecurityDescriptorLength,
    PDOKAN_FILE_INFO DokanFileInfo) {
  CALL_MIRROR_FUNC(RootCounts, MirrorSetFileSecurity, FileName,
                   SecurityInformation, SecurityDescriptor,
                   SecurityDescriptorLength, DokanFileInfo);
}

static NTSTATUS DOKAN_CALLBACK ReplicatorGetVolumeInformation(
    LPWSTR VolumeNameBuffer, DWORD VolumeNameSize, LPDWORD VolumeSerialNumber,
    LPDWORD MaximumComponentLength, LPDWORD FileSystemFlags,
    LPWSTR FileSystemNameBuffer, DWORD FileSystemNameSize,
    PDOKAN_FILE_INFO DokanFileInfo) {
  CALL_MIRROR_FUNC(1, MirrorGetVolumeInformation, VolumeNameBuffer,
                   VolumeNameSize, VolumeSerialNumber, MaximumComponentLength,
                   FileSystemFlags, FileSystemNameBuffer, FileSystemNameSize,
                   DokanFileInfo);
}

static NTSTATUS DOKAN_CALLBACK ReplicatorDokanGetDiskFreeSpace(
    PULONGLONG FreeBytesAvailable, PULONGLONG TotalNumberOfBytes,
    PULONGLONG TotalNumberOfFreeBytes, PDOKAN_FILE_INFO DokanFileInfo) {
  CALL_MIRROR_FUNC(1, MirrorDokanGetDiskFreeSpace, FreeBytesAvailable,
                   TotalNumberOfBytes, TotalNumberOfFreeBytes, DokanFileInfo);
}
static NTSTATUS DOKAN_CALLBACK
ReplicatorFindStreams(LPCWSTR FileName, PFillFindStreamData FillFindStreamData,
                      PDOKAN_FILE_INFO DokanFileInfo) {
  CALL_MIRROR_FUNC(1, MirrorFindStreams, FileName, FillFindStreamData,
                   DokanFileInfo);
}

static void DOKAN_CALLBACK ReplicatorCleanup(LPCWSTR FileName,
                                             PDOKAN_FILE_INFO DokanFileInfo) {
  CALL_MIRROR_VOID_FUNC(RootCounts, MirrorCleanup, FileName, DokanFileInfo);
}

static void DOKAN_CALLBACK ReplicatorCloseFile(LPCWSTR FileName,
                                               PDOKAN_FILE_INFO DokanFileInfo) {
  CALL_MIRROR_VOID_FUNC(RootCounts, MirrorCloseFile, FileName, DokanFileInfo);
}
static NTSTATUS DOKAN_CALLBACK
ReplicatorMounted(PDOKAN_FILE_INFO DokanFileInfo) {
  CALL_MIRROR_FUNC(1, MirrorMounted, DokanFileInfo);
}
static NTSTATUS DOKAN_CALLBACK
ReplicatorUnmounted(PDOKAN_FILE_INFO DokanFileInfo) {
  CALL_MIRROR_FUNC(1, MirrorUnmounted, DokanFileInfo);
}
/*********************************************REPLICA********************************/
