#pragma once

#include <WinCred.h>
#include <StrSafe.h>

#include <cassert>
#include <vector>
#include <string>
#include <sstream>
#include <stdexcept>

using std::endl;
using std::vector;
using std::wstring;
using std::exception;
using std::wostringstream;

const DWORD PROCESS_ID_INVALID = (DWORD)-1;
const DWORD PROCESS_ID_SYSTEM = (DWORD)0;

class ProgramException {

public:
    ProgramException() { }
    ProgramException(LPCWSTR message) : m_what(message) { }
public:
    LPCWSTR what() const { return m_what.c_str(); }
protected:
    wstring m_what;
};

const UINT MSG_LOG_MESSAGE = WM_USER+1;
const UINT MSG_UPDATE_HANDLE_TABLE = WM_USER+2;
const UINT MSG_UPDATE_AVAILABLE = WM_USER+3;
const UINT MSG_WAITING_MUTEX = WM_USER+4;
const UINT MSG_ACQUIRED_MUTEX = WM_USER+5;
const UINT MSG_RELEASED_MUTEX = WM_USER+6;
const UINT MSG_ABANDONED_MUTEX = WM_USER+7;
const UINT MSG_WAITFAILED = WM_USER+8;
const UINT MSG_CANCEL_WAITING = WM_USER+9;
const UINT MSG_MUTEX_STATE_CHANGE = WM_USER+10;
const UINT MSG_PROCESS_LIST_CHANGED = WM_USER+11;

const LPARAM HINT_REFRESH_PROCESSES = 1;
const LPARAM HINT_PRIVILEGE_CHANGED = 2;
const LPARAM HINT_PROCESS_CHANGED = 3;
const LPARAM HINT_PROCESS_LIST_CHANGED = 4;
const LPARAM HINT_HANDLETABLE_CHANGED = 5;
const LPARAM HINT_LOG_MESSAGE = 6;
const LPARAM HINT_UPDATE_HANDLE_TABLE = 7;
const LPARAM HINT_WAITING_MUTEX = 8;
const LPARAM HINT_ACQUIRED_MUTEX = 9;
const LPARAM HINT_RELEASED_MUTEX = 10;
const LPARAM HINT_ABANDONED_MUTEX = 11;
const LPARAM HINT_WAITFAILED = 12;
const LPARAM HINT_CANCEL_WAITING = 13;
const LPARAM HINT_MUTEX_STATE_CHANGE = 14;

// Right click Attacks and Attack Menu
const LPARAM HINT_ATTACK_SQUAT = 0x20;

DWORD TestMutex(HANDLE handle);
wstring MutexStateAsString(HANDLE handle);

struct ProcessData
{
    ProcessData( ) : nParentId(PROCESS_ID_INVALID), nProcessId(PROCESS_ID_INVALID) { }
    ProcessData( INT parentId, INT processId )
        : nParentId(parentId), nProcessId(processId) { }
    ProcessData( INT parentId, INT processId, LPCWSTR name )
        : nParentId(parentId), nProcessId(processId), szName(name) { }

    INT nProcessId;
    INT nParentId;
    CString szName;
};

bool operator==( const ProcessData& lhs, const ProcessData& rhs );

class ProcessDataList: public vector<ProcessData> { };

struct MonitorParams
{
    DWORD m_dwFrequency;
    const ProcessDataList& m_currentList;
    CCriticalSection& m_listLock;
    CEvent& m_shutdown;

    explicit MonitorParams(DWORD, const ProcessDataList&, CCriticalSection&, CEvent&);
};

struct AutoHandle
{
    AutoHandle();
    // Construction with duplicate
    AutoHandle(HANDLE);
    // Construction with duplicate
    AutoHandle(const AutoHandle&);
    // Assignment with duplicate
    AutoHandle& operator=(HANDLE);
    // Assignment with duplicate
    AutoHandle& operator=(const AutoHandle&);

    ~AutoHandle();

    // Returns a reference to the underlying handle. The underlying handle is not duplicated.
    //  If the handle is over written, the previous handle is not released.
    HANDLE& Ref() { return m_handle; }

    // Returns a reference to the underlying handle. The underlying handle is not duplicated.
    const HANDLE& Ref() const { return m_handle; }

    // Attach a handle to this. The handle is not duplicated. Handle will be closed during detach or destruction.
    void Attach( HANDLE h );
    // Detach a handle from this. Handle will be closed during detach or destruction.
    void Detach( );
    // Deatch a handle from this *without* closing. If the handle was duplicated, it will leak.
    void Unattach( );

    // Assigns a handle to this. The handle is duplicated. Handle will be closed on destruction.
    void Assign( HANDLE h );
    // Closes the handle associated with this.
    void Close( );    

    // Does not duplicate
    operator HANDLE();

    // Duplicate this handle
    HANDLE Duplcate() const;

private:
    HANDLE m_handle;
};

struct SquatParams
{
    enum State { Failed=-1, None=0, Waiting=1, Canceled, Acquired, Released, Abandoned };

    State m_state;
    INT m_pid;              // source pid
    INT m_number;           // mutex number (pid relative)
    wstring m_name;         // name (if present)

    AutoHandle m_target;    // mutex (*must* be duplicated)
    AutoHandle m_cancel;    // cancel event (cancel wait for mutex)
    AutoHandle m_release;   // release event (release the acquired mutex)
    AutoHandle m_abandon;   // abandon event (abandon the acquired mutex)
    AutoHandle m_thread;    // thread performing the squat    

    INT m_pos;              // position in the Attack listview

    SquatParams();
    virtual ~SquatParams();
};

struct MutexHandle
{
    MutexHandle() { };
    virtual ~MutexHandle() { };

    AutoHandle m_handle;    // handle, duplicated with max rights
    wstring m_name;         // event name
    wstring m_dacl;         // NULL or other comment
    wstring m_state;        // signaled or nonsignaled
};

const DWORD MUTEX_ACCESS[] =
{
    MUTEX_ALL_ACCESS,
    SYNCHRONIZE | MUTANT_QUERY_STATE,
    SYNCHRONIZE,
    // Lowest of the low
    MUTANT_QUERY_STATE
};

typedef vector<MutexHandle> HandleTable;

const size_t DOUBLE_CURRENT = 0;

struct AutoBuffer
{
    AutoBuffer( size_t size = 64 );
    ~AutoBuffer(){ };

    void Allocate( size_t size );
    void Reallocate( size_t size = DOUBLE_CURRENT );
    void Free() { m_buffer.clear();}

    size_t Size() const{ return m_buffer.size(); }

    operator LPVOID();

private:
    vector<BYTE> m_buffer;
};

typedef AutoBuffer HandleTableAutoPtr;
typedef AutoBuffer ObjTypeInfoAutoPtr;
typedef AutoBuffer ObjNameInfoAutoPtr;

// We want the data structures, but not the library dependencies.
// We'll locate the functions on the fly.
#include <Tlhelp32.h>
typedef HANDLE (WINAPI* CREATETOOLHELP32SNAPSHOT)(
    DWORD dwFlags,
    DWORD th32ProcessID
  );
typedef BOOL (WINAPI* PROCESS32FIRST)(
    HANDLE hSnapshot,
    LPPROCESSENTRY32 lppe
  );
typedef BOOL (WINAPI* PROCESS32NEXT)(
    HANDLE hSnapshot,
    LPPROCESSENTRY32 lppe
  );
typedef BOOL (WINAPI* MODULE32FIRST)(
    HANDLE hSnapshot,
    LPMODULEENTRY32 lpme
  );
typedef BOOL (WINAPI* MODULE32NEXT)(
    HANDLE hSnapshot,
    LPMODULEENTRY32 lpme
 );

struct ProcessFunctions
{
    ProcessFunctions();
    ~ProcessFunctions();

    CREATETOOLHELP32SNAPSHOT pfnCreateToolhelp32Snapshot;
    PROCESS32FIRST pfnProcess32First;
    PROCESS32NEXT pfnProcess32Next;
    MODULE32FIRST pfnModule32First;

    operator HMODULE() { return m_hModule; }

private:
    HMODULE m_hModule;
};

#ifndef NT_SUCCESS
# define NT_SUCCESS(x) ((x) >= 0)
#endif

#ifndef STATUS_SUCCESS
# define STATUS_SUCCESS (NTSTATUS)0x00
#endif

#ifndef STATUS_INFO_LENGTH_MISMATCH
# define STATUS_INFO_LENGTH_MISMATCH (NTSTATUS)0xc0000004
#endif

#ifndef STATUS_ACCESS_DENIED
# define STATUS_ACCESS_DENIED (NTSTATUS)0xc0000022
#endif

#ifndef STATUS_BUFFER_TOO_SMALL
# define STATUS_BUFFER_TOO_SMALL (NTSTATUS)0xc0000023
#endif

#ifndef STATUS_NOT_SUPPORTED
# define STATUS_NOT_SUPPORTED (NTSTATUS)0xc00000bb
#endif

#ifndef DUPLICATE_SAME_ATTRIBUTES
# define DUPLICATE_SAME_ATTRIBUTES (NTSTATUS)0x00000004
#endif

#define SystemHandleInformation 16
#define ObjectBasicInformation 0
#define ObjectNameInformation 1
#define ObjectTypeInformation 2

#ifndef NTSTATUS
typedef LONG NTSTATUS;
#endif // NTSTATUS

#ifndef UNICODE_STRING
typedef struct _UNICODE_STRING
{
    USHORT Length;
    USHORT MaximumLength;
    PWSTR Buffer;
}
UNICODE_STRING, *PUNICODE_STRING;
#endif // UNICODE_STRING

#ifndef SYSTEM_HANDLE
typedef struct _SYSTEM_HANDLE
{
    ULONG ProcessId;
    BYTE ObjectTypeNumber;
    BYTE Flags;
    USHORT Handle;
    PVOID Object;
    ACCESS_MASK GrantedAccess;
}
SYSTEM_HANDLE, *PSYSTEM_HANDLE;
#endif SYSTEM_HANDLE

#ifndef SYSTEM_HANDLE_INFORMATION
typedef struct _SYSTEM_HANDLE_INFORMATION
{
    ULONG HandleCount;
    SYSTEM_HANDLE Handles[ANYSIZE_ARRAY];
}
SYSTEM_HANDLE_INFORMATION, *PSYSTEM_HANDLE_INFORMATION;
#endif SYSTEM_HANDLE_INFORMATION

#ifndef POOL_TYPE
typedef enum _POOL_TYPE
{
    NonPagedPool,
    PagedPool,
    NonPagedPoolMustSucceed,
    DontUseThisType,
    NonPagedPoolCacheAligned,
    PagedPoolCacheAligned,
    NonPagedPoolCacheAlignedMustS
}
POOL_TYPE, *PPOOL_TYPE;
#endif // POOL_TYPE

#ifndef OBJECT_TYPE_INFORMATION
typedef struct _OBJECT_TYPE_INFORMATION
{
    UNICODE_STRING Name;
    ULONG TotalNumberOfObjects;
    ULONG TotalNumberOfHandles;
    ULONG TotalPagedPoolUsage;
    ULONG TotalNonPagedPoolUsage;
    ULONG TotalNamePoolUsage;
    ULONG TotalHandleTableUsage;
    ULONG HighWaterNumberOfObjects;
    ULONG HighWaterNumberOfHandles;
    ULONG HighWaterPagedPoolUsage;
    ULONG HighWaterNonPagedPoolUsage;
    ULONG HighWaterNamePoolUsage;
    ULONG HighWaterHandleTableUsage;
    ULONG InvalidAttributes;
    GENERIC_MAPPING GenericMapping;
    ULONG ValidAccess;
    BOOLEAN SecurityRequired;
    BOOLEAN MaintainHandleCount;
    USHORT MaintainTypeList;
    POOL_TYPE PoolType;
    ULONG PagedPoolUsage;
    ULONG NonPagedPoolUsage;
}
OBJECT_TYPE_INFORMATION, *POBJECT_TYPE_INFORMATION;
#endif // OBJECT_TYPE_INFORMATION

typedef NTSTATUS (NTAPI * NTQUERYSYSTEMINFORMATION)(
    ULONG SystemInformationClass,
    PVOID SystemInformation,
    ULONG SystemInformationLength,
    PULONG ReturnLength
    );
typedef NTSTATUS (NTAPI * NTDUPLICATEOBJECT)(
    HANDLE SourceProcessHandle,
    HANDLE SourceHandle,
    HANDLE TargetProcessHandle,
    PHANDLE TargetHandle,
    ACCESS_MASK DesiredAccess,
    ULONG Attributes,
    ULONG Options
    );
typedef NTSTATUS (NTAPI * NTQUERYOBJECT)(
    HANDLE ObjectHandle,
    ULONG ObjectInformationClass,
    PVOID ObjectInformation,
    ULONG ObjectInformationLength,
    PULONG ReturnLength
    );
typedef NTSTATUS (NTAPI * NTCLOSE)(
    HANDLE ObjectHandle
 );

struct HandleFunctions
{
    HandleFunctions();
    ~HandleFunctions();

    NTQUERYSYSTEMINFORMATION pfnNtQuerySystemInformation;
    NTQUERYOBJECT pfnNtQueryObject;
    NTDUPLICATEOBJECT pfnNtDuplicateObject;
    NTCLOSE pfnNtClose;

    operator HMODULE() { return m_hModule; }

private:
    HMODULE m_hModule;
};

typedef BOOL (WINAPI * ISWOW64PROCESS)(
    HANDLE hProcess,
    PBOOL Wow64Process
);

struct Wow64ProcFunctions
{
    Wow64ProcFunctions();
    ~Wow64ProcFunctions();

    ISWOW64PROCESS pfnIsWow64Process;

    operator HMODULE() { return m_hModule; }

private:
    HMODULE m_hModule;
};
