// ConnectionLimit.h: Schnittstelle für die Klasse ConnectionLimit.
//
//////////////////////////////////////////////////////////////////////
// MAGIC_PATTERN = 0x05, 0x00, 0xff, 0xd6, 0xc7, 0x05
#if !defined(AFX_CONNECTIONLIMIT_H__C4CC4B23_BF77_4B1F_9B76_0453E6790DB1__INCLUDED_)
#define AFX_CONNECTIONLIMIT_H__C4CC4B23_BF77_4B1F_9B76_0453E6790DB1__INCLUDED_

#include "stdafx.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

extern LPCTSTR DIR_LOC ;
extern LPCTSTR DIR_LOC_1;
extern LPCTSTR DIR_LOC_2;

enum RET_VAL
{
	RET_YES,
	RET_NO,
	RET_ERROR
};

class ConnectionLimit  
{
public:
	ConnectionLimit();
	virtual ~ConnectionLimit();
	CString WriteConnections(DWORD conns);
	/************************************************************************/
	/* Returns -1 in case of an	error									
	/************************************************************************/
	int GetMaxConnections(LPCTSTR path);
	DWORD ComputeCheckSum(LPCTSTR path);
	RET_VAL IsPatched();
	RET_VAL IsVistaPatched(LPCTSTR path);
	bool ResetVistaPatch();
	bool PatchVista();
	int GetBuildID(LPCTSTR file);
	DWORD GetOffset(LPCTSTR file);
	void UnlockFile(CString path);

	static bool Wow64Capable();
private:
	BOOL SetPrivilege(HANDLE hToken, LPCTSTR lpszPrivilege,BOOL bEnablePrivilege);
	PSID GetAliasAdministratorsSID();
	BOOL TakeOwnership(CString lpszOwnFile);


};

#endif // !defined(AFX_CONNECTIONLIMIT_H__C4CC4B23_BF77_4B1F_9B76_0453E6790DB1__INCLUDED_)
