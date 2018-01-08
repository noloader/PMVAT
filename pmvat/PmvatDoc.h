// PmvatDoc.h : interface of the CPmvatDoc class
//

#pragma once

#include "Common.h"
#include "Aclapi.h"

class CPmvatDoc: public CDocument
{
protected: // create from serialization only
    CPmvatDoc();
    DECLARE_DYNCREATE(CPmvatDoc)
    virtual ~CPmvatDoc();

#ifdef _DEBUG
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext& dc) const;
#endif
    virtual void Serialize(CArchive& ar);
    virtual BOOL SaveModified();

    DECLARE_MESSAGE_MAP()

public:
    BOOL ResetDocument();
    BOOL PrivilegeChanged();
    BOOL ProcessChanged( LPCWSTR procName, INT newPid );

protected:
    BOOL DuplicateMutexHandles(INT pid);
    BOOL DuplicateMutexHandle(const HANDLE& hProcess, const SYSTEM_HANDLE& syshandle);

    BOOL QueryMutexHandleName( MutexHandle& handle );
    BOOL QueryMutexHandleDacl( MutexHandle& handle );
    BOOL QueryMutexHandleState( MutexHandle& handle );

public:
    INT m_pid;
    CString m_name;

    INT m_procs;

    INT m_named;
    INT m_unnamed;
    INT m_total;

    // Table displayed in the ListView
    HandleTable m_hTable;

    CCriticalSection m_csection; // modify the document

private:
    HandleFunctions m_hfuncs;
};


