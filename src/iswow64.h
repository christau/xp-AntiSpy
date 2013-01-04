typedef BOOL (WINAPI *LPFN_ISWOW64PROCESS) (HANDLE, PBOOL);
typedef BOOL (WINAPI *LPFN_Wow64DisableWow64FsRedirection)(PVOID OldValue); 
typedef BOOL (WINAPI *LPFN_Wow64RevertWow64FsRedirection)(PVOID OldValue); 

LPFN_ISWOW64PROCESS fnIsWow64Process = (LPFN_ISWOW64PROCESS)GetProcAddress(GetModuleHandle(_T("kernel32")),"IsWow64Process");
LPFN_Wow64DisableWow64FsRedirection fnWow64DisableWow64FsRedirection = (LPFN_Wow64DisableWow64FsRedirection) GetProcAddress(GetModuleHandle(_T("kernel32")), "Wow64DisableWow64FsRedirection");
LPFN_Wow64RevertWow64FsRedirection fnWow64RevertWow64FsRedirection = (LPFN_Wow64RevertWow64FsRedirection) GetProcAddress(GetModuleHandle(_T("kernel32")), "Wow64RevertWow64FsRedirection");
 
BOOL IsWow64()
{
    BOOL bIsWow64 = FALSE;
 
    if (NULL != fnIsWow64Process)
    {
        if (!fnIsWow64Process(GetCurrentProcess(),&bIsWow64))
        {
            // handle error
        }
    }
    return bIsWow64;
}

BOOL MyWow64DisableWow64FsRedirection(PVOID OldValue)
{
	if (fnWow64DisableWow64FsRedirection)
		return fnWow64DisableWow64FsRedirection(&OldValue);

	return FALSE;
}
BOOL MyWow64RevertWow64FsRedirection(PVOID OldValue)
{
	if (fnWow64RevertWow64FsRedirection)
		return fnWow64RevertWow64FsRedirection(&OldValue);

	return FALSE;
}
