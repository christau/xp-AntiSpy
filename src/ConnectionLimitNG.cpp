/*
	Copyright (C) 2001 by Christian Taubenheim <mail at xp-antispy.org>
	This file is part of xp-AntiSpy http://www.xp-AntiSpy.org.

    xp-AntiSpy is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    xp-AntiSpy is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with xp-AntiSpy. If not, see <http://www.gnu.org/licenses/
*/

#include "ConnectionLimitNG.h"
#include "AntiSpySettings.h"
#include <imagehlp.h>

#include "verinfo.h"
#include "iswow64.h"
//no
#include <sys/types.h>

#include "Aclapi.h"
#include <sys/stat.h>
#include <io.h>
//no
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
//

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#ifdef _DEBUG
	LPCTSTR DIR_LOC = _T("\\system32\\drivers\\tcpip_test.sys");
//const char * DIR_LOC = "\\system32\\drivers\\tcpip_test.sys";
#else
	LPCTSTR DIR_LOC = _T("\\system32\\drivers\\tcpip.sys");
#endif
	LPCTSTR DIR_LOC_1 = _T("\\SERVICEPACKFILES\\I386\\tcpip.sys");
	LPCTSTR DIR_LOC_2 = _T("\\system32\\dllcache\\tcpip.sys");
	const char MAGIC_32[6] = {0x05, 0x00, 0xff, 0xd6, 0xc7, 0x05};
	TCHAR VISTA_MAGIC_32[9] = {(TCHAR)0x8b, (TCHAR)0x15, (TCHAR)0x68, (TCHAR)0xe0, (TCHAR)0x0c, (TCHAR)0x00, (TCHAR)0x83, (TCHAR)0xfa, (TCHAR)0x0a};
	TCHAR MAGIC_64[24] = {(TCHAR)0xFB, (TCHAR)0xFF, (TCHAR)0x48, (TCHAR)0x83, (TCHAR)0xEE, (TCHAR)0x80, (TCHAR)0x48, (TCHAR)0xFF, (TCHAR)0xCB, (TCHAR)0x75, (TCHAR)0xC5, (TCHAR)0x48, (TCHAR)0x8B, (TCHAR)0x74, (TCHAR)0x24, (TCHAR)0x60, (TCHAR)0x48, (TCHAR)0x8B, (TCHAR)0x5C, (TCHAR)0x24, (TCHAR)0x58, (TCHAR)0x48, (TCHAR)0x89, (TCHAR)0x3D};

const int CHUNK_SIZE = 1024;


ConnectionLimit::ConnectionLimit()
{

}

ConnectionLimit::~ConnectionLimit()
{

}

RET_VAL ConnectionLimit::IsPatched()
{
	TCHAR windir[MAX_PATH];
	GetWindowsDirectory(windir, MAX_PATH);
	wcscat(windir,DIR_LOC);
	if(CAntiSpySettings::GetSuiteType() == CAntiSpySettings::SUITE_TYPE::WINDOWS_VISTA)
	{
		return IsVistaPatched(windir);
	}
	else
	{
		const int conns = GetMaxConnections(windir);
		if(conns==10) return RET_NO;
		if(conns==-1) return RET_ERROR;
		return RET_YES;
	}
}

RET_VAL ConnectionLimit::IsVistaPatched(LPCTSTR path)
{
	UnlockFile("c:\\windows\\system32\\drivers\\vga.sys");
	CFile file;
	DWORD dwOffset = 0;
	bool isWow = IsWow64();
	PVOID oldValue = NULL;
	if(isWow)
	{
		MyWow64DisableWow64FsRedirection(&oldValue);
	}

	if(file.Open(path,CFile::modeRead))
	{
		TCHAR patchedChar = 0x90;
		if(isWow)
		{
			MyWow64RevertWow64FsRedirection(oldValue);
		}
		unsigned int bytesRead = 0;
		TCHAR buffer[1024];
		bytesRead = file.Read(buffer, 1024);
		while(bytesRead != 0)
		{
			for (int i = 0; i<bytesRead; ++i)
			{
				if(!isWow)
				{

					if(buffer[i] == VISTA_MAGIC_32[0])
					{
						if(i>= (bytesRead - 8))
						{
							int pos = file.GetPosition();
							file.Seek(-8, CFile::current);
							bytesRead-=8;
							pos = file.GetPosition();
							break;
						}

						for (int c = 1; c < 9; ++c)
						{
							if(buffer[i+c] != VISTA_MAGIC_32[c])
							{
								break;
							}
							else
							{
								if(c == 8)
								{
									dwOffset += i + 9;
									bool patched = true;
									for(int p = 0; p < 6; ++p)
									{
										if(buffer[i+9+p] != patchedChar)
											patched = false;
									}
									file.Close();
									if(patched)
										return RET_YES;
									else
										return RET_NO;
									//return dwOffset;
								}
							}
						}
					}
				}
				else
				{
					if(buffer[i] == MAGIC_64[0])
					{
						if(i>= (bytesRead - 24))
						{
							int pos = file.GetPosition();
							file.Seek(-24, CFile::current);
							bytesRead-=24;
							pos = file.GetPosition();
							break;
						}

						for (int c = 1; c < 24; ++c)
						{
							if(buffer[i+c] != MAGIC_64[c])
							{
								break;
							}
							else
							{
								if(c == 23)
								{
									dwOffset += i + 50;
									file.Close();
									//return dwOffset;
								}
							}
						}
					}

				}
			}
			dwOffset += bytesRead;
			bytesRead = file.Read(buffer, 1024);
		}
		file.Close();
	}
	return RET_ERROR;

}

bool ConnectionLimit::Wow64Capable()
{
	return IsWow64();

}

int ConnectionLimit::GetBuildID(LPCTSTR file)
{
    DWORD dwLen, dwUseless;
    LPTSTR lpVI;
	TCHAR path[MAX_PATH];
	wcscpy(path,file);

	PVOID pOldValue = NULL; 
    if (IsWow64()) 
    { 
		int ret = MyWow64DisableWow64FsRedirection(&pOldValue);
	}
	dwLen = GetFileVersionInfoSize(path, &dwUseless);
    if (dwLen==0)
        return 0;

    lpVI = (LPTSTR) malloc(dwLen);
 	int buildNo=0;
	if (lpVI)
    {
        DWORD dwBufSize;
        VS_FIXEDFILEINFO* lpFFI;
        BOOL bRet = FALSE;
        GetFileVersionInfo(path, NULL, dwLen, lpVI);

        if (VerQueryValue(lpVI, _T("\\"),(LPVOID*) &lpFFI, (UINT*) &dwBufSize))
        {
			buildNo = LOWORD(lpFFI->dwProductVersionLS);
		}
		else
			return 0;
        free(lpVI);

    }	
    if (IsWow64()) 
    { 
		MyWow64RevertWow64FsRedirection(pOldValue);
	}

	return buildNo;
}

DWORD ConnectionLimit::ComputeCheckSum(LPCTSTR path)
{
	DWORD dwHeaderSum;
	DWORD dwCheckSum;
	PVOID oldValue = NULL;
	if(IsWow64())
	{
		MyWow64DisableWow64FsRedirection(&oldValue);
	}
	HANDLE hFile=CreateFile(path,GENERIC_READ|GENERIC_WRITE,FILE_SHARE_READ,0,OPEN_EXISTING,0,0);
	if(IsWow64())
	{
		MyWow64RevertWow64FsRedirection(oldValue);
	}
	if(hFile==INVALID_HANDLE_VALUE)
	{
		return 0;
	}
	HANDLE mapping = CreateFileMapping(hFile,0,PAGE_READWRITE,0,0,NULL);
	if(!mapping)
	{
		CloseHandle(hFile);
		return 0;
	}		
	unsigned char* pView=(unsigned char *)MapViewOfFile(mapping,FILE_MAP_ALL_ACCESS,0,0,0);
	if(!pView)
	{
		CloseHandle(mapping);
		CloseHandle(hFile);
		return 0;
	}		
	DWORD nFileSize=GetFileSize(hFile,NULL);
	PIMAGE_NT_HEADERS pHeader=CheckSumMappedFile((LPVOID)pView,nFileSize,&dwHeaderSum,&dwCheckSum);	
	pHeader->OptionalHeader.CheckSum=dwCheckSum;
	pHeader=CheckSumMappedFile((LPVOID)pView,nFileSize,&dwHeaderSum,&dwCheckSum);	
	UnmapViewOfFile((LPCVOID)pView);
	CloseHandle(mapping);
	CloseHandle(hFile);			
	return dwCheckSum;
}

int ConnectionLimit::GetMaxConnections(LPCTSTR path)
{
	if(CAntiSpySettings::GetSuiteType() == CAntiSpySettings::SUITE_TYPE::WINDOWS_VISTA)
	{
		CString val = CAntiSpySettings::GetRegKeyValue(HKEY_LOCAL_MACHINE, _T("SYSTEM\\CurrentControlSet\\Services\\Tcpip\\Parameters"),_T("TcpNumConnections"),REG_DWORD);
		if(val.IsEmpty())
			return 0;
		int conns = _wtoi(val);
	}
	else
	{
		PVOID oldValue = NULL;
		DWORD conns = 0;
		int id = GetBuildID(path);
		CFile file;
		DWORD dwOffset = 0;
		dwOffset = GetOffset(path);
		if(IsWow64())
		{
			if(MyWow64DisableWow64FsRedirection(&oldValue) != TRUE)
				return -1;
		}
		if(!file.Open(path, CFile::modeRead))
		{
			MyWow64RevertWow64FsRedirection(oldValue);
			return -1;
		}
		if(IsWow64())
		{
			MyWow64RevertWow64FsRedirection(oldValue);
		}
		if(dwOffset)
		{
			file.Seek(dwOffset, CFile::begin);
			file.Read(&conns,sizeof(conns));
		}
		file.Close();
		return conns;
	}
}
bool ConnectionLimit::ResetVistaPatch()
{
	return false;
}

bool ConnectionLimit::PatchVista()
{

	PVOID oldValue = NULL;
	TCHAR windir[MAX_PATH];
	GetWindowsDirectory(windir, MAX_PATH);
	CString path(windir);
	path+=DIR_LOC;
	int id = GetBuildID(path);
	if(id != 16384)
		return false;
	
	CFile file;
	DWORD dwOffset = 0;
	dwOffset = GetOffset(path);
	if(IsWow64())
	{
		MyWow64DisableWow64FsRedirection(&oldValue);
	}
	if(!file.Open(path,CFile::modeReadWrite))
	{
		//unlock file if not alredy unlocked
		UnlockFile(path);
	}
	else
	{
		file.Close();
	}
	if(file.Open(path,CFile::modeReadWrite))
	{
		if(IsWow64())
		{
			MyWow64RevertWow64FsRedirection(oldValue);
		}
		if(dwOffset)
		{
			file.Seek(dwOffset,CFile::begin);
			TCHAR num = 0x90;
			for(int i = 0; i<6; ++i)
			{
				file.Write(&num,sizeof(num));
			}
			file.Close();
			DWORD checkSum = ComputeCheckSum(path);
			return true;
		}
	}
	return false;
}
void ConnectionLimit::UnlockFile(CString path)
{
	//TakeOwnership(path);
	TCHAR          wszDN[512];
    DWORD          cchDN = 512;

	GetUserName(wszDN, &cchDN);
	//return;
	SHELLEXECUTEINFO seInfo;
	ZeroMemory(&seInfo, sizeof(SHELLEXECUTEINFO));
	
	seInfo.cbSize=sizeof(SHELLEXECUTEINFO);
	seInfo.nShow = SW_HIDE;
	seInfo.lpVerb = _T("open");
	seInfo.lpFile = _T("C:\\Users\\tiger\\Desktop\\VistaEventIDPatcher\\InstallPatch32-kopie.bat");
	//fileStr.Format(_T("/s %s"),dllName);
//	seInfo.lpParameters = "/U administrator /f c:\\windows\\System32\\drivers\\wanarp.sys";
	seInfo.lpParameters = _T("");
	ShellExecuteEx(&seInfo);
	int ret = GetLastError();
	

	ZeroMemory(&seInfo, sizeof(SHELLEXECUTEINFO));
	seInfo.cbSize=sizeof(SHELLEXECUTEINFO);
	seInfo.nShow = SW_HIDE;
	seInfo.lpFile = _T("cmd");
	//fileStr.Format(_T("/s %s"),dllName);
	seInfo.lpParameters = _T("echo y | cacls %Systemroot%\\System32\\drivers\\vga.sys /g \"%username%\":f");
	ShellExecuteEx(&seInfo);
	Sleep(3000);
	
	
	SECURITY_DESCRIPTOR SecurityDescriptor;
    PSID pEveryoneSid = NULL;
    PSID pAdminSid = NULL;
    BOOL bRet;

	BOOL retz = SetPrivilege(NULL, _T("SeTakeOwnershipPrivilege"),TRUE);

    pAdminSid = GetAliasAdministratorsSID();
    if(!pAdminSid)
    {
        return;
    }
    
    InitializeSecurityDescriptor(&SecurityDescriptor, SECURITY_DESCRIPTOR_REVISION);

    bRet = SetSecurityDescriptorOwner(&SecurityDescriptor,pAdminSid,FALSE);

    if(bRet) 
    {
		bRet = SetFileSecurity(path, OWNER_SECURITY_INFORMATION, &SecurityDescriptor);
    }

    if(pAdminSid) 
        FreeSid(pAdminSid);


}
BOOL ConnectionLimit::SetPrivilege(HANDLE hToken, LPCTSTR lpszPrivilege,BOOL bEnablePrivilege)
{
    TOKEN_PRIVILEGES tp;
    //HANDLE           hToken;
    DWORD            dwLastError;

    tp.Privileges[0].Attributes = bEnablePrivilege? SE_PRIVILEGE_ENABLED:0;
    tp.PrivilegeCount = 1;

    LookupPrivilegeValue(NULL,lpszPrivilege,&tp.Privileges[0].Luid);
    BOOL wasNull = (hToken == NULL);
	if(hToken == NULL)
	{
		OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES,&hToken);
	}
    AdjustTokenPrivileges(hToken,FALSE,&tp,sizeof(tp),NULL,NULL);
    dwLastError = GetLastError();
    if(wasNull)
		CloseHandle(hToken);
    if(dwLastError != ERROR_SUCCESS)
    {
        return FALSE;
    }
    return TRUE;
}

PSID ConnectionLimit::GetAliasAdministratorsSID()
{	
    SID_IDENTIFIER_AUTHORITY sidAuth = SECURITY_NT_AUTHORITY;
    PSID psid = NULL;
    BOOL bRet = AllocateAndInitializeSid( 
        &sidAuth, 
        2,
        SECURITY_BUILTIN_DOMAIN_RID, 
        DOMAIN_ALIAS_RID_ADMINS,
        0, 0, 0, 0, 0, 0, &psid );

    if(!bRet) 
        return NULL;
    return psid;
}

BOOL ConnectionLimit::TakeOwnership(CString fileName) 
{
// 	LPTSTR lpszOwnFile = fileName.GetBuffer(fileName.GetLength());
//     BOOL bRetval = FALSE;
// 
//     HANDLE hToken = NULL; 
//     PSID pSIDAdmin = NULL;
//     PSID pSIDEveryone = NULL;
//     PACL pACL = NULL;
//     SID_IDENTIFIER_AUTHORITY SIDAuthWorld =SECURITY_WORLD_SID_AUTHORITY;
//     SID_IDENTIFIER_AUTHORITY SIDAuthNT = SECURITY_NT_AUTHORITY;
//     const int NUM_ACES  = 2;
//     EXPLICIT_ACCESS ea[NUM_ACES];
//     DWORD dwRes;
// 
    // Specify the DACL to use.
    // Create a SID for the Everyone group.
//     if (!AllocateAndInitializeSid(&SIDAuthWorld, 1,SECURITY_WORLD_RID, 0, 0, 0, 0, 0, 0, 0, &pSIDEveryone)) 
//     {
//         printf("AllocateAndInitializeSid (Everyone) error %u\n",
//                 GetLastError());
// 		bRetval = FALSE;
//         goto Cleanup;
//     }

    // Create a SID for the BUILTIN\Administrators group.
 /*   if (!AllocateAndInitializeSid(&SIDAuthNT, 2, SECURITY_BUILTIN_DOMAIN_RID,  DOMAIN_ALIAS_RID_ADMINS,  0, 0, 0, 0, 0, 0,  &pSIDAdmin)) 
    {
        printf("AllocateAndInitializeSid (Admin) error %u\n",
                GetLastError());
        goto Cleanup;
    }*/
// 	pSIDAdmin = GetAliasAdministratorsSID();
// 
//     ZeroMemory(&ea, NUM_ACES * sizeof(EXPLICIT_ACCESS));
// 
//     // Set read access for Everyone.
//     ea[0].grfAccessPermissions = GENERIC_READ;
//     ea[0].grfAccessMode = SET_ACCESS;
//     ea[0].grfInheritance = NO_INHERITANCE;
//     ea[0].Trustee.TrusteeForm = TRUSTEE_IS_SID;
//     ea[0].Trustee.TrusteeType = TRUSTEE_IS_WELL_KNOWN_GROUP;
//     ea[0].Trustee.ptstrName = (LPTSTR) pSIDEveryone;
// 
//     // Set full control for Administrators.
//     ea[1].grfAccessPermissions = GENERIC_ALL;
//     ea[1].grfAccessMode = SET_ACCESS;
//     ea[1].grfInheritance = NO_INHERITANCE;
//     ea[1].Trustee.TrusteeForm = TRUSTEE_IS_SID;
//     ea[1].Trustee.TrusteeType = TRUSTEE_IS_GROUP;
//     ea[1].Trustee.ptstrName = (LPTSTR) pSIDAdmin;
// 	PEXPLICIT_ACCESS pac = NULL;
// 	ULONG num = 0;
// 	
// 	ACL *pAcl;
// 	DWORD dwAclSize = sizeof(ACL) + sizeof(SYSTEM_AUDIT_ACE) + GetLengthSid(pSIDAdmin) - sizeof(DWORD) ; 
// 
// 	pAcl = (PACL)HeapAlloc(GetProcessHeap(), 0, dwAclSize);
// 
// 	if(!InitializeAcl(pAcl, dwAclSize, ACL_REVISION))
// 	{
// 		bRetval = FALSE;
// 		goto Cleanup;
// 	}	
// 
// DWORD retVal = AddAccessAllowedAce(pAcl, ACL_REVISION, GENERIC_ALL, pSIDAdmin);
// int err = GetLastError();
// 	GetExplicitEntriesFromAcl(pAcl, &num, &pac);
// 
	
//	goto Cleanup;

    
/*	if (ERROR_SUCCESS != SetEntriesInAcl(NUM_ACES, ea,  NULL,  &pACL))
    {
        printf("Failed SetEntriesInAcl\n");
        goto Cleanup;
    }
*/
    // Try to modify the object's DACL.
//     dwRes = SetNamedSecurityInfo(
//         lpszOwnFile,                 // name of the object
//         SE_FILE_OBJECT,              // type of object
//         DACL_SECURITY_INFORMATION,   // change only the object's DACL
//         NULL, NULL,                  // do not change owner or group
//         pAcl,                        // DACL specified
//         NULL);                       // do not change SACL
// 
//     if (ERROR_SUCCESS == dwRes) 
//     {
//         printf("Successfully changed DACL\n");
//         bRetval = TRUE;
//         // No more processing needed.
// 		bRetval = FALSE;
//         goto Cleanup;
//     }
//     if (dwRes != ERROR_ACCESS_DENIED)
//     {
//         printf("First SetNamedSecurityInfo call failed: %u\n",
//                 dwRes); 
// 		bRetval = FALSE;
//         goto Cleanup;
//     }

    // If the preceding call failed because access was denied, 
    // enable the SE_TAKE_OWNERSHIP_NAME privilege, create a SID for 
    // the Administrators group, take ownership of the object, and 
    // disable the privilege. Then try again to set the object's DACL.

    // Open a handle to the access token for the calling process.
/*    if (!OpenProcessToken(GetCurrentProcess(), 
                          TOKEN_ADJUST_PRIVILEGES, 
                          &hToken)) 
       {
          printf("OpenProcessToken failed: %u\n", GetLastError()); 
		bRetval = FALSE;
          goto Cleanup; 
       } 
*/
    // Enable the SE_TAKE_OWNERSHIP_NAME privilege.
/*    if (!SetPrivilege(hToken, SE_TAKE_OWNERSHIP_NAME, TRUE)) 
    {
        printf("You must be logged on as Administrator.\n");
		bRetval = FALSE;
        goto Cleanup; 
    }
*/
    // Set the owner in the object's security descriptor.
/*    dwRes = SetNamedSecurityInfo(
        lpszOwnFile,                 // name of the object
        SE_FILE_OBJECT,              // type of object
        OWNER_SECURITY_INFORMATION,  // change only the object's owner
        pSIDAdmin,                   // SID of Administrator group
        NULL,
        NULL,
        NULL); 

    if (dwRes != ERROR_SUCCESS) 
    {
        printf("Could not set owner. Error: %u\n", dwRes); 
 		bRetval = FALSE;
       goto Cleanup;
    }
    */    
    // Disable the SE_TAKE_OWNERSHIP_NAME privilege.
/*    if (!SetPrivilege(hToken, SE_TAKE_OWNERSHIP_NAME, FALSE)) 
    {
        printf("Failed SetPrivilege call unexpectedly.\n");
		bRetval = FALSE;
        goto Cleanup;
    }
*/
    // Try again to modify the object's DACL,
    // now that we are the owner.
/*    dwRes = SetNamedSecurityInfo(
        lpszOwnFile,                 // name of the object
        SE_FILE_OBJECT,              // type of object
        DACL_SECURITY_INFORMATION,   // change only the object's DACL
        NULL, NULL,                  // do not change owner or group
        pACL,                        // DACL specified
        NULL);                       // do not change SACL

    if (dwRes == ERROR_SUCCESS)
    {
        printf("Successfully changed DACL\n");
        bRetval = TRUE; 
    }
    else
    {
        printf("Second SetNamedSecurityInfo call failed: %u\n",
                dwRes); 
    }

Cleanup:

    if (pSIDAdmin)
        FreeSid(pSIDAdmin); 

    if (pSIDEveryone)
        FreeSid(pSIDEveryone); 

    if (pACL)
       LocalFree(pACL);

    if (hToken)
       CloseHandle(hToken);

	fileName.ReleaseBuffer();
    return bRetval;
*/
	return false;
}

CString ConnectionLimit::WriteConnections(DWORD conns)
{
	PVOID oldValue = NULL;
	TCHAR windir[MAX_PATH];
	GetWindowsDirectory(windir, MAX_PATH);
		
	CString patchedFiles;
	for(int i=0; i<3; ++i)
	{
		CString path(windir);
		switch(i)
		{
			case 0:
				path+=DIR_LOC;
			break;
			case 1:
				path+=DIR_LOC_1;
			break;
			case 2:
				path+=DIR_LOC_2;
			break;
		}
		int id = GetBuildID(path);
		CFile file;
		DWORD dwOffset = 0;
		dwOffset = GetOffset(path);
		bool wasPatched = true;
		if(IsWow64())
		{
			MyWow64DisableWow64FsRedirection(&oldValue);
		}
		if(file.Open(path,CFile::modeReadWrite))
		{
			if(IsWow64())
			{
				MyWow64RevertWow64FsRedirection(oldValue);
			}
			if(dwOffset)
			{
				file.Seek(dwOffset,CFile::begin);
				file.Write(&conns, sizeof(conns));
				file.Close();
				DWORD checkSum = ComputeCheckSum(path);
				patchedFiles+=path;
				patchedFiles+="\n";
			}
		}
		int err = GetLastError();
	}
	
	return patchedFiles;
}

DWORD ConnectionLimit::GetOffset(LPCTSTR path)
{
	CFile file;
	DWORD dwOffset = 0;
	bool isWow = IsWow64();
	PVOID oldValue = NULL;
	if(isWow)
	{
		MyWow64DisableWow64FsRedirection(&oldValue);
	}

	if(file.Open(path,CFile::modeRead|CFile::typeBinary))
	{
		if(isWow)
		{
			MyWow64RevertWow64FsRedirection(oldValue);
		}
		unsigned int bytesRead = 0;
		char buffer[1024];
		bytesRead = file.Read(buffer, 1024);
		while(bytesRead != 0)
		{
			for (int i = 0; i<bytesRead; ++i)
			{
				if(!isWow && (CAntiSpySettings::GetSuiteType() != CAntiSpySettings::SUITE_TYPE::WINDOWS_VISTA))
				{

					if(buffer[i] == MAGIC_32[0])
					{
						if(i>= (bytesRead - 5))
						{
							int pos = file.GetPosition();
							file.Seek(-5, CFile::current);
							bytesRead-=5;
							pos = file.GetPosition();
							break;
						}

						for (int c = 1; c < 6; ++c)
						{
							if(buffer[i+c] != MAGIC_32[c])
							{
								break;
							}
							else
							{
								if(c == 5)
								{
									dwOffset += i + 10;
									file.Close();
									return dwOffset;
								}
							}
						}
					}
				}
				else if(!isWow && (CAntiSpySettings::GetSuiteType() == CAntiSpySettings::SUITE_TYPE::WINDOWS_VISTA))
				{
					if(buffer[i] == VISTA_MAGIC_32[0])
					{
						if(i>= (bytesRead - 8))
						{
							int pos = file.GetPosition();
							file.Seek(-8, CFile::current);
							bytesRead-=8;
							pos = file.GetPosition();
							break;
						}

						for (int c = 1; c < 9; ++c)
						{
							if(buffer[i+c] != VISTA_MAGIC_32[c])
							{
								break;
							}
							else
							{
								if(c == 8)
								{
									dwOffset += i + 9;
									file.Close();
									return dwOffset;
								}
							}
						}
					}
				}
				else if(isWow && (CAntiSpySettings::GetSuiteType() != CAntiSpySettings::SUITE_TYPE::WINDOWS_VISTA))
				{
					if(buffer[i] == MAGIC_64[0])
					{
						if(i>= (bytesRead - 24))
						{
							int pos = file.GetPosition();
							file.Seek(-24, CFile::current);
							bytesRead-=24;
							pos = file.GetPosition();
							break;
						}

						for (int c = 1; c < 24; ++c)
						{
							if(buffer[i+c] != MAGIC_64[c])
							{
								break;
							}
							else
							{
								if(c == 23)
								{
									dwOffset += i + 50;
									file.Close();
									return dwOffset;
								}
							}
						}
					}

				}
			}
			dwOffset += bytesRead;
			bytesRead = file.Read(buffer, 1024);
		}
		file.Close();
	}
	return 0;
}

