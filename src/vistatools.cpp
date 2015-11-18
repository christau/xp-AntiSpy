///////////////////////////////
/* VistaTools.cxx - version 1.0

replace in winnt.h the following struct

typedef enum _TOKEN_INFORMATION_CLASS {
	    TokenUser = 1,
	    TokenGroups,
	    TokenPrivileges,
	    TokenOwner,
	    TokenPrimaryGroup,
	    TokenDefaultDacl,
	    TokenSource,
	    TokenType,
	    TokenImpersonationLevel,
	    TokenStatistics,
	    TokenRestrictedSids,
	    TokenSessionId,
	    TokenGroupsAndPrivileges,
	    TokenSessionReference,
	    TokenSandBoxInert,
	    TokenAuditPolicy,
	    TokenOrigin,
	    TokenElevationType,
	    TokenLinkedToken,
	    TokenElevation,
	    TokenHasRestrictions,
	    TokenAccessInformation,
	    TokenVirtualizationAllowed,
	    TokenVirtualizationEnabled,
	    TokenIntegrityLevel,
	    TokenUIAccess,
	    TokenMandatoryPolicy,
	    TokenLogonSid,
	    MaxTokenInfoClass  // MaxTokenInfoClass should always be the last enum


THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED
TO THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
PARTICULAR PURPOSE.

Copyright (C) 2006.  WinAbility Software Corporation. All rights reserved.

Author: Andrei Belogortseff [ http://www.tweak-uac.com ]

TERMS OF USE: You are free to use this file in any way you like, 
for both the commercial and non-commercial purposes, royalty-free,
AS LONG AS you agree with the warranty disclaimer above, 
EXCEPT that you may not remove or modify this or any of the 
preceeding paragraphs. If you make any changes, please document 
them in the MODIFICATIONS section below. If the changes are of general 
interest, please let us know and we will consider incorporating them in 
this file, as well.

If you use this file in your own project, an acknowledgement will be appreciated, 
although it's not required.

SUMMARY:

This file contains several Vista-specific functions helpful when dealing with the 
"elevation" features of Windows Vista. See the descriptions of the functions below
for information on what each function does and how to use it.

This file contains the Win32 stuff only, it can be used with or without other frameworks, 
such as MFC, ATL, etc.

HOW TO USE THIS FILE:

Make sure you have the latest Windows SDK (see msdn.microsoft.com for more information)
or this file may not compile!

This is a "combo" file that contains both the declarations (usually placed in the .h files) 
as well as the definitions (usually placed in the .cpp files) of the functions. 

To get the declarations only, include it as you would any .h file, for example:

#include "VistaTools.cxx"

To get both the declarations and definitions, define IMPLEMENT_VISTA_TOOLS before including the file:

#define IMPLEMENT_VISTA_TOOLS
#include "VistaTools.cxx"

(The above should be done once and only once per project).

The file VistaTools.cxx can be included in the VisualStudio projects, but it should be 
excluded from the build process (because its contents is compiled when it is included 
in another .cpp file with IMPLEMENT_VISTA_TOOLS defined, as shown above.)

MODIFICATIONS:
	v.1.0 (2006-Dec-16) created by Andrei Belogortseff.

*/

#if ( NTDDI_VERSION < NTDDI_LONGHORN )
#	error NTDDI_VERSION must be defined as NTDDI_LONGHORN or later
#endif

//////////////////////////////////////////////////////////////////
// if ASSERT was not defined already, lets define our own version,
// to use the CRT assert() 

#ifndef ASSERT
#	ifdef _DEBUG
#		include <assert.h>
#		define ASSERT(x) assert( x )
#		define ASSERT_HERE assert( FALSE )
#	else// _DEBUG
#		define ASSERT(x) 
#	endif//_DEBUG
#endif//ASSERT


#include <comdef.h>

/*typedef enum _TOKEN_ELEVATION_TYPE {
    TokenElevationTypeDefault = 1,
    TokenElevationTypeFull,
    TokenElevationTypeLimited,
} TOKEN_ELEVATION_TYPE, *PTOKEN_ELEVATION_TYPE;

^*/
#ifndef VS2015
typedef struct _TOKEN_ELEVATION {
    DWORD TokenIsElevated;
} TOKEN_ELEVATION, *PTOKEN_ELEVATION;
#endif

BOOL IsVista();

HRESULT 
IsElevated(BOOL * pbElevated = NULL );

/*
Use IsElevated() to determine whether the current process is elevated or not.

Parameters:

pbElevated
	[out] [optional] Pointer to a BOOL variable that, if non-NULL, receives the result.

	The possible values are:

	TRUE - the current process is elevated.
		This value indicates that either UAC is enabled, and the process was elevated by 
		the administrator, or that UAC is disabled and the process was started by a user 
		who is a member of the Administrators group.

	FALSE - the current process is not elevated (limited).
		This value indicates that either UAC is enabled, and the process was started normally, 
		without the elevation, or that UAC is disabled and the process was started by a standard user. 

Return Values
	If the function succeeds, and the current process is elevated, the return value is S_OK. 
	If the function succeeds, and the current process is not elevated, the return value is S_FALSE. 
	If the function fails, the return value is E_FAIL. To get extended error information, 
	call GetLastError().
*/

#ifdef IMPLEMENT_VISTA_TOOLS

BOOL IsVista()
{
	OSVERSIONINFO osver;

	osver.dwOSVersionInfoSize = sizeof( OSVERSIONINFO );
	
	if (	::GetVersionEx( &osver ) && 
			osver.dwPlatformId == VER_PLATFORM_WIN32_NT && 
			(osver.dwMajorVersion >= 6 ) )
		return TRUE;

	return FALSE;
}

HRESULT IsElevated( BOOL * pbElevated ) //= NULL )
{
	ASSERT( IsVista() );

	HRESULT hResult = E_FAIL; // assume an error occured
	HANDLE hToken	= NULL;

	if ( !::OpenProcessToken( 
				::GetCurrentProcess(), 
				TOKEN_QUERY, 
				&hToken ) )
	{
		ASSERT( FALSE );
		return hResult;
	}

	TOKEN_ELEVATION te = { 0 };
	DWORD dwReturnLength = 0;

	if ( !::GetTokenInformation(
				hToken,
				TokenElevation,
				&te,
				sizeof( te ),
				&dwReturnLength ) )
	{
		ASSERT( FALSE );
	}
	else
	{
		ASSERT( dwReturnLength == sizeof( te ) );

		hResult = te.TokenIsElevated ? S_OK : S_FALSE; 

		if ( pbElevated)
			*pbElevated = (te.TokenIsElevated != 0);
	}

	::CloseHandle( hToken );

	return hResult;
}

#endif// IMPLEMENT_VISTA_TOOLS